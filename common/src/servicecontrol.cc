#include "common/servicecontrol.h"
#include "common/logutil.h"
#include "common/servicecontrol.h"
#include "common/stringutil.h"

#define UNUSED(x) (void)(x)

namespace common {

ServiceControl::ServiceControl(const std::string& name) : _name(name) {

}


#ifdef _WIN32
bool ServiceControl::getManager(std::function<bool(SC_HANDLE)> cb) {
  SC_HANDLE schSCManager;

  // Get a handle to the SCM database. 
  schSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
  if (NULL == schSCManager) 
  {
    LogUtil::Debug()<<"OpenSCManager failed (" << GetLastError() << ")";
    return false;
  }

  bool ret = cb(schSCManager);
  CloseServiceHandle(schSCManager);
  return ret;
}
bool ServiceControl::getService(SC_HANDLE manager,std::function<bool(SC_HANDLE)> cb) {
  SC_HANDLE schService;

  // Get a handle to the service.
  schService = OpenService(manager, StringUtil::toWide(_name).c_str(), SERVICE_ALL_ACCESS);
  if (schService == NULL)
  { 
      LogUtil::Debug()<<"OpenService failed (" << GetLastError() << ")";
      return (GetLastError()==0 || GetLastError()==ERROR_SERVICE_DOES_NOT_EXIST);
  }

  bool ret = cb(schService);
  CloseServiceHandle(schService);
  return ret;
}
#endif

bool ServiceControl::install(const std::string& user, const std::string& password) {
	uninstall();

#ifdef _WIN32
  return getManager([this,user,password](SC_HANDLE manager) {
    wchar_t szPath[MAX_PATH];

    if( !GetModuleFileName( nullptr, szPath, MAX_PATH ) ) {
      LogUtil::Debug()<<"Cannot install service (" << GetLastError() << ")";
      return false;
    }
      
    // Create the service
    SC_HANDLE schService = CreateService( 
        manager,              // SCM database 
        StringUtil::toWide(_name).c_str(),                   // name of service 
        StringUtil::toWide(_description).c_str(),                   // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_AUTO_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        szPath,                    // path to service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        user.empty()?nullptr:StringUtil::toWide(".\\"+user).c_str(),                      // LocalSystem account 
        password.empty()?nullptr:StringUtil::toWide(password).c_str());                     // no password 

    if (schService == NULL) {
      LogUtil::Debug()<<"CreateService failed (" << GetLastError() << ")";
      return false;
    }
    return true;
  });
#else
  UNUSED(user);
  UNUSED(password);
  return false;
#endif
}

bool ServiceControl::uninstall() {
#ifdef _WIN32
  return getManager([this](SC_HANDLE manager) {
    return getService(manager,[](SC_HANDLE service) {
      if (! DeleteService(service) ) 
      {
        LogUtil::Debug()<<"DeleteService failed (" << GetLastError() << ")";
        return false;
      }
      return true;
    });
  });
#else
  return false;
#endif
}
bool ServiceControl::stop() {
#ifdef _WIN32
  return getManager([this](SC_HANDLE manager) {
    return getService(manager,[](SC_HANDLE service) {
      SERVICE_STATUS ssp;    
      if (!ControlService(service, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS) &ssp ) )
      {
        LogUtil::Debug()<<"ControlService failed (" << GetLastError() << ")";
        return false;
      }
      return true;
    });
  });
#else
  return false;
#endif
}
bool ServiceControl::start() {
#ifdef _WIN32
  return getManager([this](SC_HANDLE manager) {
    return getService(manager,[](SC_HANDLE service) {
      if (!StartService(service,0,nullptr)) {
        LogUtil::Debug()<<"StartService failed (" << GetLastError() << ")";
        return false;
      }
      return true;
    });
  });
#else
  return false;
#endif
}

}