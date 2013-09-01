#include <precomp.h>
#include <api/timer/api_timer.h>
#include <api/timer/timerclient.h>
#include <api/api.h>

#define CBCLASS TimerClientI
START_DISPATCH;
  // this one doesn't map directly so that we can catch deferredcb timers
  VCB(TIMERCLIENT_TIMERCALLBACK,   timerclient_handleDeferredCallback);
  CB(TIMERCLIENT_GETMASTERCLIENT,  timerclient_getMasterClient);
  VCB(TIMERCLIENT_ONMASTERMUX,     timerclient_onMasterClientMultiplex);
  CB(TIMERCLIENT_GETDEPPTR,        timerclient_getDependencyPtr);
  CB(TIMERCLIENT_GETSKIPPED,       timerclient_getSkipped);
  VCB(TIMERCLIENT_SETSKIPPED,      timerclient_setSkipped);
  VCB(TIMERCLIENT_POSTDEFERREDCB , timerclient_postDeferredCallback);
  CB(TIMERCLIENT_ONDEFERREDCB ,    timerclient_onDeferredCallback);
  CB(TIMERCLIENT_GETNAME,          timerclient_getName);
END_DISPATCH;

TimerClientI::TimerClientI() :
  skipped(0), timerdelay(0), disallowset(0)
{ }

TimerClientI::~TimerClientI() {
  disallowset = 1;
  if (cbs.getNumItems() > 0)
    WASABI_API_TIMER->timer_remove(this, DEFERREDCB_TIMERID);
  cbs.deleteAll();
  // unregister any forgotten timerids
  if (WASABI_API_TIMER != NULL)
    for (int i = 0; i < timerids.getNumItems(); i++)
      WASABI_API_TIMER->timer_remove(this, timerids[i]);
}

int TimerClientI::timerclient_setTimer(int id, int ms, int call_immediately) {
  if (disallowset) return 0;
//CUT not any more  ASSERTPR(id > 0, "A timer id cannot be <= 0");
  ASSERTPR(id != DEFERREDCB_TIMERID, "please choose another timer id (conflict with DEFERREDCB_TIMERID)");
  WASABI_API_TIMER->timer_add(this, id, ms);
  if (!timerids.haveItem(id)) timerids.addItem(id);
  if (call_immediately) timerclient_timerCallback(id);
  return 1;
}

int TimerClientI::timerclient_killTimer(int id) {
  WASABI_API_TIMER->timer_remove(this, id);
  timerids.removeItem(id);
  return 1;
}

void TimerClientI::timerclient_postDeferredCallback(int param1, int param2, int mindelay) {
  if (disallowset) return;
  for (int i=0;i<cbs.getNumItems();i++) {
    deferred_callback *cb = cbs.enumItem(i);
    if (cb->param1 == param1 && cb->param2 == param2) {
      cbs.removeByPos(i);
      // BU hmm this should delete it no?
      delete cb;
      break;
    }
  }
  deferred_callback *c = new deferred_callback;
  c->origin = this;
  c->param1 = param1;
  c->param2 = param2;
  cbs.addItem(c);
  WASABI_API_TIMER->timer_add(this, DEFERREDCB_TIMERID, MAX(0, mindelay));
}

void TimerClientI::timerclient_handleDeferredCallback(int id) {

  // let deriving class handle it. note we call into here even if
  // it's the deferred cb id, since older versions did that too,
  // expecting the user to call down on the method.
  timerclient_timerCallback(id);

  // process our deferred cb id
  if (id == DEFERREDCB_TIMERID) {
    WASABI_API_TIMER->timer_remove(this, DEFERREDCB_TIMERID);
    PtrList<deferred_callback> temp(cbs);
    cbs.removeAll();
    foreach(temp)
      deferred_callback *c = temp.getfor();
      c->origin->timerclient_onDeferredCallback(c->param1, c->param2);
      delete c;
    endfor
  }
}
