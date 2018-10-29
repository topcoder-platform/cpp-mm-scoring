/*
 * Copyright (C) 2018 TopCoder Inc., All Rights Reserved.
 */

#include "Runner.h"
#include <clang/Frontend/TextDiagnosticBuffer.h>
#include <clang/Frontend/CompilerInstance.h>
#include <cling/Interpreter/Interpreter.h>
#include <cling/Interpreter/Value.h>
#include <cling/Interpreter/Transaction.h>
#include <cling/Utils/Casting.h>
#include <exception>
#include <iostream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include "MemoryMonitor.h"
using namespace cling;
using namespace clang;
using namespace nlohmann;
using namespace std;

Runner::Runner(json signature) {
    validValueTypes["int"] = ValueType::vt_int;
    validValueTypes["double"] = ValueType::vt_double;
    validValueTypes["std::string"] = ValueType::vt_string;
    validValueTypes["string"] = ValueType::vt_string;
    validValueTypes["std::vector<int>"] = ValueType::vt_vector_int;
    validValueTypes["vector<int>"] = ValueType::vt_vector_int;
    validValueTypes["std::vector<double>"] = ValueType::vt_vector_double;
    validValueTypes["vector<double>"] = ValueType::vt_vector_double;
    validValueTypes["std::vector<std::string>"] = ValueType::vt_vector_string;
    validValueTypes["std::vector<string>"] = ValueType::vt_vector_string;
    validValueTypes["vector<std::string>"] = ValueType::vt_vector_string;
    validValueTypes["vector<string>"] = ValueType::vt_vector_string;
    validateSignature(signature);
    this->signature = signature;
    this->numInput = signature["input"].size();
    this->variablePrefix = "__TCMM__NEVER_DEFINE_THIS_";
    this->pMemoryMonitor = nullptr;
}

json Runner::runSubmission(json data, string& submissionCode) {
    // Check the signature compatibility.
    validateInputData(data);
    // Construct the JSON return value.
    json retVal = constructDefaultRetJson(data.size());

    // Construct cling interpreter.
    static const char* argv[1] = {"app"};
    Interpreter interpreter(1, argv, LLVMDIR);
    CompilerInstance* ci = interpreter.getCI();
    TextDiagnosticBuffer* buffer = new TextDiagnosticBuffer();
    ci->getDiagnostics().setClient(buffer, true);
    const SourceManager& sm = ci->getSourceManager();
    Interpreter::CompilationResult cr;
    double baseMemoryUsage = 0.0;

    // Declare the class definition.
    try {
        // We suggest this declaration would never fail.
        interpreter.declare("#include <chrono>");
        // Let's record the memory usage as a base, before compiling and running the test code.
        if (pMemoryMonitor != nullptr) {
            pMemoryMonitor->checkMemory(true);
            baseMemoryUsage = ((double)pMemoryMonitor->getMaxMemory())/1024.0;
        }
        // Load competitor's code.
        cr = interpreter.declare(submissionCode);
        if (cr != Interpreter::CompilationResult::kSuccess) {
            retVal["error"] = fillClingErrors(buffer, sm, "Compilation time error: ");
            return retVal;
        }
    } catch (exception& e) {
        retVal["error"] = string("Compilation time error: ") + e.what();
        return retVal;
    } catch (...) {
        retVal["error"] = "Unknown compilation time error";
        return retVal;
    }

    // Evaluate each input item.
    int idx = -1;
    vector<double> allTimeUsage, allMemoryUsage;
    for (auto& element : data) {
        try {
            ++idx;
            string outputName = variablePrefix + string("out_round_") + to_string(idx);
            string timeUsageName = variablePrefix + string("duration_round_") + to_string(idx);
            string timeStartName = variablePrefix + string("start");
            string timeEndName = variablePrefix + string("end");
            string memoryUsageName = variablePrefix + string("memory_round_") + to_string(idx);

            // Construct C++ 'script' to run.
            ostringstream ss, run_ss;
            string outputType = safeGetString(signature["output"]);
            run_ss << getOutputStatement(outputName) << "\n";
            run_ss << "std::chrono::microseconds " << timeUsageName << ";\n";
            run_ss << "{\n";
            run_ss << "std::chrono::high_resolution_clock::time_point "
                      << timeStartName << " = std::chrono::high_resolution_clock::now();\n";
            ss << outputName << " = " << safeGetString(signature["className"]) << "().";
            ss << safeGetString(signature["method"]) << "(";
            for (int i=0; i<numInput; ++i) {
                string statement = convertInputToStatement(element["input"][i], signature["input"][i], i);
                run_ss << statement << endl;
                ss << variablePrefix << "para_" << i;
                if (i < numInput-1) {
                    ss << ", ";
                }
            }
            ss << ");";
            run_ss << ss.str() << "\n";
            run_ss << "std::chrono::high_resolution_clock::time_point "
                      << timeEndName << " = std::chrono::high_resolution_clock::now();\n";
            run_ss << timeUsageName << " =  std::chrono::duration_cast<std::chrono::microseconds>("
                                       << timeEndName << " - " << timeStartName << ");\n";
            run_ss << "}\n";
            run_ss << outputName << ";\n";

            // Run the code and collect return values, time usage etc.
            Value output;
            if (pMemoryMonitor != nullptr) {
                // Refresh the memory usage explicitly. And reset the max memory usage
                // for each evaluation iteration.
                pMemoryMonitor->checkMemory(true);
            }
            cr = interpreter.process(run_ss.str(), &output);
            if (cr != Interpreter::CompilationResult::kSuccess) {
                retVal["results"][idx]["error"] = fillClingErrors(buffer, sm, "Runtime error: ");
                continue;
            }
            retVal["results"][idx]["output"] = castOutputToJson(output);

            cr = interpreter.process(timeUsageName + ".count();\n", &output);
            if (cr != Interpreter::CompilationResult::kSuccess) {
                retVal["results"][idx]["error"] = fillClingErrors(buffer, sm, "Runtime error: ");
                continue;
            }
            double ms = ((double)output.getLL())/1000.0;
            allTimeUsage.push_back(ms);
            retVal["results"][idx]["statistics"]["time"] = ms;
            if (pMemoryMonitor != nullptr) {
                // Check the memory usage and remove the baseMemoryUsage.
                pMemoryMonitor->checkMemory(false);
                double maxMemory = ((double)(pMemoryMonitor->getMaxMemory())) / 1024.0;
                maxMemory -= baseMemoryUsage;
                if (maxMemory < 0) {
                    maxMemory = 0.0;
                }
                retVal["results"][idx]["statistics"]["memory"] = maxMemory;
                allMemoryUsage.push_back(maxMemory);
            }
        } catch (exception& e) {
            retVal["results"][idx]["error"] = string("Runtime error: ") + e.what();
        } catch (...) {
            retVal["results"][idx]["error"] = "Unknown runtime error";
        }
    }

    // Calculate the global statistics.
    if (!allTimeUsage.empty()) {
        retVal["statistics"]["time"]["max"] = *std::max_element(allTimeUsage.begin(), allTimeUsage.end());
        retVal["statistics"]["time"]["min"] = *std::min_element(allTimeUsage.begin(), allTimeUsage.end());
        retVal["statistics"]["time"]["mean"] = std::accumulate(allTimeUsage.begin(),
                                                               allTimeUsage.end(), 0.0)/allTimeUsage.size();
    }
    if (!allMemoryUsage.empty()) {
        retVal["statistics"]["memory"]["max"] = *std::max_element(allMemoryUsage.begin(), allMemoryUsage.end());
        retVal["statistics"]["memory"]["min"] = *std::min_element(allMemoryUsage.begin(), allMemoryUsage.end());
        retVal["statistics"]["memory"]["mean"] = std::accumulate(allMemoryUsage.begin(),
                                                               allMemoryUsage.end(), 0.0)/allMemoryUsage.size();
    }

    // We are done.
    return retVal;
}

nlohmann::json Runner::constructDefaultRetJson(unsigned long numTest) {
    json retVal = R"(
        {
            "statistics": {
                "time": {
                    "mean": null,
                    "max": null,
                    "min": null
                },
                "memory": {
                    "mean": null,
                    "max": null,
                    "min": null
                }
            },
            "results": [
            ]
        }
        )"_json;
    json oneRet = R"(
        {
            "statistics": {
                "time": null,
                "memory": null
             },
             "output": null
        }
        )"_json;
    for (unsigned long i=0; i<numTest; ++i) {
        retVal["results"].push_back(oneRet);
    }
    return retVal;
}

string Runner::fillClingErrors(const TextDiagnosticBuffer *buffer,
                               const SourceManager &sm,
                               const string& prefix) {
    string errorMsg = prefix;
    for (auto itr=buffer->err_begin();
         itr != buffer->err_end(); ++itr) {
        //errorMsg += (itr->first.printToString(sm));
        //errorMsg += ": ";
        errorMsg += (itr->second);
        errorMsg += "\n";
    }
    return errorMsg;
}

void Runner::validateSignature(const nlohmann::json &s) {
    checkJsonKey(s, "signature", "input");
    checkJsonKey(s, "signature", "output");
    checkJsonKey(s, "signature", "className");
    checkJsonKey(s, "signature", "method");
    checkJsonType(s["input"], json::value_t::array);
    checkJsonType(s["output"], json::value_t::string, true);
    checkJsonType(s["className"], json::value_t::string, true);
    checkJsonType(s["method"], json::value_t::string, true);
    for (auto& para : s["input"]) {
        checkJsonType(para, json::value_t::string, true);
        if (validValueTypes.find(para) == validValueTypes.end()) {
            throw runtime_error(string("input value type <") + safeGetString(para) + "> is not accepted");
        }
    }
    string outputType = safeGetString(s["output"]);
    if (validValueTypes.find(outputType) == validValueTypes.end()) {
        throw runtime_error(string("output value type <") + outputType + "> is not accepted");
    }
}

void Runner::validateInputData(const json &inputItems) {
    checkJsonType(inputItems, json::value_t::array);
    for (auto& item : inputItems) {
        checkJsonKey(item, "inputItem", "input");
        json input = item["input"];
        checkJsonType(input, json::value_t::array);
        if (input.size() != (unsigned)this->numInput) {
            throw runtime_error(string("please check the input size of ") + input.dump());
        }
        for (int i=0; i<numInput; ++i) {
            // Check if the input parameters are of correct format.
            convertInputToStatement(input[i], signature["input"][i], i);
        }
    }
}

void Runner::checkJsonKey(const json &j, const string &name, const string &key) {
    if (j.find(key) == j.end()) {
        throw runtime_error(name + " is lack of '" + key + "' key");
    }
}

void Runner::checkJsonType(const json &j, json::value_t _type,
                           bool strict/*=false*/) {
    json::value_t actual = j.type();
    if (actual != _type) {
        if (strict && _type == json::value_t::string && actual == json::value_t::null) {
            throw runtime_error("string value must not be null under strict mode");
        } else if (actual == json::value_t::null &&
                (_type == json::value_t::string ||
                _type == json::value_t::object)) {
            // pass
        } else if (j.is_number() &&
                (_type == json::value_t::number_float ||
                _type == json::value_t::number_integer ||
                _type == json::value_t::number_unsigned)) {
            // pass
        } else {
            throw runtime_error(string("incompatible json type, details: ") + j.dump());
        }
    } else if (strict && actual == json::value_t::string) {
        string val = safeGetString(j);
        if (val.empty()) {
            throw runtime_error("string value must not be empty under strict mode");
        }
    }
}

std::string Runner::convertInputToStatement(const nlohmann::json &para, const std::string &decl_type,
                                            int paraIdx) {
    ostringstream ss;
    string paraName = variablePrefix + string("para_") + to_string(paraIdx);
    if (validValueTypes.find(decl_type) == validValueTypes.end()) {
        throw runtime_error(string("input value type <") + decl_type + "> is not accepted");
    }
    const ValueType& valueType = validValueTypes[decl_type];
    if (valueType == ValueType::vt_int) {
        if (para.is_number()) {
            ss << "int " << paraName << " = " << para.get<int>() << ";";
            return ss.str();
        } else {
            ss << "incompatible int parameter_" << paraIdx <<  ": " << para.get<string>();
            throw runtime_error(ss.str());
        }
    } else if (valueType == ValueType::vt_double) {
        if (para.is_number()) {
            ss << "double " << paraName << " = " << para.get<double>() << ";";
            return ss.str();
        } else {
            ss << "incompatible double parameter_" << paraIdx <<  ": " << para.get<string>();
            throw runtime_error(ss.str());
        }
    } else if (valueType == ValueType::vt_vector_int) {
        checkJsonType(para, json::value_t::array);
        ss << "std::vector<int> " << paraName << ";";
        for (auto& ele : para) {
            checkJsonType(ele, json::value_t::number_integer, true);
            ss << paraName << ".push_back((int)" << ele.get<int>() << ");";
        }
        return ss.str();
    } else if (valueType == ValueType::vt_vector_double) {
        checkJsonType(para, json::value_t::array);
        ss << "std::vector<double> " << paraName << ";";
        for (auto& ele : para) {
            checkJsonType(ele, json::value_t::number_float, true);
            ss << paraName << ".push_back((double)" << ele.get<double>() << ");";
        }
        return ss.str();
    } else if (valueType == ValueType::vt_string) {
        if (para.is_string()) {
            ss << "std::string " << paraName << " = " << para.dump() << ";";
            return ss.str();
        } else if (para.is_null()) {
            // Allow empty strings.
            ss << "std::string " << paraName << " = \"\"";
            return ss.str();
        } else {
            ss << "incompatible string parameter_" << paraIdx <<  ": " << para.dump();
            throw runtime_error(ss.str());
        }
    } else if (valueType == ValueType::vt_vector_string) {
        checkJsonType(para, json::value_t::array);
        ss << "std::vector<std::string> " << paraName << ";";
        for (auto& ele : para) {
            if (ele.is_string()) {
                ss << paraName << ".push_back(" << ele.dump() << ");";
            } else if (ele.is_null()) {
                ss << paraName << ".push_back(\"\");";
            } else {
                ss.str("");
                ss.clear();
                ss << "incompatible vector<string> parameter_" << paraIdx <<  ": " << ele.dump();
                throw runtime_error(ss.str());
            }
        }
        return ss.str();
    } else {
        throw runtime_error(string("expected declaration value type ") + decl_type);
    }
}

string Runner::getOutputStatement(const string& varName) {
    string outputType = safeGetString(signature["output"]);
    if (validValueTypes.find(outputType) == validValueTypes.end()) {
        throw runtime_error(string("output value type <") + outputType + "> is not accepted");
    }
    const ValueType& valueType = validValueTypes[outputType];
    ostringstream ss;
    if (valueType == ValueType::vt_int) {
        ss << "int " << varName << ";";
        return ss.str();
    } else if (valueType == ValueType::vt_double) {
        ss << "double " << varName << ";";
        return ss.str();
    } else if (valueType == ValueType::vt_vector_int) {
        ss << "std::vector<int> " << varName << ";";
        return ss.str();
    } else if (valueType == ValueType::vt_vector_double) {
        ss << "std::vector<double> " << varName << ";";
        return ss.str();
    } else if (valueType == ValueType::vt_string) {
        ss << "std::string " << varName << ";";
        return ss.str();
    } else if (valueType == ValueType::vt_vector_string) {
        ss << "std::vector<std::string> " << varName << ";";
        return ss.str();
    } else {
        throw runtime_error(string("unexpected output type: ") + outputType);
    }
}

json Runner::castOutputToJson(const Value &value) {
    string outputType = safeGetString(signature["output"]);
    if (validValueTypes.find(outputType) == validValueTypes.end()) {
        throw runtime_error(string("output value type <") + outputType + "> is not accepted");
    }
    const ValueType& valueType = validValueTypes[outputType];
    json ret;
    if (!value.hasValue()) {
        ret = nullptr;
    } else if (valueType == ValueType::vt_int) {
        ret = (int)(value.getAs<long long>());
    } else if (valueType == ValueType::vt_double) {
        ret = value.getDouble();
    } else if (valueType == ValueType::vt_vector_int) {
        const vector<int>* pv = value.simplisticCastAs<vector<int>*>();
        ret = *pv;
    } else if (valueType == ValueType::vt_vector_double) {
        const vector<double>* pv = value.simplisticCastAs<vector<double>*>();
        ret = *pv;
    } else if (valueType == ValueType::vt_string) {
        const string* ps = value.simplisticCastAs<string*>();
        ret = *ps;
    } else if (valueType == ValueType::vt_vector_string) {
        const vector<string>* pv = value.simplisticCastAs<vector<string>*>();
        ret = *pv;
    } else {
        throw runtime_error(string("unexpected output type: ") + outputType);
    }
    return ret;
}

std::string Runner::safeGetString(nlohmann::json j) {
    if (j.is_null()) {
        return "";
    } else if (j.is_string()) {
        string ret = j.get<string>();
        return ret;
    } else {
        return "";
    }
}

void Runner::setMemoryMonitor(MemoryMonitor *pMM) {
    this->pMemoryMonitor = pMM;
}

