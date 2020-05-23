#include "messagethread.h"

#define UNUSED(x) (void)(x)

const uint16_t MessageThread::MSG_STOP = 0xFFFF;

MessageThread::MessageThread() {
  t=std::thread(&MessageThread::messageLoop,this);
}

MessageThread::~MessageThread() {
  stop();
  join();
}

void MessageThread::post(uint16_t message) {
  std::unique_lock<std::mutex> lk(messageQueueMutex);
  messageQueue.push(message);
  messageQueueConditionVariable.notify_one();
}

void MessageThread::stop() {
  post(MSG_STOP);
}

void MessageThread::join() {
  t.join();
}

void MessageThread::OnMessage(uint16_t message) {
  UNUSED(message);
}

void MessageThread::messageLoop() {
  while (true) {
    std::unique_lock<std::mutex> lk(messageQueueMutex);
    messageQueueConditionVariable.wait(lk,[this]{ return messageQueue.size()>0; });
    while (messageQueue.size()>0) {
      uint16_t message = messageQueue.front();
      messageQueue.pop();
      if (message==MessageThread::MSG_STOP) return;
      OnMessage(message);
    }
  }
}
