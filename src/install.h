#pragma once

#include <string>

namespace apfd {

int installAsService(std::wstring user=L"", std::wstring password=L"");
int removeAsService();

}
