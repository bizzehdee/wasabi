#ifndef _FILEMON_H
#define _FILEMON_H

#include <api/timer/timerclient.h>

class FileModificationMonitor;

class FileMonWatch {
public:
  FileMonWatch(const char *filename);
  const char *getFilename() { return m_filename; }
  void check(FileModificationMonitor *monitor);
private:
  String m_filename;
  FILETIME m_lastwrite;
};

class WatchComparator {
public:
  // comparator for sorting
  static int compareItem(FileMonWatch *p1, FileMonWatch *p2) {
    return STRCMP(p1->getFilename(), p2->getFilename());
  }
  // comparator for searching
  static int compareAttrib(const char *attrib, FileMonWatch *item) {
    return STRCMP(attrib, item->getFilename());
  }
};


class FileModificationMonitor : public TimerClientDI {
public:
  FileModificationMonitor(int delay=1000);
  virtual ~FileModificationMonitor();

  virtual void beginWatchFile(const char *file);
  virtual void endWatchFile(const char *file);
  virtual void onFileModified(const char *filename) {}
  virtual void onFileDeleted(const char *filename) {}

  virtual void timerclient_timerCallback(int id);

private:
  PtrListInsertSorted<FileMonWatch, WatchComparator> m_watches;
  int m_delay;
};

#endif

