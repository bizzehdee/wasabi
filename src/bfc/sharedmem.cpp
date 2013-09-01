#include <precomp.h>
#include "sharedmem.h"

#ifdef LINUX
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#endif

SharedMem::SharedMem(int size, const char *ipc_name) {
  reset();
  init(size, ipc_name);
}

SharedMem::~SharedMem() {
  shutdown();
}

void SharedMem::reset() {
  m_handle = 0;
  m_size = -1;
  m_lockedMem = NULL;
  m_lockedSize = 0;
  m_first = 0;
}

int SharedMem::init(int size, const char *ipc_name) {
  shutdown();
  if (size != -1 && ipc_name != NULL && *ipc_name) {
#ifdef WIN32
    m_handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, ipc_name);
    m_first = ( GetLastError() != ERROR_ALREADY_EXISTS );
#elif defined(LINUX)
    m_handle = shm_open(ipc_name, O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
    if (m_handle == -1) {
      m_first = 1;
      m_handle = shm_open(ipc_name, O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
      if (m_handle == -1) m_handle = 0;
      else {
        char tmp[size];
        write(m_handle, tmp, size);
      }
      m_first = 0;
    }
#endif
    if (m_handle == 0) { 
      reset();
      return 0;
    } else {
      m_size = size;
      m_name = ipc_name;
    }
  }
  return 1;
}


int SharedMem::shutdown() {
  if (m_lockedMem != NULL) unlock();
  if (m_handle != 0) {
#ifdef WIN32
    CloseHandle(m_handle);
#elif defined(LINUX)
    close(m_handle);
#endif
  }
  reset();
  return 1;
}

void *SharedMem::lock(int offset, int size) {
  if (size == -1) size = m_size-offset;
  unlock();
  if (m_handle != 0) {
#ifdef WIN32
    m_lockedMem = MapViewOfFile(m_handle, FILE_MAP_WRITE, 0, offset, size);
#elif defined(LINUX)
    m_lockedMem = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, m_handle, offset);
    if (m_lockedMem == (void *)-1) m_lockedMem = NULL;
#endif
    if (m_lockedMem != NULL) m_lockedSize = size;
    return m_lockedMem;
  }
  else return NULL;
}

void SharedMem::unlock() {
  if (m_lockedMem) {
#ifdef WIN32
    UnmapViewOfFile(m_lockedMem);
#elif defined(LINUX)
    munmap(m_lockedMem, m_lockedSize);
#endif
  }
  m_lockedMem = NULL;
  m_lockedSize = 0;
}

