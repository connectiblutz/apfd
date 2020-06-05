#include "stringutil.h"
#include <cctype>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <codecvt>

namespace apfd::common {

std::wstring StringUtil::reverse(const std::wstring& s) {
  return std::wstring(s.rbegin(),s.rend());
}

std::wstring StringUtil::ltrim(const std::wstring &s) {
  auto begin = std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);});
  return std::wstring(begin,s.end());
}
std::wstring StringUtil::rtrim(const std::wstring &s) {
  auto end = std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c);});
  return StringUtil::reverse(std::wstring(end,s.rend()));
}
std::wstring StringUtil::trim(const std::wstring &s) {
  return StringUtil::ltrim(rtrim(s));
}

std::wstring StringUtil::asciiToHex(const std::wstring& s) {
  std::wostringstream converted;
  for (unsigned int i = 0; i < s.size(); ++i) {
    converted << std::setfill(L'0') << std::setw(2) << std::hex << (int)(s[i]);
  }
  return converted.str();
}

std::vector<std::wstring> StringUtil::split(const std::wstring& s, const wchar_t token) {
  std::vector<std::wstring> output;
  std::wstring::size_type prev_pos = 0, pos = 0;
  while((pos = s.find(token, pos)) != std::wstring::npos)
  {
    std::wstring substring( s.substr(prev_pos, pos-prev_pos) );
    output.push_back(substring);
    prev_pos = ++pos;
  }
  output.push_back(s.substr(prev_pos, pos-prev_pos));
  return output;
}

std::wstring StringUtil::toWide(const std::string& s) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  std::wstring wide = converter.from_bytes(s);
  return wide;
}

std::string StringUtil::toNarrow(const std::wstring& s) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  std::string narrow = converter.to_bytes(s);
  return narrow;
}

}