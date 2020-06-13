#pragma once

#include <string>

namespace apfd {

int installAsService(std::string user="", std::string password="");
int removeAsService();

}
