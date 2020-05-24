#include "apfdworker.h"
#include <iostream>

namespace apfd {

const uint16_t ApfdWorker::MSG_READCONFIG = 1; 

ApfdWorker::ApfdWorker() {
  post(common::MessageThread::Message(MSG_READCONFIG));
}

void ApfdWorker::OnMessage(common::MessageThread::Message message) {
    std::cout << "received message "<<message.code() << std::endl;
}

}