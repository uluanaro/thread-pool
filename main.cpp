
#include <iostream>
#include <mutex>
#include "JobSystem.h"

int main()
{
    JobSystem js(4);
    Job *a = js.createJob([]()
                          { std::cout << "Job A executed\n"; });
    Job *b = js.createJob([]()
                          { std::cout << "Job B executed\n"; });
    Job *c = js.createJob([]()
                          { std::cout << "Job C executed\n"; });
    Job *d = js.createJob([]()
                          { std::cout << "Job D executed\n"; });
    js.addDependency(a, c);
    js.addDependency(b, c);
    js.addDependency(c, d);
    js.run();
    js.wait();
    std::cout << "All tasks are completed!";
}