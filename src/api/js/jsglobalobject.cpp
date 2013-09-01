#include <precomp.h>
#include "jstimer.h"
#include "jsglobalobject.h"

#pragma warning(disable: 4800)

enum {
  DC_DELETETIMER = 1,
};

const char *JSGlobalObject::getSupportScript() {
  String str;

  m_supportScript += "function print() {\n"\
                     "  var str = \"\";" \
                     "  for ( var i = 0 ; i < print.arguments.length ; i++ )\n"\
                     "    str += print.arguments[i];\n"\
                     "  printDebug( str );\n"\
                     "}\n"\
                     "\n";
  
  m_supportScript += "function setTimeout(fn, delay, param) {\n"\
                     "  var timeout = createTimer();\n"\
                     "  timeout.setDelay(delay);\n"\
                     "  timeout.addEventListener(\"ontimer\",\n"\
                     "    function() {\n"\
                     "      clearTimeout(timeout);\n"\
                     "      fn(param);\n"\
                     "    }\n"\
                     "  );\n"\
                     "  timeout.start();\n"\
                     "  return timeout;"\
                     "}\n"\
                     "\n";

  m_supportScript += "function clearTimeout(timeout) {\n"\
                     "  if (!isTimerValid(timeout))\n"\
                     "    throw new Error(\"Invalid timeout in clearTimeout\");\n"\
                     "  else\n"\
                     "    deleteTimer(timeout);\n"\
                     "}\n"\
                     "\n";

  m_supportScript += "function setInterval(fn, delay, param) {\n"\
                     "  var interval = createTimer();\n"\
                     "  interval.setDelay(delay);\n"\
                     "  interval.addEventListener(\"ontimer\",\n"\
                     "    function() {\n"\
                     "      fn(param);\n"\
                     "    }\n"\
                     "  );\n"\
                     "  interval.start();\n"\
                     "  return interval;"\
                     "}\n"\
                     "\n";

  m_supportScript += "function clearInterval(interval) {\n"\
                     "  if (!isTimerValid(interval))\n"\
                     "    throw new Error(\"Invalid interval in clearInterval\");\n"\
                     "  else\n"\
                     "    deleteTimer(interval);\n"\
                     "}\n"\
                     "\n";

  m_supportScript += "function forwardEvent(from, event, to) {\n"\
                     "  from.addEventListener(event, \n"\
                     "    function() {\n"\
                     "      var a = {};\n"\
                     "      for (var i=0;i<arguments.length;i++) { a[i] = arguments[i]; }\n"\
                     "      to.triggerEventArgs(event, a);\n"\
                     "    }\n"\
                     "  );\n"\
                     "}\n"\
                     "\n";

  m_supportScript += "function forwardMethod(objectToCall, method, forwardMethodObject) {\n"\
                     "  forwardMethodObject[method] = function() {\n"\
                     "    return objectToCall[method].apply(objectToCall, arguments);\n"\
                     "  };\n"\
                     "}\n"\
                     "\n";

  // ...

  return m_supportScript;
}

JSGlobalObject::~JSGlobalObject() {
  m_timers.deleteAll();
}

void JSGlobalObject::printDebug(const char *str) {
  DebugString("%s", str);
}

void JSGlobalObject::alert(const char *str) {
  MessageBox(NULL, str, "Javascript alert", 0);
}

JSTimer *JSGlobalObject::createTimer() {
  JSTimer *timer = new JSTimer();
  m_timers.addItem(timer);
  return timer;
}

void JSGlobalObject::deleteTimer(JSTimer *timer) {
  // verify that this is a valid timer
  if (!m_timers.haveItem(timer)) {
    js_throwException("Invalid timer in destroyTimer");
    return;
  }
  // delay deletion because we may be inside the event for the timer which we want to delete
  // but stop the timer immediately
  timer->stop();
  timerclient_postDeferredCallback(DC_DELETETIMER, reinterpret_cast<int>(timer));
}

int JSGlobalObject::timerclient_onDeferredCallback(int p1, int p2) {
  // actually delete a timer object. fail silently if the timer is no longer valid
  if (p1 == DC_DELETETIMER) {
    JSTimer *timer = reinterpret_cast<JSTimer*>(p2);
    if (m_timers.haveItem(timer)) {
      m_timers.removeItem(timer);
      delete timer;
    }
  }
  return 1;
}


bool JSGlobalObject::isTimerValid(JSTimer *timer) {
  return static_cast<bool>(m_timers.haveItem(timer));
}

void JSGlobalObject::shutdown() {
  WASABI_API_APP->main_shutdown();
}

void JSGlobalObject::registerGlobalProperty(const char *name, JSScriptable *object) {
  s_global_props.addItem(new JSGlobalProperty(object, name));
}

void JSGlobalObject::onRegisterGlobal(JSContext *cx) {
  foreach(s_global_props)
    jsval obj = OBJECT_TO_JSVAL(s_global_props.getfor()->getObject()->js_getJSObject(cx));
    JS_SetProperty(cx, js_getJSObject(cx), s_global_props.getfor()->getName(), &obj);
  endfor;
}

PtrList<JSGlobalProperty> JSGlobalObject::s_global_props;

void JSGlobalObject::onDestroyRuntime(JSContext *cx) {
  foreach(s_global_props)
    JSScriptable *obj = s_global_props.getfor()->getObject();
    obj->js_onDestroy();
  endfor;
}