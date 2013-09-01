#ifndef _BFC_STD_CPU_H
#define _BFC_STD_CPU_H

#include <bfc/platform/platform.h>

// 31-may-2004 BU created

// CPU-specific methods. If we ever need to detect SSE or something put it
// here.

typedef unsigned long THREADID, PROCESSID;

namespace StdCPU {
  // this is the number of logical CPUs, i.e. hyperthreaded ones too
  int getNumCPUs();
  // this number is for physical CPUs, not fooled by hyperthreading
  int getNumPhysicalCPUs();

  // a unique # returned by the OS
  THREADID getCurrentThreadId();
  // get current
  PROCESSID getCurrentProcessId();
  // attempts to adjust thread priority compared to main thread
  // normal range is from -2 .. +2, and -32767 for idle, 32767 for time critical
  // use the constants from bfc/thread.h
  void setThreadPriority(int delta, OSTHREADHANDLE thread_handle = NULL);
};

#endif
