#pragma once

#include "common/messagethread.h"
#include "common/threadpool.h"
#include <list>

namespace common {

class MessageThreadPool : public MessageThread {
  public:
    MessageThreadPool(uint16_t threads = 0);
    virtual ~MessageThreadPool();
    virtual void stop();
    virtual void join();
  private:
    virtual void handleMessage(std::unique_lock<std::mutex>& lk, StoredMessage& storedMessage);
    ThreadPool threadPool;
};

}
