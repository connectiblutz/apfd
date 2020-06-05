#pragma once

#include "messagethread.h"

#include <string>
#include <memory>
#include "singleton.h"

namespace apfd::common {

class WindowsService {
  public:
    static bool Install(std::wstring name, std::wstring description, std::wstring user=L"", std::wstring password=L"");
    static bool Uninstall(std::wstring name);
  public:
    void start();
    auto name() { return _name; }
    auto thread() { return _thread; }
  protected:
    WindowsService(std::wstring name, std::shared_ptr<MessageThread> thread);
    friend Singleton::Weak;
  private:
    std::wstring _name;
    std::shared_ptr<MessageThread> _thread;
};

}
