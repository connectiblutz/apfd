#pragma once

#include "messagethread.h"

#include <string>
#include <memory>
#include "singleton.h"

namespace apfd::common {

class WindowsService {
  public:
    static bool Install(std::string name, std::string description);
    static bool Uninstall(std::string name);
    static void ChdirToBin();
  public:
    void start();
    auto name() { return _name; }
    auto thread() { return _thread; }
  protected:
    WindowsService(std::string name, std::shared_ptr<MessageThread> thread);
    friend Singleton::Weak;
  private:
    std::string _name;
    std::shared_ptr<MessageThread> _thread;
};

}
