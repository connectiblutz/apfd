#include "stringutil.h"
#include <cctype>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace apfd::common {

std::string StringUtil::reverse(const std::string& s) {
  return std::string(s.rbegin(),s.rend());
}

std::string StringUtil::ltrim(const std::string &s) {
  auto begin = std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);});
  return std::string(begin,s.end());
}
std::string StringUtil::rtrim(const std::string &s) {
  auto end = std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c);});
  return StringUtil::reverse(std::string(end,s.rend()));
}
std::string StringUtil::trim(const std::string &s) {
  return StringUtil::ltrim(rtrim(s));
}

std::string StringUtil::asciiToHex(const std::string& s) {
  std::ostringstream converted;
  for (unsigned int i = 0; i < s.size(); ++i) {
    converted << std::setfill('0') << std::setw(2) << std::hex << (int)(s[i]);
  }
  return converted.str();
}

std::vector<std::string> StringUtil::split(const std::string& s, const char token) {
  std::vector<std::string> output;
  std::string::size_type prev_pos = 0, pos = 0;
  while((pos = s.find(token, pos)) != std::string::npos)
  {
    std::string substring( s.substr(prev_pos, pos-prev_pos) );
    output.push_back(substring);
    prev_pos = ++pos;
  }
  output.push_back(s.substr(prev_pos, pos-prev_pos));
  return output;
}

}