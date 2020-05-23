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
  UNUSED(message);
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

}
