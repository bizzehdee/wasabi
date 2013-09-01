#ifndef WASABI_MUTEX_H
#define WASABI_MUTEX_H

#include <bfc/sharedmem.h>

#ifdef LINUX
#include <pthread.h>
#include <semaphore.h>
#endif

class Mutex {
public:
  Mutex(const char *ipc_name=NULL);
  virtual ~Mutex();

  int init(const char *name);
  int shutdown();

  int readlock(int timeout=-1);
  int writelock(int timeout=-1);
  void unlock();
  int islocked() { return m_locked != 0; }
  int isreadlocked() { return m_locked == 1; }
  int iswritelocked() { return m_locked == 2; }
  const char *getName() { return m_name; }

private:
  void reset();

  int m_locked; // 0, 1 (read), 2 (write)

  SharedMem m_sharedMemory;
  int volatile *m_countptr;

  String m_name;

#ifdef WIN32
  HANDLE m_readerEvent;
  HANDLE m_globalEvent;
  HANDLE m_writerMutex;
  HANDLE m_countMutex;
#elif defined(LINUX)
  int sem_wait_timeout(sem_t *sem, int timeout);
  sem_t *m_semaphore;
#endif
};

class InReadMutex {
public:
  InReadMutex(Mutex &m, int timeout=-1) { m_mutex = &m; m_succeeded = m.readlock(timeout); }
  ~InReadMutex() { m_mutex->unlock(); }

  int isIn() { return m_succeeded; }

private:
  Mutex *m_mutex;
  int m_succeeded;
};

class InWriteMutex {
public:
  InWriteMutex(Mutex &m, int timeout=-1) { m_mutex = &m; m_succeeded = m.writelock(timeout); }
  ~InWriteMutex() { m_mutex->unlock(); }

  int isIn() { return m_succeeded; }

private:
  Mutex *m_mutex;
  int m_succeeded;
};

#define BEGINREADMUTEX(x) { InReadMutex __irm(x); \
if (__irm.isIn()) {

#define BEGINWRITEMUTEX(x) { InWriteMutex __irm(x); \
if (__irm.isIn()) {

#define BEGINREADMUTEXTIMEOUT(x, t) { InReadMutex __irm(x, t); \
if (__irm.isIn()) {

#define BEGINWRITEMUTEXTIMEOUT(x, t) { InWriteMutex __irm(x, t); \
if (__irm.isIn()) {

#define ONMUTEXTIMEOUT } else {

#define ENDMUTEX } }

#define ENDMUTEX } }


#endif
