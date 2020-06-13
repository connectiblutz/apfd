#include "cJSON.h"

#include "install.h"
#include "run.h"
#include "common/stringutil.h"

#include <cstring>

int main(int argc, char** argv) {

  if (argc>1) {
    if (strcmp(argv[1],"--install")==0) {
      auto user = argc>2?argv[2]:"";
      auto pass = argc>3?argv[3]:"";
      return apfd::installAsService(user,pass);
    }
    if (strcmp(argv[1],"--remove")==0) {
      return apfd::removeAsService();
    }
    if (strcmp(argv[1],"--debug")==0) {
      return apfd::runAsApp();
    }
  }
  return apfd::runAsDaemon();
}
