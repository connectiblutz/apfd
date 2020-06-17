#include "common/messagethreadpool.h"
#include "common/logutil.h"

namespace common {

MessageThreadPool::MessageThreadPool(uint16_t threads) {
  for (auto i = 0; i < threads; i++) {
    poolThreads.push_back(std::thread(&MessageThreadPool::poolLoop,this));
  }
}

MessageThreadPool::~MessageThreadPool() {
  stop();
  join();
}


void MessageThreadPool::join() {
  static std::mutex joinMutex;
  std::lock_guard lock(joinMutex);
  for (auto&& pt : poolThreads) {
    if (pt.joinable()) pt.join();
  }
  MessageThread::join();
}

void MessageThreadPool::messageLoop() {
  std::chrono::steady_clock::time_point until(std::chrono::steady_clock::time_point::max());
  while (true) {
    std::unique_lock<std::mutex> lk(messageQueueMutex);
    // wait if there are no messages or we have an until time
    if (until!= std::chrono::steady_clock::time_point::max()) {
      messageQueueConditionVariable.wait_until(lk,until);
    } else if (messageQueue.empty()) {
      messageQueueConditionVariable.wait(lk);
    }
    // process messages until the next is a delayed
    while (!messageQueue.empty()) {
      auto storedMessage = messageQueue.top();
      if (!storedMessage.isImmediate) {
        auto now = std::chrono::steady_clock::now();
        if (now < storedMessage.deliveryTime) {
          until=storedMessage.deliveryTime;
          break;
        }
      }
      messageQueue.pop();
      until=std::chrono::steady_clock::time_point::max();
      std::unique_lock<std::mutex> lk(poolMutex);
      if (storedMessage.message.code()==MessageThread::MSG_STOP) {
        for (unsigned i = 0; i < poolThreads.size(); i++) {
          poolQueue.push(storedMessage);
        }
        poolConditionVariable.notify_all();
        return;
      } else {
        poolQueue.push(storedMessage);
        poolConditionVariable.notify_one();
      }

    }
  }
}
void MessageThreadPool::poolLoop() {
  while (true) {
    std::unique_lock<std::mutex> lk(poolMutex);
    poolConditionVariable.wait(lk);
    
    // process messages until the next is a delayed
    while (!poolQueue.empty()) {
      auto storedMessage = poolQueue.front();
      poolQueue.pop();
      if (storedMessage.message.code()==MessageThread::MSG_STOP) return;
      // unlock so OnMessage can post new messages
      lk.unlock();
      OnMessage(storedMessage.message);
      lk.lock();
    }
  }
}

}
