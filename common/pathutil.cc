#include "pathutil.h"
#include <windows.h>
#include <direct.h>

namespace apfd::common {

std::filesystem::path PathUtil::binaryExe() {
  char szPath[MAX_PATH];
  if( GetModuleFileNameA( nullptr, szPath, MAX_PATH ) ) {
    std::filesystem::path module(szPath);
    return module;
  }
  return std::filesystem::path();
}
std::filesystem::path PathUtil::binaryPath() {
  return PathUtil::binaryExe().parent_path();
}

void PathUtil::chdir(std::filesystem::path path) {
		_chdir(path.string().c_str());
}

}