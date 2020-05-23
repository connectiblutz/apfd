#include "run.h"
#include "messagethread.h"

#include <windows.h>

#define UNUSED(x) (void)(x)

SERVICE_STATUS          ssStatus;       // current status of the service
SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD dwErr = 0;

MessageThread* runnerThread = nullptr;

int runCombined() {
  runnerThread = new MessageThread();
  runnerThread->join();
  return 0;
}

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
	if (runnerThread) runnerThread->stop();
}


VOID ServiceStart(DWORD dwArgc, LPTSTR *lpszArgv)
{
  UNUSED(dwArgc);
  UNUSED(lpszArgv);

	// Service initialization
	if (!ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 3000))
	{
		ServiceStop();
		return;
	}

	std::thread mainThread(runCombined);

	if (!ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0))
	{
		ServiceStop();
		return;
	}
	// End of initialization


	// need to idle in ServiceStart so Windows knows the service is running
	mainThread.join();
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
	sshStatusHandle = RegisterServiceCtrlHandlerExA("apfd", service_ctrl, NULL);

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

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
  switch (fdwCtrlType)
  {
    // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
    {
	   if (runnerThread) runnerThread->stop();
     return TRUE;
    }
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
    default:
      return FALSE;
  }
}
int runAsApp() {
  SetConsoleCtrlHandler(CtrlHandler, TRUE);
  return runCombined();
}

int runAsDaemon() {

	SERVICE_TABLE_ENTRY dispatchTable[] =
	{
		{ (const LPSTR) "apfd", (LPSERVICE_MAIN_FUNCTION)service_main },
		{ NULL, NULL }
	};

	if (!StartServiceCtrlDispatcher(dispatchTable))
	{
		return 1;
	}

  return 0;
}
