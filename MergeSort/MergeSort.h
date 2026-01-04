#pragma once

#include <iostream>
#include <vector>
#include "ThreadPool.h"
using namespace std;

class MergeSort
{
public:
    void merge(vector<int> &res, int &left, int &right, vector<int> &previousA);
    MergeSort(vector<int> &a, ThreadPool &threadPool, bool parallelize = false);
    ~MergeSort();
};