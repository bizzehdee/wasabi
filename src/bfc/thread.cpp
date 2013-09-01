#include <precomp.h>

#include "thread.h"

#if !defined(WIN32) && !defined(WASABI_PLATFORM_POSIX)
#error port me!
#endif

#pragma warning(push)
#pragma warning(disable : 4229)

THREADCALL Thread::ThreadProc(void *param) {
  Thread *th = static_cast<Thread*>(param);
  long ret = th->threadProc();
  th->isrunning = 0;
  return (THREADCALL)ret;
}

#pragma warning(pop)

Thread::Thread(OSTHREADHANDLE _parent_handle) :
  killswitch(0),
  handle(0), parent_handle(0),
  auto_close_parent(FALSE),
  threadid(0),
  isrunning(0),
  nicekill(1)
{
  setParentHandle(_parent_handle);
}

Thread::~Thread() {
  if (handle != 0) {
    if (isrunning) {
      setKillSwitch();
      end();
    }
#ifdef WIN32
    CloseHandle(handle);
#elif defined(WASABI_PLATFORM_POSIX)
    pthread_kill(handle, SIGTERM);
#endif
  }
  setParentHandle(0);
}

THREADID Thread::start() {
  if (running()) return 1;
  threadid = 0;
#ifdef WIN32
  if (handle) CloseHandle(handle);
  handle = CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, &threadid);
#endif
#ifdef WASABI_PLATFORM_POSIX
  pthread_create(&handle, NULL, ThreadProc, (LPVOID)this);
  threadid = handle;
#endif
  if (handle)
    isrunning = 1;
  else
    threadid = 0;
  return threadid;
}

BOOL Thread::running() {
  return isrunning;
}

int Thread::end() {
  while (running()) Sleep(66);
  return 1;
}

int Thread::kill() {
#ifdef WIN32
  TerminateThread(handle, 0);	// terminate
#elif defined(WASABI_PLATFORM_POSIX)
  pthread_kill(handle, SIGTERM);
  handle = 0;	// not for WIN32
#endif
  threadid = 0;
  isrunning = 0;
  return 1;
}

void Thread::setKillSwitch(int k) {
  killswitch = k;
}

int Thread::getKillSwitch() {
  if (killswitch) return 1;
  if (parent_handle != 0 && !parentRunning()) return 1;
  return 0;
}

THREADID Thread::getThreadId() const {
  return threadid;
}

void Thread::setParentHandle(OSTHREADHANDLE _parent_handle, int _auto_close) {
  if (parent_handle != 0 && auto_close_parent) {
#ifdef WIN32
    CloseHandle(parent_handle);
#else
//#error port me!
  DebugString("setParentHandle???\n");
#endif
  }
  parent_handle = _parent_handle;
  auto_close_parent = _auto_close;
}

int Thread::parentRunning() {
  if (parent_handle == 0) return -1;
#ifdef WIN32
  DWORD exitcode;
  int r = GetExitCodeThread(parent_handle, &exitcode);
  if (r == 0 || exitcode != STILL_ACTIVE) return 0;
  return 1;
#else
//#error port me!
  DebugString("parentRunning???\n");
#endif
}

void Thread::setPriority(int priority) {
  StdCPU::setThreadPriority(priority, handle);
}
