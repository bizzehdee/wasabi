#ifndef _PROFILER_H
#define _PROFILER_H

#include <bfc/std.h>
#include <bfc/string/string.h>
#include <bfc/ptrlist.h>

/* how to profile something:

PR_ENTER("doSomething()");	// prints debugstring of info in PR_LEAVE
doSomething();
PR_LEAVE();

or 

PR_ENTER_NOPRINT("doSomething()");	// doesn't print debugstring of info
doSomething();
PR_LEAVE();
PR_PRINT("doSomething()"); 	// prints debugstring of info

*/


#ifdef NO_PROFILING
#define PR_ENTER(msg) 
#define PR_ENTER_NOPRINT(msg)
#define PR_LEAVE()
#define PR_PRINT(msg)
#else

#define _PR_ENTER(msg, line) { __Profiler __prx##line(msg) 
#define _PR_ENTER_NOPRINT(msg, line) { __Profiler __prx##line(msg, FALSE) 
#define _PR_ENTER2(msg, msg2, line) { __Profiler __prx##line(msg, msg2) 
#define PR_ENTER(msg) _PR_ENTER(msg, line)
#define PR_ENTER_NOPRINT(msg) _PR_ENTER_NOPRINT(msg, line)
#define PR_PRINT(msg) __Profiler::printInfo(msg)
#define PR_ENTER2(msg, msg2) _PR_ENTER2(msg, msg2, line) 
#define PR_LEAVE() }

class __ProfilerEntry {
  public: 
    __ProfilerEntry(const char *txt) {
      text = txt; totaltime = 0; totaln = 0; subcount = 0; lastcps = -1;
      last_ms = 0;
    }
    virtual ~__ProfilerEntry() {}

    void add(double ms) {
      double now = Std::getTimeStampMS();
      totaltime += ms; totaln++; 
      last_ms += ms;
      if (subcount == 0) {
        firstcall = now;
      }
      if (now - firstcall < 1) {
        subcount++; 
      } else {
        lastcps = subcount;
        subcount = 0;
      }
    }
    void clear_last() { last_ms = 0; }
    double getAverage() {
      if (totaln == 0) return 0;
      return totaltime / (double)totaln;
    }
    double getTotal() { return totaltime; }
    const char *getText() { return text; }
    int getLastCPS() { return lastcps; }
    double getLastTime() { return last_ms; }

  private:
    double last_ms;
    double totaltime;
    int totaln;
    stdtimevalms firstcall;
    int lastcps;
    int subcount;
    String text;
};

class __ProfilerEntrySort {
public:
  static int compareAttrib(const char *attrib, void *item) {
    return STRICMP(attrib, ((__ProfilerEntry*)item)->getText());
  }
  static int compareItem(void *i1, void *i2) {
    return STRICMP(((__ProfilerEntry*)i1)->getText(), ((__ProfilerEntry*)i2)->getText());
  }
};

extern PtrListInsertSorted<__ProfilerEntry, __ProfilerEntrySort> __profiler_entries;
extern int __profiler_indent;

class __ProfilerManager {
  public:
  static void log(const char *txt, double ms, double *total=NULL, double *average=NULL, int *lastcps=NULL) {
    __ProfilerEntry *e = getEntry(txt);
    ASSERT(e != NULL);

    /*CUT if (ms != 0.0)*/ e->add(ms);

    if (total != NULL) *total = e->getTotal();
    if (average != NULL) *average = e->getAverage();
    if (lastcps != NULL) *lastcps = e->getLastCPS();
  }
  static void getStats(const char *txt, double *ms, double *total, double *average, int *lastcps) {
    __ProfilerEntry *e = getEntry(txt);
    ASSERT(e != NULL);

    if (ms != NULL) *ms = e->getLastTime(); e->clear_last();
    if (total != NULL) *total = e->getTotal();
    if (average != NULL) *average = e->getAverage();
    if (lastcps != NULL) *lastcps = e->getLastCPS();
  }
protected:
  static __ProfilerEntry *getEntry(const char *txt) {
    int pos=-1;
    __ProfilerEntry *e = __profiler_entries.findItem(txt, &pos);
    if (pos < 0 || e == NULL) {
      e = new __ProfilerEntry(txt);
      __profiler_entries.addItem(e);
    }
    return e;
  }
};

#undef USE_TICK_COUNT

class __Profiler {
public:
  __Profiler(const char *text, int _print=TRUE) : str(text), print(_print) {
#ifdef USE_TICK_COUNT
    ts1 = GetTickCount();
#else
    ts1 = Std::getTimeStampMS();
#endif
    __profiler_indent++;
  }
  __Profiler(const char *text, const char *text2) : str(text), str2(text2) {
    print=1;
    if (!str2.isempty()) str2 += " ";
#ifdef USE_TICK_COUNT
    ts1 = GetTickCount();
#else
    ts1 = Std::getTimeStampMS();
#endif
    __profiler_indent++;
  }
  ~__Profiler() {
#ifdef USE_TICK_COUNT
    stdtimevalms ts2 = GetTickCount();
#else
    stdtimevalms ts2 = Std::getTimeStampMS();
#endif
    __profiler_indent--;
    double ms = (double)((ts2 - ts1)
#ifndef USE_TICK_COUNT
*1000.0
#endif
);
    double total=0;
    double average=0;
    int lastcps=0;
    __ProfilerManager::log(str, ms, &total, &average, &lastcps);
    if (print) {
      char buf[WA_MAX_PATH]="";
      if (lastcps >= 0) {
        SPRINTF(buf, "%*sProfiler: %s: %s%6.4f ms (total: %6.4f ms, average: %6.4f ms, calls per second : %d)\n", __profiler_indent*4, " ", str.vs(), str2.vs(), ms, total, average, lastcps);
      } else {
        SPRINTF(buf, "%*sProfiler: %s: %s%6.4f ms (total: %6.4f ms, average: %6.4f ms)\n", __profiler_indent*4, " ", str.vs(), str2.vs(), ms, total, average);
      }
      DebugString::outputdebugstring(buf);
    }
  }

  static void printInfo(const char *text) {
    double total=0;
    double average=0;
    int lastcps=0;
    double ms = 0;
    __ProfilerManager::log(text, ms);
    __ProfilerManager::getStats(text, &ms, &total, &average, &lastcps);
    char buf[WA_MAX_PATH]="";
    if (lastcps >= 0) {
      SPRINTF(buf, "%*sProfiler: %s: %6.4f (total: %6.4f ms, average: %6.4f ms, calls per second : %d)\n", __profiler_indent*4, " ", text, ms, total, average, lastcps);
    } else {
      SPRINTF(buf, "%*sProfiler: %s: %6.4f (total: %6.4f ms, average: %6.4f ms)\n", __profiler_indent*4, " ", text, ms, total, average);
    }
    DebugString::outputdebugstring(buf);
  }

private:
  int print;
  String str, str2;
  stdtimevalms ts1;
};

#endif//!NO_PROFILING

#endif
