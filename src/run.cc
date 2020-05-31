#include "run.h"
#include "apfdworker.h"
#include "consolehandler.h"
#include "windowsservice.h"

namespace apfd {

int runAsApp() {
  auto runnerThread = std::make_shared<ApfdWorker>();
  auto mt = std::dynamic_pointer_cast<common::MessageThread>(runnerThread);
  auto ch = common::Singleton::Weak::create<common::ConsoleHandler>(mt);
  runnerThread->join();
  return 0;
}

int runAsDaemon() {
  auto runnerThread = std::make_shared<ApfdWorker>();
  auto mt = std::dynamic_pointer_cast<common::MessageThread>(runnerThread);
  auto ws = common::Singleton::Weak::create<common::WindowsService>(std::string("apfd"),mt);
  return 0;
}

}
