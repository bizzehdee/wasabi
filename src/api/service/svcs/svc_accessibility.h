#ifndef _SVC_ACCESSIBILITY_H
#define _SVC_ACCESSIBILITY_H

#include <bfc/dispatch.h>
#include <api/service/services.h>

class Accessible;
class RootWnd;

class NOVTABLE svc_accessibility : public Dispatchable {
public:
  static GUID getServiceType() { return WaSvc::ACCESSIBILITY; }

  Accessible *createAccessibleObject(RootWnd *w);

  enum {
    SVC_ACCESSIBILITY_CREATEACCESSIBLEOBJECT=10,
  };
};

inline Accessible *svc_accessibility::createAccessibleObject(RootWnd *w) {
  return _call(SVC_ACCESSIBILITY_CREATEACCESSIBLEOBJECT, (Accessible *)NULL, w);
}

class NOVTABLE svc_accessibilityI: public svc_accessibility {
  public:
    virtual Accessible *createAccessibleObject(RootWnd *w)=0;

  protected:
    RECVS_DISPATCH;
};

#include <api/service/servicei.h>
template <class T>
class AccessibilityCreatorSingle : public waServiceFactoryTSingle<svc_accessibility, T> {
public:
  svc_accessibility *getHandler() {
    return getSingleService();
  }
};


#endif
