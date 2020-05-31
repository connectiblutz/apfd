#pragma once

#include "messagethread.h"

#include <string>
#include <memory>
#include "singleton.h"

namespace apfd::common {

class WindowsService {
  public:
    auto name() { return _name; }
    auto thread() { return _thread; }
  protected:
    WindowsService(std::string name, std::shared_ptr<MessageThread> thread);
    friend void Singleton::D<WindowsService>::operator()(WindowsService* p) const;
    friend Singleton;
  private:
    std::string _name;
    std::shared_ptr<MessageThread> _thread;
};

}
