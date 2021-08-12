#!/bin/bash

rm -rf build
rm -rf .vscode

if [ $CLEAN_SUBMODULE ]; then
    rm -rf thirdparty/glfw
fi
