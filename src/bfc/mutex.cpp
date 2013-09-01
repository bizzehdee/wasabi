#include <precomp.h>

#include <fcntl.h>

#include "mutex.h"

Mutex::Mutex(const char *ipc_name/* =NULL */) {
  reset();
  init(ipc_name);
}

Mutex::~Mutex() {
  shutdown();
}

void Mutex::reset() {
#ifdef WIN32
  m_readerEvent = NULL;
  m_globalEvent = NULL;
  m_writerMutex = NULL;
  m_countMutex = NULL;
#elif defined(LINUX)
  m_semaphore = NULL; 
#endif
  m_locked = 0;
  m_countptr = NULL;
}

int Mutex::init(const char *name) {
  shutdown();
  if (name && *name) {
    String m_name = name;
    String mtx = String("mutex.") + m_name + ".";
    String shm_name = mtx + "shared_mem";
    if (m_sharedMemory.init(sizeof(int), shm_name)) {
      m_countptr = (volatile int *)m_sharedMemory.lock();
      if (!m_countptr) { shutdown(); return 0; }
      if (m_sharedMemory.isFirst()) *m_countptr = -1;
#ifdef WIN32
      String wrt_name = mtx + "write_mutex";
      String cnt_name = mtx + "count_mutex";
      String gbl_name = mtx + "global_event";
      String rdr_name = mtx + "reader_event";
      m_readerEvent = CreateEvent(NULL, TRUE, FALSE, rdr_name);
      m_globalEvent = CreateEvent(NULL, FALSE, TRUE, gbl_name);
      m_writerMutex = CreateMutex(NULL, FALSE, wrt_name);
      m_countMutex = CreateMutex(NULL, FALSE, cnt_name);
      if (!m_globalEvent || !m_writerMutex || !m_readerEvent || !m_countMutex) {
        shutdown();
        return 0;
      }
#elif defined(LINUX)
      String sem_name = mtx + "semaphore";
      m_semaphore = sem_open(sem_name.getValue(), O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);
      if (m_semaphore == (sem_t *)SEM_FAILED) { shutdown(); return 0; }
#endif
      return 1;
    }
  }
  return 0;
}

int Mutex::shutdown() {
#ifdef WIN32
  if (m_globalEvent) CloseHandle(m_globalEvent);
  if (m_writerMutex) CloseHandle(m_writerMutex);
  if (m_readerEvent) CloseHandle(m_readerEvent);
  if (m_countMutex) CloseHandle(m_countMutex);
#elif defined(LINUX)
  sem_close(m_semaphore);
#endif
  m_sharedMemory.shutdown();
  reset();
  return 1;
}

int Mutex::readlock(int timeout/* =-1 */) {
#ifdef WIN32
  if (m_globalEvent == NULL) return 0;
  WaitForSingleObject(m_countMutex, INFINITE);
  if (InterlockedIncrement((long *)m_countptr) == 0) { 
    ReleaseMutex(m_countMutex);
    if (WaitForSingleObject(m_globalEvent, (timeout == -1) ? INFINITE : timeout) != WAIT_OBJECT_0) return 0;
    SetEvent(m_readerEvent);
  } else {
    ReleaseMutex(m_countMutex);
  }
  if (WaitForSingleObject(m_readerEvent, (timeout == -1) ? INFINITE : timeout) != WAIT_OBJECT_0) return 0;
  m_locked = 1;
  return 1;
#elif defined(LINUX)
  return sem_wait_timeout(m_semaphore, timeout);
#else
  return 0;
#endif
}

int Mutex::writelock(int timeout/* =-1 */) {
#ifdef WIN32
  if (m_globalEvent == NULL) return 0;
  if (WaitForSingleObject(m_writerMutex, (timeout == -1) ? INFINITE : timeout) != WAIT_OBJECT_0) return 0;
  if (WaitForSingleObject(m_globalEvent, (timeout == -1) ? INFINITE : timeout) != WAIT_OBJECT_0) {
    ReleaseMutex(m_writerMutex);
    return 0;
  }
  m_locked = 2;
  return 1;
#elif defined(LINUX)
  return sem_wait_timeout(m_semaphore, timeout);
#else
  return 0;
#endif
}

void Mutex::unlock() {
#ifdef WIN32
  if (m_globalEvent == NULL) return;
  if (m_locked == 2) {
    SetEvent(m_globalEvent);
    ReleaseMutex(m_writerMutex);
  } else if (m_locked == 1) {
    WaitForSingleObject(m_countMutex, INFINITE);
    if (InterlockedDecrement((long *)m_countptr) < 0) { 
      ResetEvent(m_readerEvent);
      SetEvent(m_globalEvent);
    }
    ReleaseMutex(m_countMutex);
  }
#elif defined(LINUX)
  sem_post( m_semaphore );
#endif
}

#ifdef LINUX
int Mutex::sem_wait_timeout(sem_t *sem, int timeout) {
  DWORD start = GetTickCount();
  do {
    if (sem_trywait(sem) != 0) {
      if (errno == EAGAIN) return 0;
      if (GetTickCount() > start + timeout) return 0;
      struct timespec ts = { 0, 0 };
      ts.tv_sec = 0;
      ts.tv_nsec = 100000;
      nanosleep( &ts, NULL);
    } else return 1;
  } while (1);
}
#endif

