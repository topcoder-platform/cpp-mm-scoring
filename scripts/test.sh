#!/bin/bash
. ./env.sh

if [[ $(uname -s) == 'Darwin' ]]; then
    DYLD_INSERT_LIBRARIES=.sources/cling-0.5/lib/libcling.dylib node node_modules/.bin/mocha
else
    LD_PRELOAD=$LD_PRELOAD:.sources/cling-0.5/lib/libcling.so node node_modules/.bin/mocha
fi
