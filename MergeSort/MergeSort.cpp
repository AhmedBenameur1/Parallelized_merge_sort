#include <algorithm>
#include <queue>
#include <thread>
#include <vector>
#include <tuple>
#include "MergeSort.h"

using namespace std;

void MergeSort::merge(vector<int> &res, vector<int> &x, vector<int> &y)
{
    int nn = x.size();
    int m = y.size();
    int indx = 0, indy = 0;
    for (int i = 0; i < nn + m; i++)
    {
        if (indx < nn && indy < m)
        {
            if (x[indx] < y[indy])
            {
                res[i] = x[indx++];
            }
            else
            {
                res[i] = y[indy++];
            }
        }
        else
        {
            if (indx < nn)
            {
                res[i] = x[indx++];
            }
            else
            {
                res[i] = y[indy++];
            }
        }
    }
}
MergeSort::MergeSort(vector<int> &a, ThreadPool &threadPool)
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
    while (i < m)
    {
        int j = i + 1;
        while (j < m && get<2>(v[j]) == get<2>(v[i]))
            j++;
        auto chunk = [&](int l, int r)
        {
            for (int k = l; k <= r; k++)
            {
                auto [left, right, level] = v[k];
                if (left == right)
                    continue;
                int mid = (left + right) / 2;
                vector<int> vl(mid - left + 1), vr(right - mid);
                for (int k = left; k <= mid; k++)
                {
                    vl[k - left] = a[k];
                }
                for (int k = mid + 1; k <= right; k++)
                {
                    vr[k - mid - 1] = a[k];
                }
                vector<int> result(right - left + 1);
                merge(result, vl, vr);
                for (int k = left; k <= right; k++)
                {
                    a[k] = result[k - left];
                }
            }
        };
        // thread pool
        int numberOfThreads = threadPool.getTotalNumberOfThreads();
        int task_id = 0;
        int chunk_size = max(1, (j - i) / numberOfThreads);
        for (int k = i; k < j; k += chunk_size)
        {
            int starting = k, ending = min(k + chunk_size - 1, j - 1);
            threadPool.addTask([chunk, starting, ending]()
                               { chunk(starting, ending); }, task_id++ % numberOfThreads);
            // chunk(starting, ending);
        }
        threadPool.waitForPendingTasks();
        // vector<thread> threads;
        // for (int k = i; k < j; k += chunk_size)
        // {
        //     int starting = k, ending = min(k + chunk_size - 1, j - 1);
        //     function<void()> task = [chunk, starting, ending]()
        //     { chunk(starting, ending); };
        //     threads.push_back(thread(task));
        // }
        // for (int k = 0; k < threads.size(); k++)
        //     threads[k].join();
        i = j;
    }
}
MergeSort::~MergeSort() {}