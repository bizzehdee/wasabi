#ifndef __TIMER_CLIENT_H
#define __TIMER_CLIENT_H

#include <bfc/dispatch.h>
#include <bfc/common.h>
#include <bfc/depend.h>
#include <bfc/tlist.h>

#define DEFERREDCB_TIMERID -2

class TimerClient;

typedef struct {
  TimerClient *origin;
  int param1;
  int param2;
} deferred_callback;

class NOVTABLE TimerClient : public Dispatchable {
protected:
  TimerClient() { }	

public:
  int timerclient_setTimer(int id, int ms, int call_immediately=FALSE);
  int timerclient_killTimer(int id);
  void timerclient_postDeferredCallback(int param1, int param2=0, int mindelay=0);
  int timerclient_onDeferredCallback(int param1, int param2);
  void timerclient_timerCallback(int id);

  TimerClient *timerclient_getMasterClient(); 
  void timerclient_onMasterClientMultiplex();
  Dependent *timerclient_getDependencyPtr(); 
  void timerclient_setSkipped(int s);
  int timerclient_getSkipped();
  void timerclient_setTimerDelay(int td);
  int timerclient_getTimerDelay();
  const char *timerclient_getName();

  enum {
    TIMERCLIENT_TIMERCALLBACK   = 100,
    TIMERCLIENT_SETTIMER        = 110,
    TIMERCLIENT_KILLTIMER       = 120,
    TIMERCLIENT_GETMASTERCLIENT = 130,
    TIMERCLIENT_ONMASTERMUX     = 140,
    TIMERCLIENT_GETDEPPTR       = 150,
    TIMERCLIENT_SETSKIPPED      = 160,
    TIMERCLIENT_GETSKIPPED      = 170,
    TIMERCLIENT_SETTIMERDELAY   = 180,
    TIMERCLIENT_GETTIMERDELAY   = 190,
    TIMERCLIENT_POSTDEFERREDCB  = 200,
    TIMERCLIENT_ONDEFERREDCB    = 210,
    TIMERCLIENT_GETNAME         = 220,
  };
};

inline void TimerClient::timerclient_timerCallback(int id) {
  _voidcall(TIMERCLIENT_TIMERCALLBACK, id);
}

inline int TimerClient::timerclient_setTimer(int id, int ms, int call_immediately) {
  return _call(TIMERCLIENT_SETTIMER, 0, id, ms, call_immediately);
}

inline int TimerClient::timerclient_killTimer(int id) {
  return _call(TIMERCLIENT_KILLTIMER, 0, id);
}

inline TimerClient *TimerClient::timerclient_getMasterClient() {
  return _call(TIMERCLIENT_GETMASTERCLIENT, (TimerClient *)NULL);
}

inline void TimerClient::timerclient_onMasterClientMultiplex() {
  _voidcall(TIMERCLIENT_ONMASTERMUX);
}

inline Dependent *TimerClient::timerclient_getDependencyPtr() {
  return _call(TIMERCLIENT_GETDEPPTR, (Dependent *)NULL);
}

inline void TimerClient::timerclient_setSkipped(int s) {
  _voidcall(TIMERCLIENT_SETSKIPPED, s);
}

inline int TimerClient::timerclient_getSkipped() {
  return _call(TIMERCLIENT_GETSKIPPED, 0);
}

inline void TimerClient::timerclient_setTimerDelay(int td) {
  _voidcall(TIMERCLIENT_SETTIMERDELAY, td);
}

inline int TimerClient::timerclient_getTimerDelay() {
  return _call(TIMERCLIENT_GETTIMERDELAY, 0);
}

inline void TimerClient::timerclient_postDeferredCallback(int param1, int param2, int mindelay) {
  _voidcall(TIMERCLIENT_POSTDEFERREDCB, param1, param2, mindelay);
}

inline int TimerClient::timerclient_onDeferredCallback(int param1, int param2) {
  return _call(TIMERCLIENT_ONDEFERREDCB, 0, param1, param2);
}

inline const char *TimerClient::timerclient_getName() {
  return _call(TIMERCLIENT_GETNAME, (const char *)NULL);
}

class NOVTABLE TimerClientI : public TimerClient {
protected:
  TimerClientI();

public:
  virtual ~TimerClientI();

  virtual int timerclient_setTimer(int id, int ms, int call_immediately=FALSE);
  virtual int timerclient_killTimer(int id);

  // override this to catch your timer events
  virtual void timerclient_timerCallback(int id) { }

  virtual TimerClient *timerclient_getMasterClient() { return NULL; }
  virtual void timerclient_onMasterClientMultiplex() { }
  virtual Dependent *timerclient_getDependencyPtr()=0;
  virtual void timerclient_setSkipped(int s) { skipped = s; }
  virtual int timerclient_getSkipped() { return skipped; }
  virtual void timerclient_setTimerDelay(int td) { timerdelay = td; }
  virtual int timerclient_getTimerDelay() { return timerdelay; }
  virtual void timerclient_postDeferredCallback(int param1, int param2=0, int mindelay=0);
  virtual int timerclient_onDeferredCallback(int param1, int param2) { return 1; }
  virtual const char *timerclient_getName() { return NULL; }
  
protected:
  RECVS_DISPATCH;

private:
  virtual void timerclient_handleDeferredCallback(int id);

  int skipped;
  int timerdelay;
  int disallowset;
  PtrList<deferred_callback> cbs;
  wasabi::TList<int> timerids;
};

class NOVTABLE TimerClientDI : public TimerClientI, public DependentI {
protected:
  TimerClientDI() { }

public:
  Dependent *timerclient_getDependencyPtr() { return this; }
};


#endif
