#!/bin/bash
. ./env.sh

if [[ $(uname -s) == 'Darwin' ]]; then
    DYLD_INSERT_LIBRARIES=.sources/cling-0.5/lib/libcling.dylib node demo.js $1
else
    LD_PRELOAD=$LD_PRELOAD:.sources/cling-0.5/lib/libcling.so node demo.js $1
fi
