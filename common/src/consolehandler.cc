#include "common/consolehandler.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace common {

#ifdef _WIN32
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
#endif

ConsoleHandler::ConsoleHandler(std::shared_ptr<MessageThread> thread) {
  _thread=thread;
#ifdef _WIN32
  SetConsoleCtrlHandler(CtrlHandler, TRUE);
#endif
}
ConsoleHandler::~ConsoleHandler() {
#ifdef _WIN32
  SetConsoleCtrlHandler(NULL, TRUE);
#endif
}

}
