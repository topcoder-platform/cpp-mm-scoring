#!/bin/bash
. ./env.sh

if [[ $(uname -s) == 'Darwin' ]]; then
    DYLD_INSERT_LIBRARIES=libcling.dylib node node_modules/.bin/mocha
else
    LD_PRELOAD=$LD_PRELOAD:libcling.so node node_modules/.bin/mocha
fi
