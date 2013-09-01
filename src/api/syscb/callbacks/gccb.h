#ifndef _GCCB_H
#define _GCCB_H

#include "syscbi.h"

namespace GarbageCollectCallback {
  enum {
    GARBAGECOLLECT=1,
  };
};

#define GARBAGECOLLECTCALLBACK_PARENT SysCallbackI
class GarbageCollectCallbackI : public GARBAGECOLLECTCALLBACK_PARENT {
protected:
  GarbageCollectCallbackI() {}

public:
  virtual int gccb_onGarbageCollect() { return 0; }

private:
  virtual FOURCC syscb_getEventType() { return SysCallback::GC; }
  virtual int syscb_notify(int msg, int param1=0, int param2=0) {
    switch (msg) {
      case GarbageCollectCallback::GARBAGECOLLECT:
        return gccb_onGarbageCollect();
    }
    return 0;
  }
};

#endif
