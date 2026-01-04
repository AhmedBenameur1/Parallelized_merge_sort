#include <algorithm>
#include <queue>
#include <thread>
#include <vector>
#include <tuple>
#include "MergeSort.h"

using namespace std;

void MergeSort::merge(vector<int> &a, int &left, int &right, vector<int> &previousA)
{
    int mid = (left + right) / 2;
    int indx = left, indy = mid + 1;
    for (int i = left; i <= right; i++)
    {
        if (indx <= mid && indy <= right)
        {
            if (previousA[indx] < previousA[indy])
            {
                a[i] = previousA[indx++];
            }
            else
            {
                a[i] = previousA[indy++];
            }
        }
        else
        {
            if (indx <= mid)
            {
                a[i] = previousA[indx++];
            }
            else
            {
                a[i] = previousA[indy++];
            }
        }
    }
}
MergeSort::MergeSort(vector<int> &a, ThreadPool &threadPool, bool parallelize)
{
    int n = a.size();
    // range [l,r] and level in the merge sort tree
    vector<tuple<int, int, int>> v;
    // bfs
    queue<tuple<int, int, int>> q;
    q.push(make_tuple(0, n - 1, 0));
    while (!q.empty())
    {
        auto [l, r, level] = q.front();
        v.push_back(make_tuple(l, r, level));
        q.pop();
        if (l != r)
        {
            int m = (l + r) / 2;
            q.push(make_tuple(l, m, level + 1));
            q.push(make_tuple(m + 1, r, level + 1));
        }
    }
    reverse(v.begin(), v.end());
    int i = 0, m = v.size();
    vector<int> previousA(n);
    while (i < m)
    {
        int j = i + 1;
        while (j < m && get<2>(v[j]) == get<2>(v[i]))
            j++;
        auto chunk = [this, &a, &previousA, &v](int l, int r)
        {
            for (int k = l; k <= r; k++)
            {
                auto &[left, right, level] = v[k];
                if (left == right)
                    continue;
                int mid = (left + right) / 2;
                for (int k = left; k <= right; k++)
                {
                    previousA[k] = a[k];
                }
                merge(a, left, right, previousA);
            }
        };
        // thread pool
        int numberOfThreads = threadPool.getTotalNumberOfThreads();
        int task_id = 0;
        int chunk_size = max(1, (j - i) / numberOfThreads);
        for (int k = i; k < j; k += chunk_size)
        {
            int l = k, r = min(k + chunk_size - 1, j - 1);
            if (parallelize)
                threadPool.addTask([chunk, l, r]()
                                   { chunk(l, r); }, task_id++ % numberOfThreads);
            else
                chunk(l, r);
        }
        if (parallelize)
            threadPool.waitForPendingTasks();
        i = j;
    }
}
MergeSort::~MergeSort() {}