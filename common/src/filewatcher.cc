#include "common/filewatcher.h"
#include "common/logutil.h"
#ifdef _WIN32
#include <windows.h>
#include <fileapi.h>
#elif defined(__APPLE__)
#include <sys/event.h>
#include <fcntl.h>
#include <unistd.h>
#else
#include <sys/inotify.h>
#endif

namespace common {

FileWatcher::FileWatcher(std::filesystem::path watchedFile, std::function<void()> onChange) : _enabled(true), _watchedFile(watchedFile), _onChange(onChange) {
  _t=std::thread(&FileWatcher::run,this);
}

FileWatcher::~FileWatcher() {
  _enabled=false;
  if (_t.joinable()) _t.join();
}

void FileWatcher::run() {
#ifdef _WIN32
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
#elif defined(__APPLE__)

    int kq;
    int event_fd;
    struct kevent events_to_monitor[1];
    struct kevent event_data[1];
    struct timespec timeout;
    unsigned int vnode_events;
 
    /* Open a kernel queue. */
    if ((kq = kqueue()) < 0) {
        LogUtil::Debug()<< "Could not open kernel queue. "<< strerror(errno);
        return;
    }
 
    /*
       Open a file descriptor for the file/directory that you
       want to monitor.
     */
    event_fd = open(_watchedFile.c_str(), O_EVTONLY);
    if (event_fd <=0) {
        LogUtil::Debug()<< "The file " << _watchedFile << " could not be opened for monitoring. "<< strerror(errno);
        return;
    }
 
    /* Set the timeout to wake us every half second. */
    timeout.tv_sec = 0;        // 0 seconds
    timeout.tv_nsec = 250000000;    // 250 milliseconds
 
    /* Set up a list of events to monitor. */
    vnode_events = NOTE_WRITE; // NOTE_DELETE |  NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK | NOTE_RENAME | NOTE_REVOKE;
    EV_SET( &events_to_monitor[0], event_fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, vnode_events, 0, (void*)_watchedFile.c_str());
 
    /* Handle events. */
    while (_enabled) {
        int event_count = kevent(kq, events_to_monitor, 1, event_data, 1, &timeout);
        if ((event_count < 0) || (event_data[0].flags == EV_ERROR)) {
            /* An error occurred. */
            LogUtil::Debug()<< "An error occurred (event count " << event_count << "). " << strerror(errno);
            break;
        }
        if (event_count) {
            _onChange();
        }
 
        /* Reset the timeout.  In case of a signal interrruption, the
           values may change. */
        timeout.tv_sec = 0;        // 0 seconds
        timeout.tv_nsec = 500000000;    // 500 milliseconds
    }
    close(event_fd);

#else
#endif
}

}