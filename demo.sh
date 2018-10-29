#!/bin/bash
. ./env.sh

if [[ $(uname -s) == 'Darwin' ]]; then
    DYLD_INSERT_LIBRARIES=libcling.dylib node demo.js $1
else
    LD_PRELOAD=$LD_PRELOAD:libcling.so node demo.js $1
fi
