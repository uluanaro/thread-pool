#include "ThreadPool.h"
#include <iostream>
#include <mutex>

int main()
{
    std::mutex m;
    {
        ThreadPool pool(4);
        for (int i = 0; i <= 6; ++i)
        {
            pool.submit([i, &m]
                        { 
                            std::lock_guard<std::mutex> lock(m);
                            std::cout << "The task " << i << " has been done\n"; });
        }
    }
    std::cout << "All tasks completed!\n";
}