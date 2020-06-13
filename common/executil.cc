#include "executil.h"
#include <cstdio>
#include <sstream>
#include "logutil.h"

namespace apfd::common {

std::string ExecUtil::Run(std::string command) {
  if (command.empty()) return "";  
  LogUtil::Debug()<<command;
  FILE   *pPipe;
  if( !(pPipe = _popen( command.c_str(), "rt" )) ) {
   return "";
  }
  std::ostringstream buf;
  char psBuffer[1024];
  while(fgets(psBuffer, sizeof psBuffer, pPipe))
  {
    buf << psBuffer;
  }
  if (feof( pPipe))
  {
    /*int returnCode = */_pclose( pPipe ) ;
  }
  LogUtil::Debug()<<buf.str();
  return buf.str();
}

}
