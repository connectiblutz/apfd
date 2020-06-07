#pragma once

#include <string>
#include <functional>
#include <windows.h>
#include <memory>

namespace apfd::common {

class ServiceControl {
  public:
    ServiceControl(const std::wstring& name);
    void setDescription(const std::wstring& description) { _description = description; }
    bool install(const std::wstring& user=L"", const std::wstring& password=L"");
    bool uninstall();
    bool start();
    bool stop();
  private:
    std::wstring _name;
    std::wstring _description;
    bool getManager(std::function<bool(SC_HANDLE)> cb);
    bool getService(SC_HANDLE manager,std::function<bool(SC_HANDLE)> cb);
};

}