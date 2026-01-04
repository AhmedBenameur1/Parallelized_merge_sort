#!/bin/bash
mkdir -p build
cd build
cmake ..
cmake --build .
Debug\ParallelizedMergeSort.exe