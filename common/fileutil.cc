#include "fileutil.h"

#include <fstream>
#include <sstream>

namespace apfd::common {


std::wstring FileUtil::FileToString(std::filesystem::path file) {
  std::wifstream t(file.wstring());
  if (!t.is_open()) return L"";
  std::wostringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

}