#ifndef WASABI_SHAREDMEM_H
#define WASABI_SHAREDMEM_H

#include <bfc/string/string.h>

class SharedMem {
public:
  SharedMem(int size=-1, const char *ipc_name=NULL);
  ~SharedMem();

  int init(int size, const char *ipc_name);
  int shutdown();

  void *lock(int offset=0, int size=-1);
  void unlock();

  int getSize() { return m_size; }
  int getLockedSize() { return m_lockedSize; }
  void *getLockedMem() { return m_lockedMem; }
  const char *getName() { return m_name; }
  int isFirst() { return m_first; }

private:
  void reset(); 
  void *m_lockedMem;
  int m_lockedSize;
  String m_name;
  int m_size;
  int m_first;

#ifdef WIN32
    HANDLE m_handle;
#elif defined(LINUX)
    int m_handle;
#endif
};

#endif
