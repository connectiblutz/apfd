#include "messagethread.h"
#include "logutil.h"

namespace apfd::common {

const uint16_t MessageThread::MSG_STOP = 0xFFFF;

MessageThread::MessageThread() {
  t=std::thread(&MessageThread::messageLoop,this);
}

MessageThread::~MessageThread() {
  stop();
  join();
}

void MessageThread::post(Message message) {
  std::unique_lock<std::mutex> lk(messageQueueMutex);
  messageQueue.push(StoredMessage(message));
  messageQueueConditionVariable.notify_one();
}
void MessageThread::postDelayed(Message message, std::chrono::milliseconds delay) {
  auto now = std::chrono::steady_clock::now();
  std::unique_lock<std::mutex> lk(messageQueueMutex);
  messageQueue.push(StoredMessage(message, now+delay));
  messageQueueConditionVariable.notify_one();
}

void MessageThread::stop() {
  post(Message(MSG_STOP));
}

void MessageThread::join() {
  static std::mutex joinMutex;
  std::lock_guard lock(joinMutex);
  if (t.joinable()) t.join();
}

void MessageThread::messageLoop() {
  std::chrono::steady_clock::time_point until(std::chrono::steady_clock::time_point::max());
  while (true) {
    std::unique_lock<std::mutex> lk(messageQueueMutex);
    if (messageQueue.empty()) {
      if (until!= std::chrono::steady_clock::time_point::max()) {
        messageQueueConditionVariable.wait_until(lk,until);
      } else {
        messageQueueConditionVariable.wait(lk);
      }
    }
    while (!messageQueue.empty()) {
      StoredMessage storedMessage = messageQueue.top();
      if (!storedMessage.isImmediate) {
        auto now = std::chrono::steady_clock::now();
        if (now < storedMessage.deliveryTime) {
          until=storedMessage.deliveryTime;
          break;
        }
      }
      until=std::chrono::steady_clock::time_point::max();
      messageQueue.pop();
      if (storedMessage.message.code()==MessageThread::MSG_STOP) return;
      OnMessage(storedMessage.message);
    }
  }
}

}
