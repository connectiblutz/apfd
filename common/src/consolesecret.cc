#include "common/consolesecret.h"

#include <iostream>
#ifdef _WIN32
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace common {
    
std::string ConsoleSecret::Get(const std::string& prompt) {
    if (!prompt.empty()) {
        std::cout << prompt;
    }
    std::string secret;

#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
    char buffer[1024];
    gets_s(buffer, sizeof buffer);
    secret=buffer;
    SetConsoleMode(hStdin, mode);
#else
    struct termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    struct termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    char* tmpBuffer;
    size_t tmpSize = 0;
    ssize_t read = getline(&tmpBuffer, &tmpSize, stdin);
    if (read) {
        secret=tmpBuffer;
    }
    free(tmpBuffer);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    return secret;
}

}