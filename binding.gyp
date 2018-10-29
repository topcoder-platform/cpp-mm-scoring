{  "conditions": [
["OS!=\"win\"", {
    "targets": [
        {
          "target_name": "MarathonMatchScoring",
          "sources": [ "src/addon.cpp", "src/Runner.h", "src/Runner.cpp", "src/MemoryMonitor.h", "src/MemoryMonitor.cpp"],
          "cflags" :  ["-fexceptions", "-frtti","-std=c++11","-D_GLIBCXX_USE_NANOSLEEP", "-DLLVMDIR=\"<!(echo $LLVM_INSTALL_PREFIX)\""],
          "cflags!": [ "-fno-exceptions","-fno-rtti"],
          "cflags_cc!": [ "-fno-exceptions","-fno-rtti"],
          "conditions": [
                  ["OS==\"mac\"", {
                    "xcode_settings": {
                     "GCC_ENABLE_CPP_RTTI": "YES",
                     "GCC_DYNAMIC_NO_PIC": 'YES',
                      "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                      "OTHER_CPLUSPLUSFLAGS" : ["-std=c++11", "-stdlib=libc++", "-D_GLIBCXX_USE_NANOSLEEP", "-DLLVMDIR=\"<!(echo $LLVM_INSTALL_PREFIX)\"","-I<!(echo $LLVM_INSTALL_PREFIX)/include"],
                    }
                  }]
                ],
          "include_dirs": [
            "<!(node -e \"require('nan')\")",
            "<!(echo $NLOHMANN_JSON_INCLUDE_DIR)",
          ],
          "link_settings": {
            "libraries": [
              "-L<!(echo $CLING_LIB_DIR)",
              "-lclingInterpreter"
            ]
          }
        }
    ]
    }]]
}
