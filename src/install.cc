#include "install.h"
#include "servicecontrol.h"

namespace apfd {

int installAsService(std::wstring user, std::wstring password) {
  auto service = common::ServiceControl(L"apfd");
  service.setDescription(L"Auto Port Forward");
  service.install(user,password);
  service.start();
  return 0;
}

int removeAsService() {
  auto service = common::ServiceControl(L"apfd");
  service.stop();
  service.uninstall();
  return 0;
}

}
