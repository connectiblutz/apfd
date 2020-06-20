#include "install.h"
#include <bcl/servicecontrol.h>

namespace apfd {

int installAsService(std::string user, std::string password) {
  auto service = bcl::ServiceControl("apfd");
  service.setDescription("Auto Port Forward");
  service.install(user,password);
  service.start();
  return 0;
}

int removeAsService() {
  auto service = bcl::ServiceControl("apfd");
  service.stop();
  service.uninstall();
  return 0;
}

}
