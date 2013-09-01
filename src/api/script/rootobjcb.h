#ifndef __ROOTOBJCB_H
#define __ROOTOBJCB_H

#include "bfc/dispatch.h"
#include "bfc/common.h"


/*  RootObjectCallback is the dispatchable class being called by RootObject script events (onNotify).
    You should not inherit directly from it, but rather from ScriptRootObject.               */

class COMEXP NOVTABLE RootObjectCallback : public Dispatchable {
  public:
    void rootobjectcb_onNotify(const char *a, const char *b, int c, int d);
    enum {
      ROOT_ONNOTIFY=10,
    };
};

inline void RootObjectCallback::rootobjectcb_onNotify(const char *a, const char *b, int c, int d) {
  _voidcall(ROOT_ONNOTIFY, a, b, c, d);
}

class COMEXP NOVTABLE RootObjectCallbackI : public RootObjectCallback {
  public:
    virtual void rootobjectcb_onNotify(const char *a, const char *b, int c, int d)=0;
  protected:
    RECVS_DISPATCH;
};


#endif
