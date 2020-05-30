#include "fileutil.h"

#include <fstream>
#include <sstream>

namespace apfd::common {


std::string FileUtil::FileToString(std::filesystem::path file) {
  std::ifstream t(file.string());
  if (!t.is_open()) return "";
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

}