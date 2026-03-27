#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool {
    public:
        ThreadPool(int numThreads);
        void addTask(std::function<void()> task);
        ~ThreadPool();
    private: 
        std::vector<std::thread> workers; // holds the worker threads
        std::queue<std::function<void()>> tasks; // holds the tasks
        std::mutex mx;// protects the queue
        std::condition_variable kickStarter;// puts threads to sleep / wakes them
        bool stop = false; // signals shutdown
};


#endif