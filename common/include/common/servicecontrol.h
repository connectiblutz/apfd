#pragma once

#include <string>
#include <functional>
#include <memory>
#ifdef _WIN32
#include <windows.h>
#endif

namespace common {

class ServiceControl {
  public:
    ServiceControl(const std::string& name);
    void setDescription(const std::string& description) { _description = description; }
    bool install(const std::string& user="", const std::string& password="");
    bool uninstall();
    bool start();
    bool stop();
  private:
    std::string _name;
    std::string _description;
#ifdef _WIN32
    bool getManager(std::function<bool(SC_HANDLE)> cb);
    bool getService(SC_HANDLE manager,std::function<bool(SC_HANDLE)> cb);
#endif
};

}