#include "run.h"
#include "apfdworker.h"
#include <bcl/consolehandler.h>
#include <bcl/windowsservice.h>
#include <bcl/logutil.h>
#include <filesystem>
#include <string>

#ifndef _WIN32
#include <unistd.h>
#endif

namespace apfd {

int runAsApp() {
  auto runnerThread = std::make_shared<ApfdWorker>();
  auto mt = std::dynamic_pointer_cast<bcl::MessageThread>(runnerThread);
  auto ch = bcl::Singleton::Weak::create<bcl::ConsoleHandler>(mt);
  runnerThread->join();
  return 0;
}

int runAsDaemon() {
  auto logPath = std::filesystem::temp_directory_path();
  logPath/="apfd.log";
  bcl::Singleton::Strong::create<bcl::LogUtil>(logPath);
  auto runnerThread = std::make_shared<ApfdWorker>();
  auto mt = std::dynamic_pointer_cast<bcl::MessageThread>(runnerThread);
#ifdef _WIN32
  auto ws = bcl::Singleton::Weak::create<bcl::WindowsService>(std::string("apfd"),mt);
  ws->start();
#else
  if (fork()==0) {
    mt->join();
  }
#endif
  return 0;
}

}
