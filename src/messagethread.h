#pragma once

#include <thread>

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
};
