#include "run.h"
#include "apfdworker.h"
#include "consolehandler.h"
#include "windowsservice.h"

namespace apfd {

int runAsApp() {
  auto runnerThread = std::make_shared<ApfdWorker>();
  auto ch = common::ConsoleHandler::make_shared(std::dynamic_pointer_cast<common::MessageThread>(runnerThread));
  runnerThread->join();
  return 0;
}

int runAsDaemon() {
  auto runnerThread = std::make_shared<ApfdWorker>();
  auto ws = common::WindowsService::make_shared(std::string("apfd"),std::dynamic_pointer_cast<common::MessageThread>(runnerThread));
  return 0;
}

}
