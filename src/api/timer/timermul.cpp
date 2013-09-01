#include <precomp.h>
#include <bfc/platform/platform.h>
#include "timermul.h"

#include <api/api.h>
#ifdef WASABI_COMPILE_CONFIG
#include <bfc/attrib/attribs.h>
#include <bfc/attrib/cfgitem.h>
#endif

PtrListQuickSorted<MultiplexerServer, MultiplexerServerComparatorTID> servers_tid;
PtrListQuickSorted<MultiplexerServer, MultiplexerServerComparatorTID> servers_mux;

TimerMultiplexer::TimerMultiplexer() {
  timerset = 0;
  nslices = 0;
  resolution = -1;
  last_resolution_check = 0;
  client = NULL;
  curslice = 0;
  running_timer = NULL;
  uioptions = NULL;
  justexited = 0;
  firstevent = 1;
  fudgefactor = START_FUDGEFACTOR;
  timer_counts = 0;
  full_time_spent = 0;
  last_timer_time = 0;
  resetTimer(50); // initial, is changed for config value on first event
  checkResolution(Std::getTickCount());
}

TimerMultiplexer::~TimerMultiplexer() {
  doShutdown();
}

void TimerMultiplexer::setClient(TimerMultiplexerClient *_client) {
  client = _client;
}

void TimerMultiplexer::addTimer(int ms, void *data) {
  if (ms < 0) { DebugString("Timer with negative delay set, ignored coz the time machine service isn't ready yet\n"); }
  MultiplexedTimer *t = new MultiplexedTimer(ms, data);
  if (ms >= MAX_TIMER_DELAY) {
    lptimers.addItem(t);
    t->nexttick = Std::getTickCount() + t->ms;
  } else {
    t->lastmscount = Std::getTickCount();
    timers.addItem(t);
    if (nslices > 0)
      distribute(t);
  }
}

void TimerMultiplexer::removeTimer(void *data) {

  if (running_timer && running_timer->data == data)
    running_timer = NULL;

  int i;
  for (i=0;i<timers.getNumItems();i++) {
    MultiplexedTimer *t = timers.enumItem(i);
    if (t->data == data) {
      removeFromWheel(t);
      timers.removeByPos(i);
      delete t;
      return;
    }
  }
  for (i=0;i<lptimers.getNumItems();i++) {
    MultiplexedTimer *t = lptimers.enumItem(i);
    if (t->data == data) {
      removeFromLowPrecision(t);
      delete t;
      return;
    }
  }
}

void TimerMultiplexer::setResolution(int ms) {
  resolution = ms;
}

void TimerMultiplexer::shutdown() {
  doShutdown();
}

void TimerMultiplexer::doShutdown() {
  timers.deleteAll();
  wheel.deleteAll();
  lptimers.deleteAll();
  if (timerset) {
    MultiplexerServer *s = servers_mux.findItem((const char *)this);
    if (s) {
#ifdef WIN32
      KillTimer(NULL, s->getId());
#elif defined(LINUX)

#endif
      servers_mux.removeItem(s);
      servers_tid.removeItem(s);
      delete s;
    }
    timerset = 0;
  }
}

void TimerMultiplexer::checkResolution(DWORD now) {
  if (last_resolution_check < now - RESOLUTION_CHECK_DELAY) {
    last_resolution_check = now;
#ifdef WASABI_COMPILE_CONFIG
//    if (uioptions == NULL) {
      // {9149C445-3C30-4e04-8433-5A518ED0FDDE}
      const GUID uioptions_guid =
      { 0x9149c445, 0x3c30, 0x4e04, { 0x84, 0x33, 0x5a, 0x51, 0x8e, 0xd0, 0xfd, 0xde } };
      uioptions = WASABI_API_CONFIG->config_getCfgItemByGuid(uioptions_guid);
//    }
    int nresolution = uioptions ? _intVal(uioptions, "Multiplexed timers resolution") : DEF_RES;
#else
  int nresolution = DEF_RES;
#endif
    nresolution = MAX(10, MIN(MAX_TIMER_DELAY/LOW_RES_DIV, nresolution));
    if (nresolution != resolution) {
      resetTimer(nresolution);
      resolution = nresolution;
      resetWheel();
    }
  }
}

#ifdef _WIN64
#define IDEVENTTYPE UINT_PTR
#else
#define IDEVENTTYPE UINT
#endif
VOID CALLBACK timerMultiplexerServerProc(OSWINDOWHANDLE hwnd, UINT uMsg, IDEVENTTYPE idEvent, DWORD dwTime) {
  MultiplexerServer *s = servers_tid.findItem((const char *)&idEvent);
  if (s) s->getMultiplexer()->onServerTimer();
}

void TimerMultiplexer::resetTimer(int newresolution) {
  if (timerset) {
    MultiplexerServer *s = servers_mux.findItem((const char *)this);
    if (s)
      KillTimer(INVALIDOSWINDOWHANDLE, s->getId());
  }

  // linux port implements settimer
  UINT id = (UINT)SetTimer(INVALIDOSWINDOWHANDLE, 0, newresolution, timerMultiplexerServerProc);
  MultiplexerServer *s = servers_mux.findItem((const char *)this);
  if (!s) {
    s = new MultiplexerServer(this, id);
    servers_mux.addItem(s);
    servers_tid.addItem(s);
  } else {
    s->setId(id);
    servers_tid.sort();
  }
  timerset = 1;
}

PtrList<MultiplexedTimer> *TimerMultiplexer::getSlice(int n) {
  ASSERT(nslices > 0);
  PtrList<MultiplexedTimer> *slice = wheel.enumItem(n % nslices);
  ASSERTPR(slice != NULL, StringPrintf("Returning NULL slice in TimerMultiplexer::getSlice - nslices = %d - n = %d", nslices, n));
  return slice;
}

void TimerMultiplexer::resetWheel() {

  wheel.deleteAll();

  nslices = MAX_TIMER_DELAY / resolution;

  for (int i=0;i<nslices;i++)
    wheel.addItem(new PtrList< MultiplexedTimer >);

  curslice = 0;
  distributeAll();
}

void TimerMultiplexer::distributeAll() {
  for (int i=0;i<timers.getNumItems();i++) {
    distribute(timers.enumItem(i));
  }
}

void TimerMultiplexer::distribute(MultiplexedTimer *t) {
  ASSERT(t != NULL);

  int delay = t->ms;

  int slice = (int)(delay * fudgefactor / resolution + curslice);
  PtrList<MultiplexedTimer> *l = getSlice(slice);

  ASSERT(l != NULL);

  l->addItem(t);
}

void TimerMultiplexer::onServerTimer() {

  justexited = 0;

  DWORD now = Std::getTickCount();

  checkResolution(now);

  timer_counts++;
  full_time_spent += last_timer_time ? (now - last_timer_time) : resolution;
  last_timer_time = now;
  fudgefactor = ((timer_counts * resolution) / (float)full_time_spent) * 0.95f;
  if (full_time_spent >= 2000 || timer_counts == 0xFFFFFFFF) {
    timer_counts /= 2;
    full_time_spent /= 2;
  }

  runCurSlice(now);

  if ((curslice % (nslices/LOW_RES_DIV)) == 0) { // execute low precision timers every MAX_TIMER_DELAY/LOW_RES_DIV
    runLowPrecisionTimers(now);
  }

  if (!justexited) {
    curslice++;
    curslice %= nslices;
  }

  justexited = 1;

  if (firstevent) {
    firstevent = 0;
    checkResolution(Std::getTickCount());
  }
}

void TimerMultiplexer::runCurSlice(DWORD now) {
  //DebugString("Running slice %d\n", curslice);
  PtrList<MultiplexedTimer> *slice = getSlice(curslice);
  ASSERT(slice != NULL);

  // mark them clean
  int i;
  for (i=0;i<slice->getNumItems();i++)
    slice->enumItem(i)->flag = 0;

  // run events
  int n;
  do {
    n = 0;
    for (i=0;i<slice->getNumItems();i++) {
      MultiplexedTimer *t = slice->enumItem(i);
      if (t == NULL) break; // do not remove this line even if you think it's useless
      // t might have been removed by a previous runTimer in this slice, so see if it's still here and if not, ignore
      if (!timers.haveItem(t)) { slice->removeItem(t); i--; continue; }
      if (t->flag == 1) continue;
      t->flag = 1;
      int lastdelay = MAX(0, (int)(now - t->lastmscount));
      DWORD last = t->lastmscount;
      if (last == 0) last = now;
      else
        if ((now-last) < (DWORD)t->ms) {
          int remaining = (t->ms - (now-last));
          if (remaining > (resolution / 2)) {
            slice->removeByPos(i);
            i--;
            PtrList<MultiplexedTimer> *next = getSlice(curslice+1);
            next->addItem(t);
            continue;
          }
        }
      t->lastmscount = now;
      t->lastdelay = lastdelay;
      running_timer = t;
      runTimer(now, last, t, slice, i);
      i--;
// -----------------------------------------------------------------------
// WARNING
//
// below this line, you can no longer assume that t is pointing at valid
// memory, because runTimer can eventually call removeTimer
// -----------------------------------------------------------------------
      n++;
    }
  } while (n > 0);
}

void TimerMultiplexer::runTimer(DWORD now, DWORD last, MultiplexedTimer *t, PtrList<MultiplexedTimer> *slice, int pos) {
  //DebugString("fudgefactor = %f", fudgefactor);
  //i think all these INT's should be DWORD_PTR (with apropriate linux/darwin defs made in their platform files
  //this will allow for code compatability with 32bit and 64bit platforms
  int nextslice = (int)(curslice + t->ms * fudgefactor / resolution);
  int spent = now - last;
  int lost = spent - t->ms;

  if (lost > 0) {
    t->lost += (float)lost / (float)t->ms;
  }

  PtrList<MultiplexedTimer> *next = getSlice(nextslice);
  ASSERT(next != NULL);

  if (slice == next) {
    nextslice++;
    next = getSlice(nextslice);
  }

  slice->removeByPos(pos);
  next->addItem(t);

  int skip = (int)t->lost;
  t->lost -= (int)t->lost;
  if (client) {
    client->onMultiplexedTimer(t->data, skip, t->lastdelay);
// -----------------------------------------------------------------------
// WARNING
//
// below this line, you can no longer assume that t is pointing at valid
// memory, because onMultiplexedTimer can eventually call removeTimer
// -----------------------------------------------------------------------
  }
}

void TimerMultiplexer::removeFromWheel(MultiplexedTimer *t) {
  for (int i=0;i<nslices;i++) {
    PtrList<MultiplexedTimer> *slice = getSlice(i);
    for (int j=0;j<slice->getNumItems();j++) {
      if (slice->enumItem(j) == t) {
        slice->removeByPos(j);
        j--;
      }
    }
  }
}

void TimerMultiplexer::removeFromLowPrecision(MultiplexedTimer *t) {
  for (int i=0;i<lptimers.getNumItems();i++) {
    if (lptimers.enumItem(i) == t) {
      lptimers.removeByPos(i);
      i--;
    }
  }
}

void TimerMultiplexer::runLowPrecisionTimers(DWORD now) {
  int restart;
  do {
    restart = 0;
    for (int i=0;i<lptimers.getNumItems();i++) {
      MultiplexedTimer *t = lptimers.enumItem(i);
      if (t->nexttick < now) {
        if (client) {
          running_timer = t;
          int res = (int)(MAX_TIMER_DELAY/LOW_RES_DIV);
          t->lost += (now - t->nexttick) / t->ms;
          int skip = (int)t->lost;
          t->lost -= skip; // remove integer part
          DWORD last = t->lastmscount;
          t->lastdelay = now-last;
          t->lastmscount = now;
          t->nexttick = t->nexttick+(t->ms)*(skip+1);
          client->onMultiplexedTimer(t->data, skip, t->lastdelay);
// -----------------------------------------------------------------------
// WARNING
//
// below this line, you can no longer assume that t is pointing at valid
// memory, because onMultiplexedTimer can eventually call removeTimer
// -----------------------------------------------------------------------
        }
        if (running_timer == NULL) { // onMultiplexedTimer called removeTimer
          restart =1;
          break;
        }
      }
    }
  } while (restart);
}


int TimerMultiplexer::getNumTimers() {
  return timers.getNumItems();
}

int TimerMultiplexer::getNumTimersLP() {
  return lptimers.getNumItems();
}
