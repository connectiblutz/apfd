#include "apfdworker.h"
#include <iostream>

namespace apfd {


void ApfdWorker::OnMessage(common::MessageThread::Message message) {
    std::cout << "received message "<<message.code() << std::endl;
}

}