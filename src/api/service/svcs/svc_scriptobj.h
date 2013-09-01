#ifndef _SVC_SCRIPTOBJECT_H
#define _SVC_SCRIPTOBJECT_H

#include <bfc/dispatch.h>
#include <api/service/services.h>

class ScriptObjectController;

class svc_scriptObject : public Dispatchable {
public:
  static GUID getServiceType() { return WaSvc::SCRIPTOBJECT; }
  ScriptObjectController *getController(int n);
  void onRegisterClasses(ScriptObjectController *rootController);

  enum {
    GETCONTROLLER=10,
    ONREGISTER=20,
  };
};

inline ScriptObjectController *svc_scriptObject::getController(int n) {
  return _call(GETCONTROLLER, (ScriptObjectController *)0, n);
}

inline void svc_scriptObject::onRegisterClasses(ScriptObjectController *rootController) {
  _voidcall(ONREGISTER, rootController);
}

// derive from this one
class svc_scriptObjectI : public svc_scriptObject {
public:
  virtual ScriptObjectController *getController(int n)=0;
  virtual void onRegisterClasses(ScriptObjectController *rootController) {};

protected:
  RECVS_DISPATCH;
};

template <class T>
class ScriptObjectControllerCreator : public svc_scriptObjectI {
public:
  static const char *getServiceName() { return "ScriptObjectControllerCreator"; }

  ScriptObjectControllerCreator() {
    single_controller = new T;
  }
  virtual ~ScriptObjectControllerCreator() {
    delete single_controller;
  }
  virtual ScriptObjectController *getController(int n) {
    if (n == 0) return single_controller;
    return NULL;
  }

private:
  T *single_controller;
};

#include <api/service/servicei.h>
template <class T>
class ScriptObjectCreator : public waServiceFactoryTSingle<svc_scriptObject, T> {};

#include <api/service/svc_enum.h>

class ExternalScriptObjectEnum : public SvcEnumT<svc_scriptObject> {
public:
  ExternalScriptObjectEnum() { }

protected:
  virtual int testService(svc_scriptObject*svc) {
    return (svc->getController(0) != NULL);
  }
};

#endif
