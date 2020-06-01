#include "install.h"
#include "windowsservice.h"

namespace apfd {

int installAsService() {
  common::WindowsService::Install("apfd","Auto Port Forward");
  return 0;
}

int removeAsService() {
  common::WindowsService::Uninstall("apfd");
  return 0;
}

}
