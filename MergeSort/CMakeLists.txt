add_library(MergeSort MergeSort.cpp)

target_include_directories(MergeSort PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(MergeSort PRIVATE ThreadPool)