SAFE_PWD=$(echo "$PWD" | sed 's/ /\\ /g')

UNAME=`uname -s`
if [ x"$UNAME" = x"Darwin" ]; then
   export OS_VERSION=$(sw_vers|grep ProductVersion|awk -F" " '{print $2}')
fi
export CLING_DIR=".sources/cling-0.5/lib/cmake/cling"
export LLVM_INSTALL_PREFIX=".sources/cling-0.5"
export CLING_LIB_DIR="$SAFE_PWD/.sources/cling-0.5/lib"
export NLOHMANN_JSON_INCLUDE_DIR=".sources/include"
