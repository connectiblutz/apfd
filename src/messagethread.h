#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class MessageThread {
  public:
    class Message {
      public:
        Message(uint16_t code, std::shared_ptr<void> data = nullptr) : _code(code), _data(data) { }
        uint16_t code() { return _code; }
        template <typename T> T data() { return std::static_pointer_cast<T>(_data); }
      private:
        uint16_t _code;
        std::shared_ptr<void> _data;
    };
  public:
    MessageThread();
    ~MessageThread();
    void post(Message message);
    void stop();
    void join();
    void OnMessage(Message message);
  private:
    std::thread t;
    void messageLoop();
    static const uint16_t MSG_STOP;
    std::mutex messageQueueMutex;
    std::condition_variable messageQueueConditionVariable;
    std::queue<Message> messageQueue;
};
