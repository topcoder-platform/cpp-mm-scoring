## Tool Usage

The tool MarathonMatchScoring is just a demo that displays several usage of the interpretation tool.

You can run old C++ version tool in `demo/MarathonMatchScoring` and follow exist usage.
Usage:

```
cd demo
./MarathonMatchScoring <test_files_path> <demo_index>
```

You can run new node addons version tool in `build/Release/MarathonMatchScoring.node` in root directory.
```
bash scripts/demo.sh <demo_index>
```

For example you can compare result `./demo/MarathonMatchScoring test_files 1` and `bash scripts/demo.sh 1`.
I did not change example commands later because new version will be much easier to test.

- **test\_files\_path** represents path to the `test_files` folder of this submission. This folder contains some test CPP code and some input data of JSON format.
- **demo\_index** represents the index of different test cases. It should be an integer starts from 1.

Example:

```
./demo/MarathonMatchScoring test_files 3
```
In this submission, test cases are provided as below. Here's a brief introduction of their design rationale.


| No. | Name                       | Description                              |
| :--- | -------- | ---------------------------------------- |
| 1 | testFullParameters | This test case validates that the tool supports all the required data types, i.e. int, double, string, vector\<int\>, vector\<double\>, vector\<string\>. Meanwhile, it validates that the tool may handle runtime errors properly. |
| 2 | testInvalidClassDefinition | This test case validates that the tool could detect compilation errors and respond properly. |
| 3 | testVectorIntInOutSort | This test case validates that the tool may handle return value of type **vector\<int\>** correctly. And the invoked method contains **auto** keyword to indicate that the interpreter supports c++11. |
| 4 | testVectorDoubleInOutX2 | This test case validates that the tool may handle return value of type **vector\<double\>** correctly. And the invoked method contains **lambda** function to indicate that the interpreter supports c++11 quite well. |
| 5 | testVectorStrInOutReverse | This test case validates that the tool may handle return value of type **vector\<string\>** correctly. |
| 6 | testSignatureMethodMismatch | This test case validates that the tool could detect mismatch between method signature and test data. In that case, the application exits with some error message. |
| 7 | testReturnComplexStrAsItIs | This test case validates that the tool could handle special characters within a string properly. Special characters includes <\">, <\")>, <\\> etc. |
| 8 | testMemoryUsage | This test case validates that the memory usage could be estimated quite accurately. |
| 9 | testTimeUsage | This test case validates that the costed time could be estimated quite accurately. |

## Validation results

### testFullParameters

The signature:

```
{
"input": ["int", "double", "string", "vector<int>", "vector<double>", "vector<string>"],
"output": "int",
"className": "MockedClass",
"method": "testFullParameter"
}
```

The input data:

```
[
{
"input": [1, 2.0, "hello", [4, 3, 2], [1.0, 2.0, 3.0], ["test", "string", "array"]]
},
{
"input": [1, 2.0, "INVALID", [4, 3, 2], [1.0, 2.0, 3.0], ["test", "string", "array"]]
},
{
"input": [1, 2.0, "hello", [], [1.0, 2.0, 3.0], ["test", "string", "array"]]
},
{
"input": [1, 2.0, "hello", [4, 3, 2], [], ["test", "string", "array"]]
},
{
"input": [1, -5.0, "hello", [4, 3, 2], [1.0, 2.0], []]
}
]
```

The execution output:

```
$ ./MarathonMatchScoring ../test_files/ 1
Test code: /MockedClass.cpp ...
Input data: /FullParametersData.txt ...
Here is the output json:
{
    "results": [
        {
            "output": 26,
            "statistics": {
                "memory": 29196.0,
                "time": 1004.488
            }
        },
        {
            "error": "Runtime error: string parameter is invalid",
            "output": null,
            "statistics": {
                "memory": null,
                "time": null
            }
        },
        {
            "error": "Runtime error: vector<int> must not be empty",
            "output": null,
            "statistics": {
                "memory": null,
                "time": null
            }
        },
        {
            "error": "Runtime error: vector<double> must not be empty",
            "output": null,
            "statistics": {
                "memory": null,
                "time": null
            }
        },
        {
            "output": 13,
            "statistics": {
                "memory": 32400.0,
                "time": 1003.733
            }
        }
    ],
    "statistics": {
        "memory": {
            "max": 32400.0,
            "mean": 30798.0,
            "min": 29196.0
        },
        "time": {
            "max": 1004.488,
            "mean": 1004.1105,
            "min": 1003.733
        }
    }
}
```

Here input data #2-#4 meet some runtime errors. Those errors are dumped into the *error* field properly. What's more, input data #5 continues to run correctly despite of failures from #2 to #4.

### testInvalidClassDefinition

The signature:

```
{
"input": ["int"],
"output": "int",
"className": "InvalidClass",
"method": "method"
}
```

The input data:

```
[
{
"input": [3]
}
]
```

The execution output:

```
$ ./MarathonMatchScoring ../test_files/ 2
Test code: /InvalidClass.cpp ...
Input data: /InvalidClassData.txt ...
Here is the output json:
{
    "error": "Compilation time error: use of undeclared identifier 'THIS_IS_AN_INVALID_LINE_THAT_WOULD_CAUSE_COMPILATION_ERROR'\n",
    "results": [
        {
            "output": null,
            "statistics": {
                "memory": null,
                "time": null
            }
        }
    ],
    "statistics": {
        "memory": {
            "max": null,
            "mean": null,
            "min": null
        },
        "time": {
            "max": null,
            "mean": null,
            "min": null
        }
    }
}
```

The source code of `InvalidClass.cpp` has some compilation issues. Thus a global error message is returned. And the input data won't be evaluated.

### testVectorIntInOutSort

The signature:

```
{
"input": ["std::vector<int>"],
"output": "std::vector<int>",
"className": "MockedClass",
"method": "testVectorIntInOutSort"
}
```

The input data:

```
[
{
"input": [[8, 5, 4, 16, -2, 55]]
}
]
```


The execution output:

```
$ ./MarathonMatchScoring ../test_files/ 3
Test code: /MockedClass.cpp ...
Input data: /VectorIntInOutSortData.txt ...
Here is the output json:
{
    "results": [
        {
            "output": [
                -2,
                4,
                5,
                8,
                16,
                55
            ],
            "statistics": {
                "memory": 26620.0,
                "time": 0.06
            }
        }
    ],
    "statistics": {
        "memory": {
            "max": 26620.0,
            "mean": 26620.0,
            "min": 26620.0
        },
        "time": {
            "max": 0.06,
            "mean": 0.06,
            "min": 0.06
        }
    }
}
```

The invoked method sorts a given **vector<int>** array as shown above.

### testVectorDoubleInOutX2

The signature:

```
{
"input": ["std::vector<double>"],
"output": "std::vector<double>",
"className": "MockedClass",
"method": "testVectorDoubleInOutX2"
}
```

The input data:

```
[
{
"input": [[8, 5, 4, 16, -2, 55]]
}
]
```

The execution output:

```
$ ./MarathonMatchScoring ../test_files/ 4
Test code: /MockedClass.cpp ...
Input data: /VectorDoubleInOutX2Data.txt ...
Here is the output json:
{
    "results": [
        {
            "output": [
                16.0,
                10.0,
                8.0,
                32.0,
                -4.0,
                110.0
            ],
            "statistics": {
                "memory": 26864.0,
                "time": 0.057
            }
        }
    ],
    "statistics": {
        "memory": {
            "max": 26864.0,
            "mean": 26864.0,
            "min": 26864.0
        },
        "time": {
            "max": 0.057,
            "mean": 0.057,
            "min": 0.057
        }
    }
}
```

The invoked method times 2.0 to each element of the given **vector<double>** array as shown above.

### testVectorStrInOutReverse

The signature:

```
{
"input": ["std::vector<string>"],
"output": "std::vector<string>",
"className": "MockedClass",
"method": "testVectorStrInOutReverse"
}
```

The input data:

```
[
{
"input": [["test", "string", "array"]]
}
]
```

The execution output:

```
$ ./MarathonMatchScoring ../test_files/ 5
Test code: /MockedClass.cpp ...
Input data: /VectorStrInOutReverseData.txt ...
Here is the output json:
{
    "results": [
        {
            "output": [
                "array",
                "string",
                "test"
            ],
            "statistics": {
                "memory": 26728.0,
                "time": 0.054
            }
        }
    ],
    "statistics": {
        "memory": {
            "max": 26728.0,
            "mean": 26728.0,
            "min": 26728.0
        },
        "time": {
            "max": 0.054,
            "mean": 0.054,
            "min": 0.054
        }
    }
}
```

The invoked method reverses a given **vector<string>** array as shown above.

### testSignatureMethodMismatch

The signature:

```
{
"input": ["std::vector<string>", "double"],
"output": "std::vector<string>",
"className": "MockedClass",
"method": "testVectorStrInOutReverse"
}
```

The input data:

```
[
{
"input": [["test", "string", "array"]]
}
]
```

The execution output:

```
$ ./MarathonMatchScoring ../test_files/ 6
libc++abi.dylib: terminating with uncaught exception of type std::runtime_error: please check the input size of [["test","string","array"]]
Abort trap: 6
```

In this case, the signature requires input of format `["std::vector<string>", "double"]`. But the actual input data does not contain the 2nd parameter. Thus the application exits with error message printed.

### testReturnComplexStrAsItIs

The signature:

```
{
"input": ["string"],
"output": "std::string",
"className": "MockedClass",
"method": "testReturnComplexStrAsItIs"
}
```

The input data:

```
[
{
"input": ["This string contains a \\ character."]
},
{
"input": ["This string contains a \" character."]
},
{
"input": ["This string contains a )\" sequence."]
},
 {
 "input": ["This string contains a R\"()\" sequence."]
 }
]
```

The execution output:

```
$ ./MarathonMatchScoring ../test_files/ 7
Test code: /MockedClass.cpp ...
Input data: /ReturnComplexStrAsItisData.txt ...
Here is the output json:
{
    "results": [
        {
            "output": "This string contains a \\ character.",
            "statistics": {
                "memory": 24632.0,
                "time": 0.048
            }
        },
        {
            "output": "This string contains a \" character.",
            "statistics": {
                "memory": 24732.0,
                "time": 0.008
            }
        },
        {
            "output": "This string contains a )\" sequence.",
            "statistics": {
                "memory": 25332.0,
                "time": 0.007
            }
        },
        {
            "output": "This string contains a R\"()\" sequence.",
            "statistics": {
                "memory": 25636.0,
                "time": 0.008
            }
        }
    ],
    "statistics": {
        "memory": {
            "max": 25636.0,
            "mean": 25083.0,
            "min": 24632.0
        },
        "time": {
            "max": 0.048,
            "mean": 0.017750000000000002,
            "min": 0.007
        }
    }
}
```

### testMemoryUsage

The signature:

```
{
"input": ["int"],
"output": "int",
"className": "MockedClass",
"method": "testMemoryUsage"
}
```

The input data (Each input parameter means the number of MB memory to allocate):

```
[
{
"input": [2]
},
{
"input": [150]
},
{
"input": [10]
}
]
```

The execution output:


```
./MarathonMatchScoring ../test_files/ 8
Test code: /MockedClass.cpp ...
Input data: /MemoryUsageData.txt ...
Here is the output json:
{
    "results": [
        {
            "output": 0,
            "statistics": {
                "memory": 27104.0,
                "time": 406.698
            }
        },
        {
            "output": 0,
            "statistics": {
                "memory": 181096.0,
                "time": 518.65
            }
        },
        {
            "output": 0,
            "statistics": {
                "memory": 38020.0,
                "time": 417.636
            }
        }
    ],
    "statistics": {
        "memory": {
            "max": 181096.0,
            "mean": 82073.33333333333,
            "min": 27104.0
        },
        "time": {
            "max": 518.65,
            "mean": 447.6613333333333,
            "min": 406.698
        }
    }
}
```

The memory is estimated as follows:

* 1. The runner record the memory usage (marked as **A**) before compiling the test code. This memory usage is not related to the tested code, but to our runner.
* 2. The runner reset the peak memory usage before running the test code.
* 3. When the test code is running, there's a separate thread monitoring the memory usage continuously. And the peak memory (marked as **P**) usage is updated as well.
* 4. After the code finishes running, the runner class collect the latest peak memory usage (i.e. **P**) withing this evaluation. And use (**P-A**) as the final estimation of memory usage for this evaluation.

Note that step 2 is very important. Otherwise, the variable **P** would grow monotonously between consequent evaluations. That is, the peak memory of evaluation #k+1 would be always larger than that of evaluation #k. Thus the estimation would be incorrect.

In this test case, we see that memory usage of evaluation #3 is less than that of #2. The memory estimation seems reasonable.

### testTimeUsage

The signature:

```
{
"input": ["int"],
"output": "int",
"className": "MockedClass",
"method": "testTimeUsage"
}
```

The input data:

```
[
{
"input": [1]
},
{
"input": [958]
},
{
"input": [100]
}
]
```

The execution output:

```
./MarathonMatchScoring ../test_files/ 9
Test code: /MockedClass.cpp ...
Input data: /TimeUsageData.txt ...
Here is the output json:
{
    "results": [
        {
            "output": 1,
            "statistics": {
                "memory": 24340.0,
                "time": 1.359
            }
        },
        {
            "output": 958,
            "statistics": {
                "memory": 24412.0,
                "time": 960.773
            }
        },
        {
            "output": 100,
            "statistics": {
                "memory": 24512.0,
                "time": 101.967
            }
        }
    ],
    "statistics": {
        "memory": {
            "max": 24512.0,
            "mean": 24421.333333333332,
            "min": 24340.0
        },
        "time": {
            "max": 960.773,
            "mean": 354.6996666666667,
            "min": 1.359
        }
    }
}
```

Compilation time of test class definition is subtracted from the running time for a better estimation. 
