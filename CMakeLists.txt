cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(parallelized_merge_sort)

add_executable(ParallelizedMergeSort ParallelizedMergeSort.cpp)

add_subdirectory(ThreadPool)
add_subdirectory(MergeSort)

target_link_libraries(ParallelizedMergeSort PRIVATE ThreadPool)
target_link_libraries(ParallelizedMergeSort PRIVATE MergeSort)