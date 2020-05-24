#pragma once

#include "messagethread.h"

namespace apfd {

class ApfdWorker : public common::MessageThread {
    public:
        void OnMessage(common::MessageThread::Message message);
};

}