#ifndef _SVCCB_H
#define _SVCCB_H

#include <api/syscb/callbacks/syscbi.h>

namespace SvcCallback {
  enum {
    ONREGISTER=10,
    ONDEREGISTER=20,
  };
};

#define SVCCALLBACK_PARENT SysCallbackI
class SvcCallbackI : public SVCCALLBACK_PARENT {
protected:
  SvcCallbackI() { }

public:
  virtual void svccb_onSvcRegister(GUID type, waServiceFactory *svc) { }
  virtual void svccb_onSvcDeregister(GUID type, waServiceFactory *svc) { }

private:
  virtual FOURCC syscb_getEventType() { return SysCallback::SERVICE; }

  virtual int syscb_notify(int msg, long param1, long param2) {
    switch (msg) {
      case SvcCallback::ONREGISTER:
      {
        GUID g = *reinterpret_cast<GUID *>(param1);
        svccb_onSvcRegister(g, reinterpret_cast<waServiceFactory*>(param2));
      }
      break;
      case SvcCallback::ONDEREGISTER:
      {
        GUID g = *reinterpret_cast<GUID *>(param1);
        svccb_onSvcRegister(g, reinterpret_cast<waServiceFactory*>(param2));
      }
      break;
      default: return 0;
    }
    return 1;
  }
};

#endif
