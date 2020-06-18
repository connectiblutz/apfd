#include "common/threadpool.h"
#include "common/logutil.h"

namespace common {

ThreadPool::ThreadPool(uint16_t threads) : running(true), _stopWhenEmpty(false) {
  if (threads==0) {
    threads = std::thread::hardware_concurrency();
  }
  LogUtil::Debug()<<"using thread pool size "<<threads;
  for (auto i = 0; i < threads; i++) {
    poolThreads.push_back(std::thread(&ThreadPool::poolLoop,this));
  }
}

ThreadPool::~ThreadPool() {
  stop();
  join();
}

void ThreadPool::post(std::shared_ptr<Runnable> runnable) {
  std::unique_lock<std::mutex> lk(poolMutex);
  poolQueue.push(runnable);
  poolConditionVariable.notify_one();
}

void ThreadPool::stop() {
  std::unique_lock<std::mutex> lk(poolMutex);
  running=false;
  poolConditionVariable.notify_all();
}

void ThreadPool::stopWhenEmpty() {
  std::unique_lock<std::mutex> lk(poolMutex);
  _stopWhenEmpty=true;
  poolConditionVariable.notify_all();
}

void ThreadPool::join() {
  static std::mutex joinMutex;
  std::lock_guard lock(joinMutex);
  for (auto&& pt : poolThreads) {
    if (pt.joinable()) pt.join();
  }
}

void ThreadPool::poolLoop() {
  std::unique_lock<std::mutex> lk(poolMutex);
  while (running) {
    poolConditionVariable.wait(lk);
    // process messages until the next is a delayed
    while (!poolQueue.empty()) {
      auto runnable = poolQueue.front();
      poolQueue.pop();
      lk.unlock();
      runnable->run();
      lk.lock();
    }
    if (_stopWhenEmpty && poolQueue.empty()) {
      break;
    }
  }
}

}
