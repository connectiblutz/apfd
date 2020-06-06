#include "wslutil.h"
#include "executil.h"
#include "stringutil.h"
#include "logutil.h"
#include "WslApiLoader.h"
#include <windows.h>
#include <wslapi.h>
#include <sstream>

namespace apfd::wsl {

#define WSL_VERSION_2 0x08

unsigned long WslUtil::getVersion(const std::wstring& distro) {
  auto loader =  WslApiLoader(distro);   
  ULONG Version, DefaultUID, DefaultEnvCnt;
  WSL_DISTRIBUTION_FLAGS WslFlags;
  PSTR* DefaultEnv = NULL;
  if (S_OK == loader.WslGetDistributionConfiguration(&Version,&DefaultUID,&WslFlags,&DefaultEnv,&DefaultEnvCnt)) {
    /*printf("Version: %lu\n"
            "DefaultUID: %lu\n"
            "Default Environment Variables Count: %lu\n",
            Version, DefaultUID, DefaultEnvCnt);
    for (unsigned int i = 0; i < DefaultEnvCnt; i++) {
      printf("Default Environment Variables Array[%u]: %s\n",i,DefaultEnv[i]);
    }
    if (WslFlags&WSL_DISTRIBUTION_FLAGS_ENABLE_INTEROP) printf("WslFlags: WSL_DISTRIBUTION_FLAGS_ENABLE_INTEROP\n");
    if (WslFlags&WSL_DISTRIBUTION_FLAGS_APPEND_NT_PATH) printf("WslFlags: WSL_DISTRIBUTION_FLAGS_APPEND_NT_PATH\n");
    if (WslFlags&WSL_DISTRIBUTION_FLAGS_ENABLE_DRIVE_MOUNTING) printf("WslFlags: WSL_DISTRIBUTION_FLAGS_ENABLE_DRIVE_MOUNTING\n");
    if (WslFlags&WSL_VERSION_2) printf("WslFlags: WSL_VERSION_2\n");*/

    for (unsigned int i = 0; i < DefaultEnvCnt; i++) {
      free(DefaultEnv[i]);
    }

    // Version, as returned by WslGetDistributionConfiguration, is wrong. Check flags for (undocumented) 0x08 instead
    // see https://github.com/Biswa96/wslbridge2/issues/11#issuecomment-544376625
    Version=(WslFlags&WSL_VERSION_2)?2:1;

    common::LogUtil::Debug()<<L"wsl version "<<Version;
    return Version;
  }
  return 1;
}

std::wstring WslUtil::getIP(const std::wstring& distro, const std::wstring& intf) {
  std::wstring output = run(distro,L"ip -4 addr show "+intf+L" | grep -oP '(?<=inet\\s)\\d+(\\.\\d+){3}'");
  return common::StringUtil::trim(output);
}

std::wstring WslUtil::run(const std::wstring& distro, const std::wstring& command) {
  std::wstring ret;
  // Create a pipe to read the output of the launched process.
  HANDLE readPipe;
  HANDLE writePipe;
  SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, true};
  auto loader =  WslApiLoader(distro);   
  if (CreatePipe(&readPipe, &writePipe, &sa, 0)) {
    // Query the UID of the supplied username.
    common::LogUtil::Debug()<<command;
    HANDLE child;
    HRESULT hr = loader.WslLaunch(command.c_str(), true, GetStdHandle(STD_INPUT_HANDLE), writePipe, GetStdHandle(STD_ERROR_HANDLE), &child);
    if (SUCCEEDED(hr)) {
      // Wait for the child to exit and ensure process exited successfully.
      WaitForSingleObject(child, INFINITE);
      DWORD exitCode;
      if ((GetExitCodeProcess(child, &exitCode) == false) || (exitCode != 0)) {
        hr = E_INVALIDARG;
      }
      CloseHandle(child);
      char buffer[1024];
      DWORD bytesRead;
      std::wostringstream strbuf;
      while (true) {
        DWORD bytesAvail = 0;
        if (!PeekNamedPipe(readPipe, NULL, 0, NULL, &bytesAvail, NULL)) {
            break;
        }
        if (bytesAvail==0) break;
        if (ReadFile(readPipe, buffer, (sizeof(buffer)-1), &bytesRead, nullptr)) {
          if (bytesRead==0) break;
          buffer[bytesRead] = ANSI_NULL;
          strbuf<<common::StringUtil::toWide(buffer);
        }
      }
      ret = strbuf.str();
    }
    CloseHandle(readPipe);
    CloseHandle(writePipe);
  }
  common::LogUtil::Debug()<<ret;
  return ret;
}

}