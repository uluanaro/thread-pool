#include <atomic>
#include <functional>
#include <vector>
#include <mutex>
#include <memory>
#include "ThreadPool.h"

struct Job
{
    std::function<void()> work;
    std::atomic<int> remaining{0};
    std::vector<Job *> dependents;
};

class JobSystem
{
    ThreadPool pool;
    std::vector<std::unique_ptr<Job>> jobs;
    std::atomic<int> completed{0};
    std::mutex waitMutex;
    std::condition_variable waitCv;

public:
    explicit JobSystem(size_t numThreads) : pool(numThreads) {}

    Job *createJob(std::function<void()> work)
    {
        auto job = std::make_unique<Job>();
        job->work = std::move(work);
        jobs.push_back(std::move(job));
        return jobs.back().get();
    }
    void addDependency(Job *before, Job *after)
    {
        after->remaining += 1;
        (before->dependents).push_back(after);
    }
    void schedule(Job *job)
    {
        pool.submit([this, job]
                    {
            job->work();

            for (Job* dep: job->dependents) {
                
                if (--dep->remaining == 0) {
                    schedule(dep);
                }  
            }
            bool done;
            {
                 std::lock_guard<std::mutex> lk(waitMutex);
                done = (++completed == (int)jobs.size());
            }
            if (done) waitCv.notify_one(); });
    }
    void run()
    {
        for (auto &job : jobs)
        {
            if (job->remaining == 0)
            {
                schedule(job.get());
            }
        }
    }
    void wait()
    {
        std::unique_lock<std::mutex> lock(waitMutex);
        waitCv.wait(lock, [this]
                    { return completed == (int)jobs.size(); });
    }
};