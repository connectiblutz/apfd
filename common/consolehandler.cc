#include "consolehandler.h"

#include <windows.h>

namespace apfd::common {

namespace singleton {
  std::weak_ptr<ConsoleHandler> _;
}


BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
  switch (fdwCtrlType)
  {
    // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
    {
      auto s = singleton::_.lock();
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


template<class... Args>
std::shared_ptr<ConsoleHandler> ConsoleHandler::make_shared(Args&&... args)
{
  auto obj = new ConsoleHandler(std::forward<Args>(args)...);
  auto made = std::shared_ptr<ConsoleHandler>(obj,D());
  singleton::_=made;
  return made;
}
template std::shared_ptr<ConsoleHandler> ConsoleHandler::make_shared<std::shared_ptr<MessageThread>>(std::shared_ptr<MessageThread>&&);

}
