#include "common/messagethread.h"
#include "common/logutil.h"

namespace common {

const uint16_t MessageThread::MSG_STOP = 0xFFFF;

MessageThread::MessageThread() {
  t=std::thread(&MessageThread::messageLoop,this);
}

MessageThread::~MessageThread() {
  join();
}

void MessageThread::post(uint16_t code) {
  post(Message(code));
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

void MessageThread::clear(uint16_t code) {
  std::unique_lock<std::mutex> lk(messageQueueMutex);
  messageQueue.erase(StoredMessage(Message(code)));
}

void MessageThread::stop() {
  post(MSG_STOP);
}

void MessageThread::join() {
  stop();
  static std::mutex joinMutex;
  std::lock_guard lock(joinMutex);
  if (t.joinable()) t.join();
}

void MessageThread::messageLoop() {
  std::chrono::steady_clock::time_point until(std::chrono::steady_clock::time_point::max());
  std::unique_lock<std::mutex> lk(messageQueueMutex);
  while (true) {
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
      if (storedMessage.message.code()==MessageThread::MSG_STOP) return;
      handleMessage(lk,storedMessage);
    }
  }
}
void MessageThread::handleMessage(std::unique_lock<std::mutex>& lk, StoredMessage& storedMessage) {
  // unlock so OnMessage can post new messages
  lk.unlock();
  OnMessage(storedMessage.message);
  lk.lock();
}

}
