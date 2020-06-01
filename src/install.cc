#include "install.h"
#include "windowsservice.h"

namespace apfd {

int installAsService(std::string user, std::string password) {
  common::WindowsService::Install("apfd","Auto Port Forward",user,password);
  return 0;
}

int removeAsService() {
  common::WindowsService::Uninstall("apfd");
  return 0;
}

}
