#include "install.h"
#include "servicecontrol.h"

namespace apfd {

int installAsService(std::string user, std::string password) {
  auto service = common::ServiceControl("apfd");
  service.setDescription("Auto Port Forward");
  service.install(user,password);
  service.start();
  return 0;
}

int removeAsService() {
  auto service = common::ServiceControl("apfd");
  service.stop();
  service.uninstall();
  return 0;
}

}
