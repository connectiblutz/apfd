#pragma once

#include <filesystem>
#include <thread>
#include <functional>

namespace common {

class FileWatcher {
  public:
    FileWatcher(std::filesystem::path watchedFile, std::function<void()> onChange);
    ~FileWatcher();
  private:
    void run();
    bool _enabled;
    std::filesystem::path _watchedFile;
    std::function<void()> _onChange;
    std::thread _t;
};

}