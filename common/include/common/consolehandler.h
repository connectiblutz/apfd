#pragma once

#include "common/messagethread.h"
#include "common/singleton.h"

namespace common {

class ConsoleHandler {
  public:
    auto thread() { return _thread; }
  protected:
    ConsoleHandler(std::shared_ptr<MessageThread> thread);
    ~ConsoleHandler();
    friend Singleton::Weak;
  private:
    std::shared_ptr<MessageThread> _thread;
};

}
