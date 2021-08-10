#!/bin/bash

# python setup.py clean --all
rm -rf .eggs
rm -rf build
rm -rf dist
rm -rf tmp
rm -rf ptv.egg-info

rm -rf .vscode

if [ $CLEAN_SUBMODULE ]; then
    rm -rf thirdparty/glfw
fi
