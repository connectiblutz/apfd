#pragma once

#include "messagethread.h"

#include <string>
#include <memory>

namespace apfd::common {

class WindowsService {
  public:
    template<class... Args>
    static std::shared_ptr<WindowsService> make_shared(Args&&... args);
  public:
    auto name() { return _name; }
    auto thread() { return _thread; }
  protected:
    WindowsService(std::string name, std::shared_ptr<MessageThread> thread);
    class D { public: void operator()(WindowsService* p) const { delete p; } };
    friend void D::operator()(WindowsService* p) const;
  private:
    std::string _name;
    std::shared_ptr<MessageThread> _thread;
};

}
