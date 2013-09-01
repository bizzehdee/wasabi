//NONPORTABLE (api is portable, implementation is not)
#ifndef _CRITSEC_H
#define _CRITSEC_H

#ifdef WIN32
#include <windows.h>
#endif

#include <bfc/common.h>
#include <bfc/assert.h>

//#define DEBUG_CS

#ifdef DEBUG_CS
#include <bfc/string/string.h>
#endif

/**
  CriticalSection is a portable object that implements a critical section,
  which is to say, it ensures that no two threads can be in that same
  section of code at the same time. Usually you make them a global object
  or allocate them with new and pass them to both threads.

  @short Critical section class.
  @author Nullsoft
  @ver 1.0
  @see Thread
  @see InCriticalSection
*/

class CriticalSection {
public:
  CriticalSection();
  virtual ~CriticalSection();

/**
  Enters the critical section. If another thread is already in the critical
  section, the calling thread will be blocked until the other thread calls
  leave().
  @see leave()
*/
  void enter();
/**
  Leaves the critical section. If another thread is currently blocked on
  this critical section, it will be unblocked. If multiple threads are blocking
  only one will be unblocked.
  @see enter()
*/
  void leave();

/**
  Calls enter() and leave() in quick succession. Useful to make sure that no
  other thread is in the critical section (although another thread could
  immediately re-enter)
  @see enter()
  @see leave()
*/
  void inout();

private:
#ifdef ASSERTS_ENABLED
  int within;
#endif
  CRITICAL_SECTION cs;
#ifndef WIN32
#ifndef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
  unsigned int recursionCount;
  pthread_t owner;
  pthread_cond_t condition;
#endif
#endif
};

/**
  This is a little helper class to ease the use of class CriticalSection.
  When it is instantiated, it enters a given critical section. When it is
  destroyed, it leaves the given critical section.
    CriticalSection a_cs;
    void blah() {
      InCriticalSection cs(a_cs); // critical section protection begins
      if (test) {
        return 0; // critical section protection ends
      }
      // critical section protection still active!
      doSomething();
      return 1; // critical section protection ends
    }
  
  @author Nullsoft
  @see CriticalSection
*/
class InCriticalSection {
public:
#ifdef DEBUG_CS
  InCriticalSection(CriticalSection *cs, const char *file=NULL, int line=0, const char *name=NULL)
    : m_cs(cs), m_file(file), m_line(line), m_name(name) { 
    if (file && line && name) {
      DebugString("%s(%d) : trying to enter critical section (%s)\n", file, line, name);
    }
    m_cs->enter(); 
    if (file && line && name) {
      DebugString("%s(%d) : entered critical section (%s)\n", file, line, name);
    }
  }
  InCriticalSection(CriticalSection &cs, const char *file=NULL, int line=0, const char *name=NULL)
  : m_cs(&cs), m_file(file), m_line(line), m_name(name) { 
    if (file && line && name) {
      DebugString("%s(%d) : trying to enter critical section (%s)\n", file, line, name);
    }
    m_cs->enter(); 
    if (file && line && name) {
      DebugString("%s(%d) : entered critical section (%s)\n", file, line, name);
    }
  }
  ~InCriticalSection() { 
    DebugString("%s(%d) : leaving critical section (%s)\n", m_file.getValue(), m_line, m_name.getValue());
    m_cs->leave(); 
    DebugString("%s(%d) : left critical section (%s)\n", m_file.getValue(), m_line, m_name.getValue());
  }
#else
  InCriticalSection(CriticalSection *cs) : m_cs(cs) { m_cs->enter(); }
  InCriticalSection(CriticalSection &cs) : m_cs(&cs) { m_cs->enter(); }
  ~InCriticalSection() { m_cs->leave(); }
#endif
private:
  CriticalSection *m_cs;
#ifdef DEBUG_CS
  String m_file, m_name;
  int m_line;
#endif
};

#ifdef DEBUG_CS
#define INCRITICALSECTION(x) _INCRITICALSECTION(__FILE__, __LINE__, x)
#define _INCRITICALSECTION(fid, lid, x) InCriticalSection __I_C_S__##id(x, fid, lid, #x)
#else
#define INCRITICALSECTION(x) _INCRITICALSECTION(__LINE__, x)
#define _INCRITICALSECTION(id, x) InCriticalSection __I_C_S__##id(x)
#endif


#endif
