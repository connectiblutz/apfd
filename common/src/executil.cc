#include "common/executil.h"
#include <cstdio>
#include <sstream>
#include "common/logutil.h"

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

namespace common {

std::string ExecUtil::Run(std::string command) {
  if (command.empty()) return "";  
  LogUtil::Debug()<<command;
  FILE   *pPipe;
  if( !(pPipe = popen( command.c_str(), "rt" )) ) {
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
    /*int returnCode = */pclose( pPipe ) ;
  }
  LogUtil::Debug()<<buf.str();
  return buf.str();
}

}
