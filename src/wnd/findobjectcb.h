#ifndef _FINDOBJECTCALLBACK_H
#define _FINDOBJECTCALLBACK_H

#include <bfc/dispatch.h>

class RootWnd;

class NOVTABLE FindObjectCallback : public Dispatchable {	
protected:
  virtual ~FindObjectCallback() {}

public:

  int findobjectcb_matchObject(RootWnd *object);
  
  enum {
    FINDOBJECTCB_MATCHOBJECT = 0,
  };
};

inline int FindObjectCallback::findobjectcb_matchObject(RootWnd *object) {
  return _call(FINDOBJECTCB_MATCHOBJECT, 0, object);
}


class _FindObjectCallback : public FindObjectCallback {
public:
  virtual int findobjectcb_matchObject(RootWnd *object)=0;

protected:
  RECVS_DISPATCH;
};


class FindObjectCallbackI : public _FindObjectCallback {
public:
  FindObjectCallbackI() {}
  virtual ~FindObjectCallbackI() {}

  virtual int findobjectcb_matchObject(RootWnd *object)=0;
};

#endif
