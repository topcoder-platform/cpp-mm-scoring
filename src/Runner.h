/*
 * Copyright (C) 2018 TopCoder Inc., All Rights Reserved.
 */

#ifndef MARATHONMATCHSCORING_RUNNER_H
#define MARATHONMATCHSCORING_RUNNER_H

#include <string>
#include <nlohmann/json.hpp>
#include <map>

namespace clang {
    class TextDiagnosticBuffer;
    class SourceManager;
}

namespace cling {
    class Value;
}

class MemoryMonitor;

class Runner {
public:
    explicit Runner(nlohmann::json signature);
    nlohmann::json runSubmission(nlohmann::json data,
                                 std::string& submissionCode);

    // Assign the memory monitor. So that we can track the memory usage properly.
    void setMemoryMonitor(MemoryMonitor* pMM);

private:
    enum ValueType {
        vt_int,
        vt_double,
        vt_string,
        vt_vector_int,
        vt_vector_double,
        vt_vector_string
    };

    // Validate the signature is of correct format.
    void validateSignature(const nlohmann::json& s);

    // Validate the input data is consistent with the signature.
    void validateInputData(const nlohmann::json& inputItems);

    // Construct the default json structure so that we can fill more details later.
    static nlohmann::json constructDefaultRetJson(unsigned long numTest);

    // Extract cling interpreting errors as a std::string.
    static std::string fillClingErrors(const clang::TextDiagnosticBuffer* buffer,
                                       const clang::SourceManager& sm,
                                       const std::string& prefix);

    // Check that the json object contains the specified key.
    static void checkJsonKey(const nlohmann::json& j, const std::string& name, const std::string& key);

    // Check that the json object is of required type.
    static void checkJsonType(const nlohmann::json& j, nlohmann::json::value_t _type,
                              bool strict=false);

    // Convert the input parameters into c++ statement.
    std::string convertInputToStatement(const nlohmann::json& para,
                                               const std::string& decl_type,
                                               int paraIdx);

    // Convert the output value into c++ statement.
    std::string getOutputStatement(const std::string& varName);

    // Convert the cling output value into a json object.
    nlohmann::json castOutputToJson(const cling::Value& value);

    // Get string value from a json object.
    static std::string safeGetString(nlohmann::json j);


private:
    // The method signature.
    nlohmann::json signature;

    // Size of parameter list.
    int numInput;

    // Valid data types. e.g. int, double, string, vector<int> etc.
    std::map<std::string, ValueType> validValueTypes;

    // Variable prefix to avoid conflict with potential global variables.
    std::string variablePrefix;

    // Used to monitor the memory usage.
    MemoryMonitor* pMemoryMonitor;
};


#endif //MARATHONMATCHSCORING_RUNNER_H
