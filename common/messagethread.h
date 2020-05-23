#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <vector>

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
    void postDelayed(Message message, std::chrono::milliseconds delay);
    void stop();
    void join();
    void OnMessage(Message message);
  private:
    class StoredMessage {
      public:
        StoredMessage(Message message) : message(message), isImmediate(true) { }
        StoredMessage(Message message, std::chrono::steady_clock::time_point deliveryTime) : message(message), isImmediate(false), deliveryTime(deliveryTime) { }
        Message message;
        bool isImmediate;
        std::chrono::steady_clock::time_point deliveryTime;
    };
    class StoredMessageCompare {
      public:
        bool operator() (StoredMessage a, StoredMessage b)
        {
          if (a.isImmediate&&!b.isImmediate) return true;
          if (!a.isImmediate&&b.isImmediate) return false;
          if (a.isImmediate&&b.isImmediate) return false;
          return a.deliveryTime < b.deliveryTime;
        }
    };
  private:
    std::thread t;
    void messageLoop();
    static const uint16_t MSG_STOP;
    std::mutex messageQueueMutex;
    std::condition_variable messageQueueConditionVariable;
    std::priority_queue<StoredMessage, std::vector<StoredMessage>, StoredMessageCompare> messageQueue;
};
