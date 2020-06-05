#include "executil.h"
#include <cstdio>
#include <sstream>
#include "logutil.h"

namespace apfd::common {

std::wstring ExecUtil::Run(std::wstring command) {
  if (command.empty()) return L"";  
  LogUtil::Debug()<<command;
  FILE   *pPipe;
  if( !(pPipe = _wpopen( command.c_str(), L"rt" )) ) {
   return L"";
  }
  std::wostringstream buf;
  wchar_t   psBuffer[1024];
  while(fgetws(psBuffer, sizeof psBuffer, pPipe))
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
