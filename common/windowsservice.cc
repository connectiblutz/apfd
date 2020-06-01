#include "windowsservice.h"
#include "logutil.h"
#include <windows.h>

#define UNUSED(x) (void)(x)

namespace apfd::common {

SERVICE_STATUS          ssStatus;       // current status of the service
SERVICE_STATUS_HANDLE   sshStatusHandle;

BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;
	BOOL fResult = TRUE;

	if (dwCurrentState == SERVICE_START_PENDING)
		ssStatus.dwControlsAccepted = 0;
	else
		ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_POWEREVENT;

	ssStatus.dwCurrentState = dwCurrentState;
	ssStatus.dwWin32ExitCode = dwWin32ExitCode;
	ssStatus.dwWaitHint = dwWaitHint;

	if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
		ssStatus.dwCheckPoint = 0;
	else
		ssStatus.dwCheckPoint = dwCheckPoint++;

	// Report the status of the service to the service control manager.
	fResult = SetServiceStatus(sshStatusHandle, &ssStatus);

	return fResult;
}

VOID ServiceStop()
{
	ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 3000);
  auto s = Singleton::get<WindowsService>();
	if (s) {
    s->thread()->stop();
  }
}


VOID ServiceStart(DWORD dwArgc, LPTSTR *lpszArgv)
{
  UNUSED(dwArgc);
  UNUSED(lpszArgv);

	// Service initialization
	if (!ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0))
	{
		ServiceStop();
		return;
	}
	// End of initialization


	// need to idle in ServiceStart so Windows knows the service is running
    auto s = Singleton::get<WindowsService>();
    if (s) {
      s->thread()->join();
    }
}


DWORD WINAPI service_ctrl( __in  DWORD dwCtrlCode, __in  DWORD dwEventType, __in  LPVOID lpEventData, __in  LPVOID lpContext)
{
  UNUSED(dwEventType);
  UNUSED(lpEventData);
  UNUSED(lpContext);

	DWORD retCode = NO_ERROR;
	// Handle the requested control code.
	switch (dwCtrlCode)
	{
		// Stop the service.
		// SERVICE_STOP_PENDING should be reported before setting the Stop Event - hServerStopEvent - in
		// ServiceStop().  This avoids a race condition which may result in a 1053 - The Service did not respond... error.
	case SERVICE_CONTROL_STOP:
		ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);
		ServiceStop();
		retCode = NO_ERROR;
		return retCode;

		// Update the service status.
	case SERVICE_CONTROL_INTERROGATE:
		ReportStatusToSCMgr(ssStatus.dwCurrentState, NO_ERROR, 0);
		retCode = NO_ERROR;
		break;
	default:
    {
    }
		break;

	}

	return retCode;
}


void WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// register our service control handler:
  std::string name;
  auto s = Singleton::get<WindowsService>();
	if (s) {
    name = s->name();
  }
	sshStatusHandle = RegisterServiceCtrlHandlerExA(name.c_str(), service_ctrl, NULL);

	if (!sshStatusHandle)
	{
		goto cleanup;
	}

	// SERVICE_STATUS members that don't change in example
	ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ssStatus.dwServiceSpecificExitCode = 0;

	// report the status to the service control manager.
	if (!ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 3000))
	{
		goto cleanup;
	}

	ServiceStart(dwArgc, lpszArgv);

	ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);


cleanup:

	return;
}

WindowsService::WindowsService(std::string name,std::shared_ptr<MessageThread> thread) {
  _name=name;
  _thread=thread;
}

void WindowsService::start() {
	SERVICE_TABLE_ENTRYA dispatchTable[] =
	{
		{ (const LPSTR) _name.c_str(), (LPSERVICE_MAIN_FUNCTIONA)service_main },
		{ NULL, NULL }
	};
	StartServiceCtrlDispatcherA(dispatchTable);
}


bool WindowsService::Install(std::string name, std::string description) {
	WindowsService::Uninstall(name);

    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    char szPath[MAX_PATH];

    if( !GetModuleFileNameA( nullptr, szPath, MAX_PATH ) ) {
        LogUtil::Debug()<<"Cannot install service (" << GetLastError() << ")";
        return false;
    }

    // Get a handle to the SCM database.  
    schSCManager = OpenSCManagerA( 
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 
 
    if (NULL == schSCManager) {
        LogUtil::Debug()<<"OpenSCManager failed (" << GetLastError() << ")";
        return false;
    }

    // Create the service
    schService = CreateServiceA( 
        schSCManager,              // SCM database 
        name.c_str(),                   // name of service 
        description.c_str(),                   // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_AUTO_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        szPath,                    // path to service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password 
 
    if (schService == NULL) {
        LogUtil::Debug()<<"CreateService failed (" << GetLastError() << ")";
        CloseServiceHandle(schSCManager);
        return false;
    }

	if (!StartServiceA(schService,0,nullptr)) {
        LogUtil::Debug()<<"StartService failed (" << GetLastError() << ")";
        CloseServiceHandle(schSCManager);
        return false;
	}

    CloseServiceHandle(schService); 
    CloseServiceHandle(schSCManager);

	return true;
}

bool WindowsService::Uninstall(std::string name) {
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    SERVICE_STATUS ssStatus; 
	SERVICE_STATUS_PROCESS ssp;

    // Get a handle to the SCM database. 
    schSCManager = OpenSCManagerA( 
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 
 
    if (NULL == schSCManager) 
    {
		LogUtil::Debug()<<"OpenSCManager failed (" << GetLastError() << ")";
        return false;
    }

    // Get a handle to the service.
    schService = OpenServiceA( 
        schSCManager,       // SCM database 
        name.c_str(),          // name of service 
        SERVICE_ALL_ACCESS);            // need delete access 
 
    if (schService == NULL)
    { 
        LogUtil::Debug()<<"OpenService failed (" << GetLastError() << ")";
        CloseServiceHandle(schSCManager);
        return (GetLastError()==0 || GetLastError()==ERROR_SERVICE_DOES_NOT_EXIST);
    }

	if ( !ControlService(
            schService, 
            SERVICE_CONTROL_STOP, 
            (LPSERVICE_STATUS) &ssp ) )
    {
        LogUtil::Debug()<<"ControlService failed (" << GetLastError() << ")";
		//CloseServiceHandle(schService); 
		//CloseServiceHandle(schSCManager);
		//return false;
    }

    // Delete the service.
    if (! DeleteService(schService) ) 
    {
		LogUtil::Debug()<<"DeleteService failed (" << GetLastError() << ")";
		CloseServiceHandle(schService); 
		CloseServiceHandle(schSCManager);
        return false;
    }
 
    CloseServiceHandle(schService); 
    CloseServiceHandle(schSCManager);

	return true;
}

}
