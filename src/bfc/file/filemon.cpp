#include <precomp.h>
#include "filemon.h"

#define FILEMON_TIMER 54

FileMonWatch::FileMonWatch(const char *filename) {
  m_filename = filename;
  m_lastwrite.dwHighDateTime = m_lastwrite.dwLowDateTime = 0;
  WIN32_FIND_DATA fd;
  HANDLE handle;
  if ((handle = FindFirstFile(filename, &fd)) != INVALID_HANDLE_VALUE) {
    m_lastwrite = fd.ftLastWriteTime;
    FindClose(handle);
  }
}

void FileMonWatch::check(FileModificationMonitor *monitor) {
  WIN32_FIND_DATA fd;
  HANDLE handle;
  if ((handle = FindFirstFile(m_filename, &fd)) != INVALID_HANDLE_VALUE) {
    if (m_lastwrite.dwHighDateTime != fd.ftLastWriteTime.dwHighDateTime || m_lastwrite.dwLowDateTime != fd.ftLastWriteTime.dwLowDateTime) {
      m_lastwrite = fd.ftLastWriteTime;
      monitor->onFileModified(m_filename);
    }
    FindClose(handle);
  } else if (m_lastwrite.dwHighDateTime != 0 || m_lastwrite.dwLowDateTime != 0) {
    monitor->onFileDeleted(m_filename);
    m_lastwrite = fd.ftLastWriteTime;
  }
}

FileModificationMonitor::FileModificationMonitor(int delay) : m_delay(delay) {
}

FileModificationMonitor::~FileModificationMonitor() {
  m_watches.deleteAll();
}

void FileModificationMonitor::beginWatchFile(const char *file) {
  m_watches.addItem(new FileMonWatch(file));
  if (m_watches.getNumItems() == 1) {
    timerclient_setTimer(FILEMON_TIMER, m_delay);
  }
}

void FileModificationMonitor::endWatchFile(const char *file) {
  FileMonWatch *w = m_watches.findItem(file);
  if (w) m_watches.removeItem(w);
  if (m_watches.getNumItems() == 0) {
    timerclient_killTimer(FILEMON_TIMER);
  }
}

void FileModificationMonitor::timerclient_timerCallback(int id) {
  if (id == FILEMON_TIMER) {
    foreach(m_watches)
      m_watches.getfor()->check(this);
    endfor;
  }
}

