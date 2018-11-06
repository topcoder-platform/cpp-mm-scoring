SAFE_PWD=$(echo "$PWD" | sed 's/ /\\ /g')

export OS_VERSION=$(sw_vers|grep ProductVersion|awk -F" " '{print $2}')
export CLING_DIR=".sources/cling-0.5/lib/cmake/cling"
export LLVM_INSTALL_PREFIX=".sources/cling-0.5"
export CLING_LIB_DIR="$SAFE_PWD/.sources/cling-0.5/lib"
export NLOHMANN_JSON_INCLUDE_DIR=".sources/include"
