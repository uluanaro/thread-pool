# ThreadPool

A fixed-size thread pool in modern C++ (C++17): worker threads that pull tasks
from a shared, thread-safe queue and run them concurrently.

## Why
Spawning a thread per task is expensive — creation/teardown and excessive
context switching ("thread thrashing") eat the gains. A pool creates a fixed
number of workers once and reuses them for many tasks.

## What it demonstrates
- `std::thread` lifecycle, joined on shutdown
- Thread-safe task queue with `std::mutex` + `std::condition_variable`
- Producer/consumer pattern (submit → queue → workers)
- Correct `condition_variable` usage: a predicate guards against spurious
  and lost wakeups
- Minimal critical section: tasks run *outside* the lock, so workers execute
  in real parallel
- Graceful shutdown that drains remaining tasks before stopping (RAII dtor)

## API
    ThreadPool pool(4);              // 4 worker threads
    pool.submit([]{ /* work */ });   // enqueue a task
    // destructor joins all workers after queued tasks finish

## Build & run
    g++ -std=c++17 -pthread -o tp main.cpp
    ./tp

## Possible extensions
- `submit` returning a `std::future` for task results
- A Job System on top: tasks with dependencies
- Per-worker work-stealing queues