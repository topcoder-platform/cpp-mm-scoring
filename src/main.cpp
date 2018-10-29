/*
 * Copyright (C) 2018 TopCoder Inc., All Rights Reserved.
 */
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "Runner.h"
#include "MemoryMonitor.h"
using namespace std;
using namespace nlohmann;

// Load a text file into a std::string.
string readWholeFile(const string& filePath) {
    ifstream in(filePath);
    ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

// Run a cpp source file against the given signature and data file.
void runTestCodeAndData(string testDir, const string& cppName,
                        const string& strSignature, const string& inputName) {
    // Load data.
    if (testDir.at(testDir.size()-1) == '/') {
        testDir = testDir.substr(0, testDir.size()-1);
    }
    string code = readWholeFile(testDir + cppName);
    json signature = json::parse(strSignature);
    string strInput = readWholeFile(testDir+inputName);
    json input = json::parse(strInput);

    // Run code in cling interpreter.
    pid_t pid = getpid();
    MemoryMonitor memoryMonitor(100, (int)pid);
    memoryMonitor.start();
    Runner runner(signature);
    runner.setMemoryMonitor(&memoryMonitor);
    json results = runner.runSubmission(input, code);
    cout << "Test code: " << cppName << " ..." << endl;
    cout << "Input data: " << inputName << " ..." << endl;
    cout << "Here is the output json:" << endl;
    cout << results.dump(4) << endl;
}

// Test case 1.
void testFullParameters(const string& testDir) {
    string signature = R"(
{
"input": ["int", "double", "string", "vector<int>", "vector<double>", "vector<string>"],
"output": "int",
"className": "MockedClass",
"method": "testFullParameter"
}
)";
    runTestCodeAndData(testDir, "/MockedClass.cpp", signature, "/FullParametersData.txt");
}

// Test case 2.
void testInvalidClassDefinition(const string& testDir) {
    string signature = R"(
{
"input": ["int"],
"output": "int",
"className": "InvalidClass",
"method": "method"
}
)";
    runTestCodeAndData(testDir, "/InvalidClass.cpp", signature, "/InvalidClassData.txt");
}

// Test case 3.
void testVectorIntInOutSort(const string& testDir) {
    string signature = R"(
{
"input": ["std::vector<int>"],
"output": "std::vector<int>",
"className": "MockedClass",
"method": "testVectorIntInOutSort"
}
)";
    runTestCodeAndData(testDir, "/MockedClass.cpp", signature, "/VectorIntInOutSortData.txt");
}

// Test case 4.
void testVectorDoubleInOutX2(const string& testDir) {
    string signature = R"(
{
"input": ["std::vector<double>"],
"output": "std::vector<double>",
"className": "MockedClass",
"method": "testVectorDoubleInOutX2"
}
)";
    runTestCodeAndData(testDir, "/MockedClass.cpp", signature, "/VectorDoubleInOutX2Data.txt");
}

// Test case 5.
void testVectorStrInOutReverse(const string& testDir) {
    string signature = R"(
{
"input": ["std::vector<string>"],
"output": "std::vector<string>",
"className": "MockedClass",
"method": "testVectorStrInOutReverse"
}
)";
    runTestCodeAndData(testDir, "/MockedClass.cpp", signature, "/VectorStrInOutReverseData.txt");
}

// Test case 6.
void testSignatureMethodMismatch(const string& testDir) {
    string signature = R"(
{
"input": ["std::vector<string>", "double"],
"output": "std::vector<string>",
"className": "MockedClass",
"method": "testVectorStrInOutReverse"
}
)";
    runTestCodeAndData(testDir, "/MockedClass.cpp", signature, "/VectorStrInOutReverseData.txt");
}

// Test case 7.
void testReturnComplexStrAsItIs(const string& testDir) {
    string signature = R"(
{
"input": ["string"],
"output": "std::string",
"className": "MockedClass",
"method": "testReturnComplexStrAsItIs"
}
)";
    runTestCodeAndData(testDir, "/MockedClass.cpp", signature, "/ReturnComplexStrAsItisData.txt");
}

// Test case 8.
void testMemoryUsage(const string& testDir) {
    string signature = R"(
{
"input": ["int"],
"output": "int",
"className": "MockedClass",
"method": "testMemoryUsage"
}
)";
    runTestCodeAndData(testDir, "/MockedClass.cpp", signature, "/MemoryUsageData.txt");
}

// Test case 9.
void testTimeUsage(const string& testDir) {
    string signature = R"(
{
"input": ["int"],
"output": "int",
"className": "MockedClass",
"method": "testTimeUsage"
}
)";
    runTestCodeAndData(testDir, "/MockedClass.cpp", signature, "/TimeUsageData.txt");
}

// Usage.
void printUsage() {
    cout << "Usage: ./MarathonMatchScoring <test_files_path> <demo_index>" <<endl;
    cout << "Example: ./MarathonMatchScoring ../test_files 3" << endl;
    exit(-1);
}

// The main function.
int main(int argc, const char* const* argv) {
    if (argc != 3) {
        printUsage();
    }

    string testDir = argv[1];
    int idx = atoi(argv[2]);
    switch (idx) {
        case 1:
            testFullParameters(testDir);
            break;
        case 2:
            testInvalidClassDefinition(testDir);
            break;
        case 3:
            testVectorIntInOutSort(testDir);
            break;
        case 4:
            testVectorDoubleInOutX2(testDir);
            break;
        case 5:
            testVectorStrInOutReverse(testDir);
            break;
        case 6:
            testSignatureMethodMismatch(testDir);
            break;
        case 7:
            testReturnComplexStrAsItIs(testDir);
            break;
        case 8:
            testMemoryUsage(testDir);
            break;
        case 9:
            testTimeUsage(testDir);
            break;
        default:
            printUsage();
            cout << "Invalid demo index: "<< argv[2] <<". Exting ..." << endl;
    }
}
