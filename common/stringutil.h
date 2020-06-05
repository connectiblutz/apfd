#pragma once

#include <string>
#include <vector>

namespace apfd::common {

class StringUtil {
  public:
    static std::wstring reverse(const std::wstring& s);
    static std::wstring ltrim(const std::wstring& s);
    static std::wstring rtrim(const std::wstring& s);
    static std::wstring trim(const std::wstring& s);
    static std::wstring asciiToHex(const std::wstring& s);
    static std::vector<std::wstring> split(const std::wstring& s, const wchar_t token);
    static std::wstring toWide(const std::string& s);
    static std::string toNarrow(const std::wstring& s);
};
}