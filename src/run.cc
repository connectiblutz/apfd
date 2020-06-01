#include "run.h"
#include "apfdworker.h"
#include "consolehandler.h"
#include "windowsservice.h"
#include "logutil.h"
#include "pathutil.h"

namespace apfd {

int runAsApp() {
  auto runnerThread = std::make_shared<ApfdWorker>();
  auto mt = std::dynamic_pointer_cast<common::MessageThread>(runnerThread);
  auto ch = common::Singleton::Weak::create<common::ConsoleHandler>(mt);
  runnerThread->join();
  return 0;
}

int runAsDaemon() {
  common::Singleton::Strong::create<common::LogUtil>(std::filesystem::path("apfd.log"));
  common::PathUtil::chdir(common::PathUtil::binaryPath());
  auto runnerThread = std::make_shared<ApfdWorker>();
  auto mt = std::dynamic_pointer_cast<common::MessageThread>(runnerThread);
  auto ws = common::Singleton::Weak::create<common::WindowsService>(std::string("apfd"),mt);
  ws->start();
  return 0;
}

}
