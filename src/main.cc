#include "cJSON.h"

#include "install.h"
#include "run.h"

#include <cstring>

int main(int argc, char** argv) {

  if (argc>1) {
    if (strcmp(argv[1],"--install")==0) {
      return installAsService();
    }
    if (strcmp(argv[1],"--remove")==0) {
      return removeAsService();
    }
    if (strcmp(argv[1],"--debug")==0) {
      return runAsApp();
    }
  }
  return runAsDaemon();
}
