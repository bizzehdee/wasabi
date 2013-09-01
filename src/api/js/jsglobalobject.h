#ifndef JSGLOBALOBJECT_H
#define JSGLOBALOBJECT_H

#include "sourcesink/JSGlobalObjectEventSource.h"
#include "js/JSJSGlobalObject.h"
#include <api/timer/timerclient.h>

class JSGlobalProperty {
public:
  JSGlobalProperty(JSScriptable *object, const char *name) : m_object(object),
                                                             m_name(name) {}
  JSScriptable *getObject() { return m_object; }
  const char *getName() { return m_name; }

private:
  JSScriptable *m_object;
  String m_name;
};

class JSGlobalObject : public JSScriptable,
                       public JSGlobalObjectEventSource,
                       public JSJSGlobalObject,
                       public TimerClientDI {
public:
  JSGlobalObject() {}
  virtual ~JSGlobalObject();

  const char *getSupportScript();

  // this should be called at startup, with objects that do no expire -- todo: use something better !
  static void registerGlobalProperty(const char *name, JSScriptable *object);
  virtual void onRegisterGlobal(JSContext *cx);
  static void onDestroyRuntime(JSContext *cx);

  SCRIPT void printDebug(const char *str);
  SCRIPT void alert(const char *str);
  SCRIPT JSTimer *createTimer();
  SCRIPT void deleteTimer(JSTimer *timer);
  SCRIPT bool isTimerValid(JSTimer *timer);
  SCRIPT void shutdown();

  virtual int timerclient_onDeferredCallback(int p1, int p2);

  String m_supportScript;
  PtrListInsertSortedByPtrVal<JSTimer> m_timers;

  static PtrList<JSGlobalProperty> s_global_props;
};

#endif
