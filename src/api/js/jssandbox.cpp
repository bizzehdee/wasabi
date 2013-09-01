#include <precomp.h>
#include "jssandbox.h"

#define TIMERID_GC 10

int JSSandbox::s_numJSScripts = 0;
JSRuntime *JSSandbox::s_runtime = NULL;
JSContext *JSSandbox::s_context = NULL;

JSSandbox::JSSandbox() {
  m_global = NULL;
  if (s_numJSScripts++ == 0) 
    initRuntime();
  createGlobalObject();
}

JSSandbox::~JSSandbox() {
  destroyGlobalObject();
  if (--s_numJSScripts == 0)
    shutdownRuntime();
}

void JSSandbox::initRuntime() {
  s_runtime = JS_NewRuntime(8L * 1024L * 1024L);
  if (s_runtime == NULL) {
    DebugString("Failed to create a javascript runtime");
    shutdownRuntime();
    return;
  }
  s_context = JS_NewContext(s_runtime, 8192);
  if (s_context == NULL) {
    DebugString("Failed to create a javascript context");
    shutdownRuntime();
    return;
  }

  JS_SetOptions(s_context, JSOPTION_VAROBJFIX);
  JS_SetErrorReporter(s_context, errorReporter);

  // TODO: Create a new service type so that people can inject global objects here

  onCreateRuntime();

  timerclient_setTimer(TIMERID_GC, 1000);
}

void dumper(const char *name, void *rp, void *data) {
  DebugString("JS ROOT REMAINING: %s", name);
}

void JSSandbox::shutdownRuntime() {
  timerclient_killTimer(TIMERID_GC);

  onDestroyRuntime();
  JSGlobalObject::onDestroyRuntime(s_context);

  if (s_context) {
    JS_DestroyContext(s_context);
    s_context = NULL;
  }

  #ifdef DEBUG
  void *v;
  JS_DumpNamedRoots(s_runtime, dumper, &v);
  #endif

  if (s_runtime) {
    JS_DestroyRuntime(s_runtime);
    s_runtime = NULL;
  }

  JS_ShutDown();
}

void JSSandbox::timerclient_timerCallback(int id) {
  switch (id) {
    case TIMERID_GC:
      JS_MaybeGC(s_context);
      break;
  }
}

void JSSandbox::load(const char *bytes, int len, const char *sourcefile, int sourcelinenumber) {
  m_sourcefile = sourcefile;
  m_sourceline = sourcelinenumber;
  m_buffer.append(bytes, len);
  m_compiled_script = JS_CompileScript(s_context, getGlobalObject(), m_buffer.getMemory(), m_buffer.getSize(), m_sourcefile, m_sourceline);
}

void JSSandbox::execute() {
  if (m_compiled_script) {
    jsval ret = JSVAL_VOID;
    JS_ExecuteScript(s_context, getGlobalObject(), m_compiled_script, &ret);
  }
}

const char *JSSandbox::getDefaultScript() {
  m_defaultScript = "";

  m_defaultScript += m_global->getSupportScript();
  // ...

  return m_defaultScript;
}

void JSSandbox::errorReporter(JSContext *cx, const char *message, JSErrorReport *report) {
  String msg;
  msg += StringPrintf("JS Error %08x", report->errorNumber);
  if (report->filename) 
    msg += StringPrintf(" in %s(%d)", report->filename, report->lineno);
  if (message)
    msg += StringPrintf(" [%s]", message);
  if (report->linebuf)
    msg += StringPrintf(" : %s[%d]", report->linebuf, report->tokenptr-report->linebuf);
  DebugString("%s", msg);
}

void JSSandbox::createGlobalObject() {
  if (m_global) return;
  m_global = new JSGlobalObject();
  m_global->setGlobalObject();
  if (!m_global->js_getJSObject(s_context)) {
    DebugString("Failed to create a global js object");
    return;
  }

  m_global->onRegisterGlobal(getContext());

  String defaultScript = getDefaultScript();

  jsval rv;
  JS_EvaluateScript(s_context, getGlobalObject(), defaultScript, defaultScript.len(), "StartupScript", 0, &rv);
}

void JSSandbox::destroyGlobalObject() {
  if (!m_global) return;
  delete m_global;
  m_global = NULL;
}

