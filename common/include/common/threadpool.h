#pragma once

#include <queue>
#include <mutex>
#include <list>
#include <functional>
#include <thread>
#include <memory>

namespace common {

class ThreadPool {
  public:
    class Runnable {
      public:
        Runnable() {};
        Runnable(std::function<void(void)> fn) : _fn(fn) {};
        virtual ~Runnable() {};
        virtual void run() { if (_fn) _fn(); };
      private:
        std::function<void(void)> _fn;
    };
  public:
    ThreadPool(uint16_t threads = 0);
    virtual ~ThreadPool();
    virtual void post(std::shared_ptr<Runnable> runnable);
    virtual void join();
    virtual void stop();
  private:
    bool running;
    void poolLoop();
    std::queue<std::shared_ptr<Runnable>> poolQueue;
    std::list<std::thread> poolThreads;
    std::mutex poolMutex;
    std::condition_variable poolConditionVariable;
};

}
