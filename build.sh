#!/usr/bin/bash

mkdir build
clear 
cd build 
cmake .. && cmake --build . && ./tests
cd ..
