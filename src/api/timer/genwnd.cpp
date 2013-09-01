#include <precomp.h>
#include "genwnd.h"
#include <api/api.h>
#include <api/timer/tmultiplex.h>

GenWnd::~GenWnd() {
}

void GenWnd::postDeferredFunctionCallback(int p1, int p2, deferredCallbackFunction f) {
  cbft *c = new cbft;
  c->f = f;
  c->p1 = p1;
  c->p2 = p2;
  postDeferredCallback(ID_FUNC, (int)c);
}

int GenWnd::onDeferredCallback(int p1, int p2) {
  switch (p1) {
    case ID_FUNC: {
      cbft *c = (cbft *)p2;
      c->f(c->p1, c->p2);
      delete c;
      break;
    }
  }
  return GENWND_PARENT::onDeferredCallback(p1, p2);
}

int GenWnd::allocateNewTimer(int delay, userTimerProc p, void *d1, void *d2) {
  ASSERTPR(genwnd != NULL, "allocate new timer but genwnd has not been set yet");
  userTimerProcStruct *utps = new userTimerProcStruct;
  utps->fn = p;
  utps->data1 = d1;
  utps->data2 = d2;
  utps->id = hi++;
  ASSERTPR(hi != 0, "exhausted timer ids, fix reollover");
  genwnd->setTimer(utps->id, delay);
  timerprocs.addItem(utps);
  return utps->id;
}

void GenWnd::deallocateTimer(int id) {
  for (int i=0;i<timerprocs.getNumItems();i++) {
    userTimerProcStruct *s = timerprocs.enumItem(i);
    if (s->id == id) {
      genwnd->killTimer(s->id);
      timerprocs.removeByPos(i);
      delete s;
      return;
    }
  }
}

int GenWnd::onPostOnInit() {
  GENWND_PARENT::onPostOnInit();
  return 1;
}

void GenWnd::timerCallback(int id) {
  if (id < 255) {
    GENWND_PARENT::timerCallback(id);
    return;
  }
  for (int i=0;i<timerprocs.getNumItems();i++) {
    userTimerProcStruct *s = timerprocs.enumItem(i);
    if (s->id == id) {
      s->fn(s->id, s->data1, s->data2);
      return;
    }
  }
  GENWND_PARENT::timerCallback(id);
}

void GenWnd::setTimerGenWnd(GenWnd *t) {
  genwnd = t;
}

PtrList<userTimerProcStruct> GenWnd::timerprocs;
int GenWnd::hi=255;
GenWnd *GenWnd::genwnd=NULL;

