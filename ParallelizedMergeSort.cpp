#include <algorithm>
#include <queue>
#include <thread>
#include <execution>
#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <ThreadPool.h>
#include <MergeSort.h>

using namespace std;
using namespace std::chrono;

namespace
{
    // Generate a random vector of integers
    vector<int> generate_random_vector(size_t n, int seed = 42)
    {
        mt19937 rng(seed);
        uniform_int_distribution<int> dist(0, 1'000'000);
        vector<int> v(n);
        for (size_t i = 0; i < n; ++i)
            v[i] = dist(rng);
        return v;
    }

    // Benchmark function: returns milliseconds
    template <typename Func>
    double benchmark(Func f)
    {
        auto start = high_resolution_clock::now();
        f();
        auto end = high_resolution_clock::now();
        return duration<double, milli>(end - start).count();
    }
}
int main()
{
    ThreadPool threadPool;
    vector<size_t> sizes = {100000, 500000, 1000000, 5000000, 10000000};

    cout << "Checking C++ version: " << __cplusplus << "\n";
    cout << "Serial vs Parallel std::sort benchmark (milliseconds)\n\n";
    cout << "number of hardware threads: " << thread::hardware_concurrency() << "\n";

    for (auto n : sizes)
    {
        cout << "Array size: " << n << "\n";

        vector<int> original = generate_random_vector(n);

        // serial sort
        vector<int> v_serial = original;
        double t_serial = benchmark([&]()
                                    { sort(v_serial.begin(), v_serial.end()); });

        // Parallel 1 sort
        vector<int> v_parallel_1 = original;
        double t_parallel_1 = benchmark([&]()
                                        { std::sort(std::execution::par,
                                                    v_parallel_1.begin(),
                                                    v_parallel_1.end()); });
        // serial merge 2 sort with thread pool
        vector<int> serial_merge = original;
        double t_serial_merge = benchmark([&]()
                                          { MergeSort sorter(serial_merge, threadPool); });
        // Parallel 2 sort with thread pool
        vector<int> v_parallel_2 = original;
        double t_parallel_2 = benchmark([&]()
                                        { MergeSort sorter(v_parallel_2, threadPool, true); });
        // Verify correctness
        if (v_serial != v_parallel_2 || v_serial != v_parallel_1 || v_serial != serial_merge)
        {
            cerr << "ERROR: Sorted arrays do not match!\n";
            return 1;
        }

        cout << "  Serial:   " << t_serial << " ms\n";
        cout << "  Parallel 1: " << t_parallel_1 << " ms\n";
        cout << "  Serial merge sort: " << t_serial_merge << " ms\n";
        cout << "  Parallel merge sort: " << t_parallel_2 << " ms\n";
        cout << "-----------------------------\n";
    }

    return 0;
}
