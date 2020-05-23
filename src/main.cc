#include "cJSON.h"

#include "install.h"
#include "run.h"

#include <cstring>

int main(int argc, char** argv) {

  if (argc>0) {
    if (strcmp(argv[0],"--install")==0) {
      return installAsService();
    }
    if (strcmp(argv[0],"--remove")==0) {
      return removeAsService();
    }
    if (strcmp(argv[0],"--debug")==0) {
      return runAsApp();
    }
  }
  return runAsDaemon();
}
