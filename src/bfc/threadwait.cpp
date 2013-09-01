#include <precomp.h>
#include "threadwait.h"

ThreadWait::ThreadWait() {
  m_event = NULL;
}

ThreadWait::~ThreadWait() {
  shutdown();
}

void ThreadWait::init(const char *name) {
  m_event = CreateEvent(NULL, TRUE, TRUE, name);
}

void ThreadWait::shutdown() {
  CloseHandle(m_event);
  m_event = NULL;
}

void ThreadWait::set() {
  if (m_event == NULL) return;
  ResetEvent(m_event);
}

void ThreadWait::clear() {
  if (m_event == NULL) return;
  SetEvent(m_event);
}

int ThreadWait::wait(int timeoutms) {
  if (m_event == NULL) return 2;
  DWORD r = WaitForSingleObject(m_event, (timeoutms == -1) ? INFINITE : timeoutms);
  if (r == WAIT_TIMEOUT) return 1;
  return 0;
}
