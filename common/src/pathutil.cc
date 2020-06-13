#include "common/pathutil.h"
#include <windows.h>
#include <direct.h>

namespace common {

std::filesystem::path PathUtil::binaryExe() {
  wchar_t szPath[MAX_PATH];
  if( GetModuleFileName( nullptr, szPath, MAX_PATH ) ) {
    std::filesystem::path module(szPath);
    return module;
  }
  return std::filesystem::path();
}
std::filesystem::path PathUtil::binaryPath() {
  return PathUtil::binaryExe().parent_path();
}

void PathUtil::chdir(std::filesystem::path path) {
		_wchdir(path.wstring().c_str());
}

}