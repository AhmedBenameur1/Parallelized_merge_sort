
#include "ThreadPool.h"

using namespace std;

ThreadPool::ThreadPool() : _numberOfPendingTasks(0), _totalNumberOfThreads(thread::hardware_concurrency()), _endAllJobs(false)
{
    _tasks.resize(_totalNumberOfThreads);
    _taskMutexes.resize(_totalNumberOfThreads);
    _newTask.resize(_totalNumberOfThreads);
    for (unsigned i = 0; i < _totalNumberOfThreads; i++)
    {
        _taskMutexes[i] = make_unique<mutex>();
        _newTask[i] = make_unique<condition_variable>();
        _threads.push_back(new thread([this, i]()
                                      { processThread(i); }));
    }
}
ThreadPool::~ThreadPool()
{
    _endAllJobs.store(true);
    for (unsigned i = 0; i < _totalNumberOfThreads; i++)
    {
        _newTask[i]->notify_one();
        _threads[i]->join();
        delete _threads[i];
    }
}

void ThreadPool::increaseNumberOfPendingTasks()
{
    lock_guard<mutex> guardNumberOfRunningThreads(_numberOfPendingTasksMutex);
    _numberOfPendingTasks++;
}

void ThreadPool::decreaseNumberOfPendingTasks()
{
    lock_guard<mutex> guardNumberOfRunningThreads(_numberOfPendingTasksMutex);
    _numberOfPendingTasks--;
    assert(_numberOfPendingTasks >= 0);
    // notify main thread in case it is waiting for all running threads to finish
    if (_numberOfPendingTasks == 0)
        _waitForRunningThreads.notify_one();
}

void ThreadPool::processThread(int i)
{
    unique_lock<mutex> guard(*_taskMutexes[i]);
    while (!_endAllJobs.load())
    {
        if (_tasks[i].empty() && !_endAllJobs.load())
        {
            _newTask[i]->wait(guard, [this, i]()
                              { return !_tasks[i].empty() || _endAllJobs.load(); });
        }
        if (_endAllJobs.load())
        {
            while (!_tasks[i].empty())
                _tasks[i].pop(), decreaseNumberOfPendingTasks();
            break;
        }
        function<void()> task = _tasks[i].front();
        _tasks[i].pop();
        guard.unlock();
        task();
        guard.lock();
        decreaseNumberOfPendingTasks();
    }
}
void ThreadPool::addTask(function<void()> task, int i)
{
    unique_lock<mutex> guard(*_taskMutexes[i]);
    _tasks[i].push(task);
    increaseNumberOfPendingTasks();
    _newTask[i]->notify_one();
}
void ThreadPool::waitForPendingTasks()
{
    unique_lock<mutex> guardNumberOfRunningThreads(_numberOfPendingTasksMutex);
    if (_numberOfPendingTasks > 0)
    {
        _waitForRunningThreads.wait(guardNumberOfRunningThreads, [this]()
                                    { return _numberOfPendingTasks == 0; });
    }
}
