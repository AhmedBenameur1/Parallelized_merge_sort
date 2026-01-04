setlocal enabledelayedexpansion
if not exist build mkdir build
cd build
cmake ..
cmake --build .
Debug\ParallelizedMergeSort.exe
pause
