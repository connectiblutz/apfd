#include "install.h"
#include "windowsservice.h"

namespace apfd {

int installAsService(std::wstring user, std::wstring password) {
  common::WindowsService::Install(L"apfd",L"Auto Port Forward",user,password);
  return 0;
}

int removeAsService() {
  common::WindowsService::Uninstall(L"apfd");
  return 0;
}

}
