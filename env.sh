if [[ $(uname -s) == 'Darwin' ]]; then
    export CLING_DIR=/usr/local/cling/0.5_1/lib/cmake/cling
    export LLVM_INSTALL_PREFIX=/usr/local/cling/0.5_1
    export CLING_LIB_DIR=$LLVM_INSTALL_PREFIX/lib
    export NLOHMANN_JSON_INCLUDE_DIR=/usr/local/include
else
    export CLING_DIR=/usr/local/cling/0.5_1/lib/cmake/cling
    export LLVM_INSTALL_PREFIX=/usr/local/cling/0.5_1
    export CLING_LIB_DIR=$LLVM_INSTALL_PREFIX/lib
    export NLOHMANN_JSON_INCLUDE_DIR=/usr/local/include
fi
