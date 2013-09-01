#include <precomp.h>
#include "critsec.h"


#if !defined(WIN32) && !defined(LINUX)
#error port me!
#endif

// uncomment this if needed
//#define CS_DEBUG

CriticalSection::CriticalSection() {
#ifdef WIN32
  InitializeCriticalSection(&cs);
#elif defined(WASABI_PLATFORM_POSIX)
#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
  pthread_mutex_t recursive = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#else
  pthread_mutex_t recursive = PTHREAD_MUTEX_INITIALIZER; 
  pthread_cond_init(&condition,NULL);
  recursionCount = 0;
  owner = NULL;
#endif 
  cs.mutex = recursive;
#endif // platform

#if defined(CS_DEBUG) && defined(ASSERTS_ENABLED)
  within = 0;
#endif
}

CriticalSection::~CriticalSection() {
#if defined(CS_DEBUG) && defined(ASSERTS_ENABLED)
  ASSERT(!within);
#endif
#ifdef WIN32
  DeleteCriticalSection(&cs);
#elif defined(WASABI_PLATFORM_POSIX)
  pthread_mutex_destroy(&cs.mutex);
#ifndef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
  pthread_cond_destroy(&condition);
#endif
#endif // platform
}

void CriticalSection::enter() {
#ifdef WIN32
  EnterCriticalSection(&cs);
#elif defined(WASABI_PLATFORM_POSIX)
#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
  pthread_mutex_lock(&cs.mutex);
#else
  pthread_t thread = pthread_self();
  pthread_mutex_lock(&cs.mutex);
  if(owner == thread)
    recursionCount++;
  else  {
    while ( recursionCount )
	  pthread_cond_wait(&condition,&cs.mutex);
    recursionCount = 1;
	owner = thread;
  }
  pthread_mutex_unlock(&cs.mutex);  
#endif
#endif // platform

#if defined(CS_DEBUG) && defined(ASSERTS_ENABLED)
//CUT DebugString("within %d", within);
//CUT  ASSERT(!within);
  within++;
#endif
}

void CriticalSection::leave() {
#if defined(CS_DEBUG) && defined(ASSERTS_ENABLED)
  within--;
  ASSERT(within >= 0);
#endif

#ifdef WIN32
  LeaveCriticalSection(&cs);
#elif defined(WASABI_PLATFORM_POSIX)
#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
  pthread_mutex_unlock(&cs.mutex);
#else
  pthread_mutex_lock(&cs.mutex);
  recursionCount--;
  if(!recursionCount)  {
    owner = NULL;
	pthread_cond_signal(&condition);
  }
  pthread_mutex_unlock(&cs.mutex);  
#endif
#endif // platform
}

void CriticalSection::inout() {
  enter();
  leave();
}
