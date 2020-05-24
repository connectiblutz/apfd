#pragma once

#include "messagethread.h"

namespace apfd::common {

class ConsoleHandler {
  public:
    template<class... Args>
    static std::shared_ptr<ConsoleHandler> make_shared(Args&&... args);
  public:
    auto thread() { return _thread; }
  protected:
    ConsoleHandler(std::shared_ptr<MessageThread> thread);
    ~ConsoleHandler();
    class D { public: void operator()(ConsoleHandler* p) const { delete p; } };
    friend void D::operator()(ConsoleHandler* p) const;
  private:
    std::shared_ptr<MessageThread> _thread;
};

}
