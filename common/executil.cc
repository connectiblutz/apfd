#include "executil.h"
#include <cstdio>
#include <sstream>
#include "logutil.h"

namespace apfd::common {

std::string ExecUtil::Run(std::string command) {
  if (command.empty()) return "";  
  LOG(std::cout,command);
  FILE   *pPipe;
  if( !(pPipe = _popen( command.c_str(), "rt" )) ) {
   return "";
  }
  std::stringstream buf;
  char   psBuffer[1024];
  while(fgets(psBuffer, sizeof psBuffer, pPipe))
  {
    buf << psBuffer << std::endl;
  }
  if (feof( pPipe))
  {
    /*int returnCode = */_pclose( pPipe ) ;
  }
  LOG(std::cout,buf.str());
  return buf.str();
}

}
