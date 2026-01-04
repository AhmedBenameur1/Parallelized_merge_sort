#pragma once

#include <iostream>
#include <vector>
#include "ThreadPool.h"
using namespace std;

class MergeSort
{
public:
    void merge(vector<int> &result, vector<int> &left, vector<int> &right);
    MergeSort(vector<int> &a, ThreadPool &threadPool);
    ~MergeSort();
};