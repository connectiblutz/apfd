#include "run.h"
#include "messagethread.h"
#include "consolehandler.h"
#include "windowsservice.h"

namespace apfd {

int runAsApp() {
  auto runnerThread = new common::MessageThread();
  auto ch = common::ConsoleHandler::make_shared(std::shared_ptr<common::MessageThread>(runnerThread));
  runnerThread->join();
  return 0;
}

int runAsDaemon() {
  auto ws = common::WindowsService::make_shared(std::string("apfd"),std::make_shared<common::MessageThread>());
  return 0;
}

}
