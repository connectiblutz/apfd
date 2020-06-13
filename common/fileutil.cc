#include "fileutil.h"

#include <fstream>
#include <sstream>

namespace apfd::common {


std::string FileUtil::FileToString(std::filesystem::path file) {
  std::ifstream t(file.wstring());
  if (!t.is_open()) return "";
  std::ostringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

}