#include "common/messagethreadpool.h"
#include "common/logutil.h"

#define UNUSED(x) (void)(x)

namespace common {

MessageThreadPool::MessageThreadPool(uint16_t threads) : threadPool(threads) {
}

MessageThreadPool::~MessageThreadPool() {
  stop();
  join();
}

void MessageThreadPool::stop() {
  threadPool.stop();
  MessageThread::stop();
}

void MessageThreadPool::stopWhenEmpty() {
  threadPool.stopWhenEmpty();
  MessageThread::stopWhenEmpty();
}

void MessageThreadPool::join() {
  threadPool.join();
  MessageThread::join();
}

void MessageThreadPool::handleMessage(std::unique_lock<std::mutex>& lk, StoredMessage& storedMessage) {
  UNUSED(lk);
  threadPool.post(std::make_shared<ThreadPool::Runnable>([this,storedMessage](){
    OnMessage(storedMessage.message);
  }));
}

}
