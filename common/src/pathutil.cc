#include "common/pathutil.h"
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

namespace common {

std::filesystem::path PathUtil::binaryExe() {
#ifdef _WIN32
  wchar_t szPath[MAX_PATH];
  if( GetModuleFileName( nullptr, szPath, MAX_PATH ) ) {
    std::filesystem::path module(szPath);
    return module;
  }
#endif
  return std::filesystem::path();
}
std::filesystem::path PathUtil::binaryPath() {
  return PathUtil::binaryExe().parent_path();
}

void PathUtil::chdir(std::filesystem::path path) {
#ifdef _WIN32
  _wchdir(path.wstring().c_str());
#else
  ::chdir(path.string().c_str());
#endif
}

}