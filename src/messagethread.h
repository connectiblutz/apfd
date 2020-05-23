#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class MessageThread {
  public:
    MessageThread();
    ~MessageThread();
    void post(uint16_t message);
    void stop();
    void join();
    void OnMessage(uint16_t message);
  private:
    std::thread t;
    void messageLoop();
    static const uint16_t MSG_STOP;
    std::mutex messageQueueMutex;
    std::condition_variable messageQueueConditionVariable;
    std::queue<uint16_t> messageQueue;
};
