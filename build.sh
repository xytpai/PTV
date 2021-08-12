#!/bin/bash

if [ -f ./thirdparty/glfw/build/src/libglfw3.a ]; then
    echo "./thirdparty/glfw/build/src/libglfw3.a found"
else
    if [ ! -d ./thirdparty/glfw ]; then
        cd thirdparty
        # git clone https://github.com/glfw/glfw
        git clone https://github.com.cnpmjs.org/glfw/glfw
        cd ..
    fi
    cd ./thirdparty/glfw
    mkdir build; cd build
    cmake ..; make
    cd ../../../
fi

mkdir build; cd build
cmake ..; make -j4
cd ..; ./build/world