#include "threadPool.hpp"

ThreadPool::ThreadPool(int numThreads) {
    for (int i = 0; i < numThreads; i++) {
        workers.emplace_back(
            [this]() {
                // lambda function for each thread
                while (1) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mx);
                        kickStarter.wait(lock, [this] {
                            return !tasks.empty() || stop;
                        });

                        if (stop && tasks.empty())
                            return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            }
        );
    }
};

void ThreadPool::addTask(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(mx);
        tasks.push(task);
    }

    kickStarter.notify_one();
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mx);
        stop = true;
    }
    kickStarter.notify_all();

    for (auto& worker : workers)
        worker.join();
}