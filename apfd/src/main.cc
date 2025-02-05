#include "cJSON.h"

#include "install.h"
#include "run.h"
#include <bcl/stringutil.h>
#include <bcl/consolesecret.h>

#include <cstring>

int main(int argc, char** argv) {

  if (argc>1) {
#ifdef _WIN32
    if (strcmp(argv[1],"--install")==0) {
      auto user = std::string(argc>2?argv[2]:"");
      auto pass = std::string(argc>3?argv[3]:"");
      if (pass.empty()) {
        pass = bcl::ConsoleSecret::Get("Enter password : ");
      }
      return apfd::installAsService(user,pass);
    }
    if (strcmp(argv[1],"--remove")==0) {
      return apfd::removeAsService();
    }
#endif
    if (strcmp(argv[1],"--debug")==0) {
      return apfd::runAsApp();
    }
  }
  return apfd::runAsDaemon();
}
