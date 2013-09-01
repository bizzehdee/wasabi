#ifndef JSSANDBOX_H
#define JSSANDBOX_H

#include "jsapi.h"
#include "jsobj.h"
#include "jsglobalobject.h"
#include <api/timer/timerclient.h>

class JSSandbox : public TimerClientDI {
public:
  JSSandbox();
  virtual ~JSSandbox();

  void load(const char *bytes, int len, const char *sourcefile=NULL, int sourcelinenumber=0);
  void execute();

  virtual void onCreateRuntime() {}
  virtual void onDestroyRuntime() {}

  virtual const char *getDefaultScript();

  static JSRuntime *getRuntime() { return s_runtime; }
  static JSContext *getContext() { return s_context; }
  JSObject *getGlobalObject() { return m_global->js_getJSObject(s_context); }

  virtual void timerclient_timerCallback(int id);

private:

  static void errorReporter(JSContext *cx, const char *message, JSErrorReport *report);

  void initRuntime();
  void shutdownRuntime();
  void createGlobalObject();
  void destroyGlobalObject();

  MemBlock<char> m_buffer;
  
  static int s_numJSScripts;
  String m_sourcefile;
  int m_sourceline;

  JSScript *m_compiled_script;

  static JSRuntime *s_runtime;
  static JSContext *s_context;

  JSGlobalObject *m_global;
  String m_defaultScript;
};

#endif // JSSANDBOX_H
