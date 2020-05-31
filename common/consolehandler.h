#pragma once

#include "messagethread.h"
#include "singleton.h"

namespace apfd::common {

class ConsoleHandler {
  public:
    auto thread() { return _thread; }
  protected:
    ConsoleHandler(std::shared_ptr<MessageThread> thread);
    ~ConsoleHandler();
    friend void Singleton::D<ConsoleHandler>::operator()(ConsoleHandler* p) const;
    friend Singleton;
  private:
    std::shared_ptr<MessageThread> _thread;
};

}
