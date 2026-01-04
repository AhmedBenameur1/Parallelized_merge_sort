#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <functional>
#include <atomic>
#include <cassert>

using namespace std;

class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();
    void processThread(int i);
    bool isThreadAvailable(int i) const { return _tasks[i].empty(); }
    size_t getTotalNumberOfThreads() const { return _totalNumberOfThreads; }
    void addTask(function<void()> task, int i);
    void waitForPendingTasks();
    unsigned getNumberOfRunningThreads() const { return _numberOfPendingTasks; }
    void increaseNumberOfPendingTasks();
    void decreaseNumberOfPendingTasks();

private:
    vector<thread *> _threads;
    vector<unique_ptr<mutex>> _taskMutexes;
    vector<unique_ptr<condition_variable>> _newTask;
    vector<queue<function<void()>>> _tasks;
    mutex _numberOfPendingTasksMutex;
    condition_variable _waitForRunningThreads;
    unsigned _numberOfPendingTasks;
    unsigned _totalNumberOfThreads;
    atomic<bool> _endAllJobs;
};