#include "common/consolehandler.h"

#include <windows.h>

namespace common {

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
  switch (fdwCtrlType)
  {
    // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
    {
      auto s = Singleton::get<ConsoleHandler>();
      if (s) {
        s->thread()->stop();
        return TRUE;
      } else {
       return FALSE;
     }
    }
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
    default:
      return FALSE;
  }
}

ConsoleHandler::ConsoleHandler(std::shared_ptr<MessageThread> thread) {
  _thread=thread;
  SetConsoleCtrlHandler(CtrlHandler, TRUE);
}
ConsoleHandler::~ConsoleHandler() {
  SetConsoleCtrlHandler(NULL, TRUE);
}

}
