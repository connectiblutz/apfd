#include "run.h"
#include "apfdworker.h"
#include "common/consolehandler.h"
#include "common/windowsservice.h"
#include "common/logutil.h"
#include <filesystem>
#include <string>

#ifndef _WIN32
#include <unistd.h>
#endif

namespace apfd {

int runAsApp() {
  auto runnerThread = std::make_shared<ApfdWorker>();
  auto mt = std::dynamic_pointer_cast<common::MessageThread>(runnerThread);
  auto ch = common::Singleton::Weak::create<common::ConsoleHandler>(mt);
  runnerThread->join();
  return 0;
}

int runAsDaemon() {
  auto logPath = std::filesystem::temp_directory_path();
  logPath/="apfd.log";
  common::Singleton::Strong::create<common::LogUtil>(logPath);
  auto runnerThread = std::make_shared<ApfdWorker>();
  auto mt = std::dynamic_pointer_cast<common::MessageThread>(runnerThread);
#ifdef _WIN32
  auto ws = common::Singleton::Weak::create<common::WindowsService>(std::string("apfd"),mt);
  ws->start();
#else
  if (fork()==0) {
    mt->join();
  }
#endif
  return 0;
}

}
