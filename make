#! /usr/bin/bash

if [ ! -d "./build" ]; then
    mkdir build
fi

if [ ! -d "./output" ]; then
    mkdir output
fi

name=process

clang -I Include $name.c -o ./build/$name
./build/$name