#ifndef __TIMER_MULTIPLEXER_H
#define __TIMER_MULTIPLEXER_H

#include <bfc/common.h>
#include <bfc/ptrlist.h>

// FG> not too sure how to get a callback for attribute change, if anyone wants to change it be my guest ;)
#define RESOLUTION_CHECK_DELAY 1000 // check for resolution changes every second

// if uioptions CfgItem not found, use this value for resolution
#if !defined(DEFAULT_TIMERRESOLUTION)
#define DEF_RES 20
#else
#define DEF_RES DEFAULT_TIMERRESOLUTION
#endif

// below MAX_TIMER_DELAY, timer are multiplexed using a 'wheel' algorithm (mem used = MAX_TIMER_DELAY/resolution * sizeof(PtrList) + ntimers*sizeof(MultiplexedTimer), but fast (no lookup) )
// above MAX_TIMER_DELAY, resolution drops to MAX_TIMER_DELAY/LOW_RES_DIV and uses ntimers*sizeof(MultiplexedTimer) bytes
#define MAX_TIMER_DELAY 5000  // keep this dividable by LOW_RES_DIV please
#define LOW_RES_DIV      25
// note: MAX_TIMER_DELAY / LOW_RES_DIV is the speed of the base lowres timer 

// aim at an earlier slice than normal. this will NOT make the timer run faster 
// than requested because we check how sooner the timer is compared to what was
// requested. if that value is above resolution/2 (meaning triggering the timer now
// is closer to what was requested than triggering it on the next slice) we trigger
// the client code immediately, otherwise we wait for the next slice.
// this is necessary because due to the processing we
// have to do for each slice, delay accumulates, which makes timers run very slow 
// if their delay is a high multiple of the base resolution. this fixes this problem.
// also note that this is the start value. as time passes, the value is adjusted
// in real time
#define START_FUDGEFACTOR 0.5f

class CfgItem;

class TimerMultiplexerClient {
  public:
    virtual void onMultiplexedTimer(void *data, int skip, int mssincelast)=0;
};

class MultiplexedTimer {
  public:
    MultiplexedTimer(int _ms, void *_data) : ms(_ms), data(_data) {
      nexttick=0;
      flag=0;
      lost = 0;
      lastmscount=0;
      lastdelay=0;
    }
    virtual ~MultiplexedTimer() { }

    int ms;
    void *data;
    DWORD nexttick; // only used by low precision timers
    int flag; // only used by hi precision timers
    float lost; // only used by hi precision timers
    DWORD lastmscount;
    int lastdelay;
};

class TimerMultiplexer {
  public:

    TimerMultiplexer();
    virtual ~TimerMultiplexer();

    virtual void setClient(TimerMultiplexerClient *client);

    virtual void onServerTimer();

    virtual void addTimer(int ms, void *data);
    virtual void removeTimer(void *data);
    virtual void setResolution(int ms);

    virtual void shutdown();
    virtual int getNumTimers();
    virtual int getNumTimersLP();

  private:

    void checkResolution(DWORD now);
    void resetTimer(int newresolution);
    void resetWheel();
    void distributeAll();
    void distribute(MultiplexedTimer *t);
    void runCurSlice(DWORD now);
    void runTimer(DWORD now, DWORD last, MultiplexedTimer *t, PtrList<MultiplexedTimer> *slice, int pos);
    void removeFromWheel(MultiplexedTimer *t);
    void runLowPrecisionTimers(DWORD now);
    void removeFromLowPrecision(MultiplexedTimer *t);
    void doShutdown();
    PtrList<MultiplexedTimer> *getSlice(int n);

    TimerMultiplexerClient *client;
    int resolution;
    DWORD last_resolution_check;
    int timerset;

    int curslice;
    int nslices;
    int justexited;
    int firstevent;
    float fudgefactor;
    int last_timer_time;
    int full_time_spent;
    int timer_counts;

    PtrList< PtrList< MultiplexedTimer > > wheel;
    PtrListInsertSortedByPtrVal< MultiplexedTimer > timers;
    PtrList< MultiplexedTimer > lptimers;
    MultiplexedTimer *running_timer;

    CfgItem *uioptions;
};

class MultiplexerServer {
public:
  MultiplexerServer(TimerMultiplexer *mux, UINT tid) : m_mux(mux), m_tid(tid) {}
  virtual ~MultiplexerServer() {}
  TimerMultiplexer *getMultiplexer() { return m_mux; }
  UINT getId() { return m_tid; }
  void setId(UINT id) { m_tid = id; }
private:
  TimerMultiplexer *m_mux;
  UINT m_tid;
};

class MultiplexerServerComparatorTID {
public:
  // comparator for sorting
  static int compareItem(MultiplexerServer *p1, MultiplexerServer* p2) {
    if (p1->getId() < p2->getId()) return -1;
    if (p1->getId() > p2->getId()) return 1;
    return 0;
  }
  // comparator for searching
  static int compareAttrib(const char *attrib, MultiplexerServer *item) {
    if (*((UINT *)attrib) < item->getId()) return -1;
    if (*((UINT *)attrib) < item->getId()) return 1;
    return 0;
  }
};

class MultiplexerServerComparatorMux{
public:
  // comparator for sorting
  static int compareItem(MultiplexerServer *p1, MultiplexerServer* p2) {
    if (p1->getMultiplexer() < p2->getMultiplexer()) return -1;
    if (p1->getMultiplexer() > p2->getMultiplexer()) return 1;
    return 0;
  }
  // comparator for searching
  static int compareAttrib(const char *attrib, MultiplexerServer *item) {
    if ((TimerMultiplexer *)attrib < item->getMultiplexer()) return -1;
    if ((TimerMultiplexer *)attrib < item->getMultiplexer()) return 1;
    return 0;
  }
};

#endif
