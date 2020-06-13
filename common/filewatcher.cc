#include "filewatcher.h"
#include "logutil.h"
#include <windows.h>
#include <fileapi.h>

namespace apfd::common {

FileWatcher::FileWatcher(std::filesystem::path watchedFile, std::function<void()> onChange) : _enabled(true), _watchedFile(watchedFile), _onChange(onChange) {
  _t=std::thread(&FileWatcher::run,this);
}

FileWatcher::~FileWatcher() {
  _enabled=false;
  if (_t.joinable()) _t.join();
}

void FileWatcher::run() {
  auto parent = std::filesystem::absolute(_watchedFile).parent_path();
  HANDLE dwChangeHandle = FindFirstChangeNotification(parent.wstring().c_str(), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
 
  if (!dwChangeHandle || dwChangeHandle == INVALID_HANDLE_VALUE) 
  {
    LogUtil::Debug()<<"ERROR: FindFirstChangeNotification function failed. "<<GetLastError();
    return; 
  }

  HANDLE hFile = CreateFile(_watchedFile.wstring().c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
  FILETIME lastWrite;
  if (!GetFileTime( hFile, nullptr, nullptr, &lastWrite)) {
    LogUtil::Debug()<<"ERROR: GetFileTime function failed. "<<GetLastError();
  }

  while (_enabled) 
  { 
    DWORD dwWaitStatus = WaitForMultipleObjects(1, &dwChangeHandle,  FALSE, 250); 
 
    switch (dwWaitStatus) 
    { 
      case WAIT_OBJECT_0: 
        FILETIME thisLastWrite;
        if (!GetFileTime( hFile, nullptr, nullptr, &thisLastWrite)) {
          LogUtil::Debug()<<"ERROR: GetFileTime function failed. "<<GetLastError();
        } else {
          if (0!=CompareFileTime(&thisLastWrite,&lastWrite)) {
            lastWrite=thisLastWrite;
            _onChange();
          }
        }
        if ( FindNextChangeNotification(dwChangeHandle) == FALSE )
        {
          LogUtil::Debug()<<"ERROR: FindNextChangeNotification function failed.";
        }
        break; 
 
        case WAIT_TIMEOUT:
        default: 
          break;
      }
   }

  FindCloseChangeNotification(dwChangeHandle);
}

}