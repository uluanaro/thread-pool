#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <functional>

class ThreadPool
{
public:
    using Task = std::function<void()>;

private:
    std::vector<std::thread> workers;
    std::queue<Task> tasks;
    std::mutex m;
    std::condition_variable cv;
    bool stop = false;

    void workerLoop()
    {
        while (true)
        {
            Task task;

            {
                std::unique_lock<std::mutex> lock(m);
                cv.wait(lock, [this] { return stop || !tasks.empty(); });

                if (stop && tasks.empty())
                {
                    return;
                }

                task = std::move(tasks.front());
                tasks.pop();
            }

            task();
        }
    }

public:
    explicit ThreadPool(size_t numThreads)
    {
        workers.reserve(numThreads);
        for (size_t i = 0; i < numThreads; ++i)
        {
            workers.emplace_back(&ThreadPool::workerLoop, this);
        }
    }

    ~ThreadPool()
    {
        {
            std::lock_guard<std::mutex> lock(m);
            stop = true;
        }

        cv.notify_all();

        for (auto& worker : workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }

    void submit(Task task)
    {
        {
            std::lock_guard<std::mutex> lock(m);
            if (stop)
            {
                throw std::runtime_error("submit on stopped ThreadPool");
            }
            tasks.push(std::move(task));
        }

        cv.notify_one();
    }
};