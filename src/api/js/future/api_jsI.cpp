//<?#include "<class data="implementationheader"/>"
#include "api_jsI.h"
//?>
api_js *jsApi = NULL;

api_jsI::api_jsI() {
  // Create the JS Runtime when we create the api
  rt = JS_NewRuntime(0x400000L);
}

api_jsI::~api_jsI() {
  //delete all contexts
  foreach(ctxs) {
    JS_DestroyContext(ctxs.getfor());
  } endfor;

  //kill the runtime
  JS_DestroyRuntime(rt);
  JS_ShutDown();
}

JSContext *api_jsI::createNewContext() {
  return ctxs.addItem(JS_NewContext(rt, 8192));
}

JSContext *api_jsI::enumContext(int n) {
  return ctxs.enumItem(n);
}

int api_jsI::deleteContext(int n) {
  JS_DestroyContext(ctxs.enumItem(n));
  ctxs.removeByPos(n);
  return 0;
}

int api_jsI::deleteContext(JSContext *ctx) {
  JS_DestroyContext(ctx);
  ctxs.removeItem(ctx);
  return 0;
}

JSContext *api_jsI::getMainContext() {
  return ctxs.enumItem(0);
}

void api_jsI::collectGarbage(JSContext *ctx) {
  JS_GC(ctx);
}

JSObject *api_jsI::createNewObject(JSContext *cx, JSClass *cls, JSObject *parent) {
  if(!STRICMP(cls->name, "global")) {
    globalObj = JS_NewObject(cx, cls, 0, parent);
    JS_SetGlobalObject(cx, globalObj);
    JS_InitStandardClasses(cx, globalObj);
    return globalObj;
  } else {
    if(!parent) parent = globalObj;
    JSObject *newObj = JS_NewObject(cx, cls, 0, parent);
    objs.addItem(newObj);
    return newObj;
  }
  return (JSObject *)0;
}

JSObject *api_jsI::enumObject(int n) {
   return objs.enumItem(n);
}

JSObject *api_jsI::getGlobalObject() {
  return globalObj;
}

void api_jsI::assignFunctions(JSContext *cx, JSObject *classobj, JSFunctionSpec *spec) {
  JS_DefineFunctions(cx, classobj, spec);
}

JSScript *api_jsI::compileScript(JSContext *cx, JSObject *obj, const char *filename) {
  return JS_CompileFile(cx, obj, filename);
}

JSScript *api_jsI::compileScript(JSContext *cx, JSObject *obj, const char *bytes, long len, const char *filename) {
  return JS_CompileScript(cx, obj, bytes, len, filename, 0);
}

JSObject *api_jsI::createScriptObject(JSContext *cx, JSScript *script) {
  return JS_NewScriptObject(cx, script);
}

void api_jsI::createNamedRoot(JSContext *cx, JSObject *scriptobject, const char *name) {
  JS_AddNamedRoot(cx, scriptobject, name);
}

void api_jsI::deleteNamedRoot(JSContext *cx, JSObject *scriptobject) {
  JS_RemoveRoot(cx, scriptobject);
}

bool api_jsI::executeScript(JSContext *cx, JSScript *script, jsval *rval) {
   return !!JS_ExecuteScript(cx, globalObj, script, rval);
}

bool api_jsI::executeFunction(JSContext *cx, JSObject *script, const char *name, int argn, jsval *args, jsval *rval) {
  return !!JS_CallFunctionName(cx, script, name, argn, args, rval);
}

bool api_jsI::evalScript(JSContext *cx, JSObject *obj, const char *bytes, long len, jsval *retval, const char *filename) {
  return !!JS_EvaluateScript(cx, obj, bytes, len, filename, 0, retval);
}

int api_jsI::jsvalToInt(JSContext *cx, jsval *v) {
   long ret = 0;
   JS_ValueToECMAInt32(cx, *v, &ret);
   return (int)ret;
}

double api_jsI::jsvalToDouble(JSContext *cx, jsval *v) {
  double ret = 0;
  JS_ValueToNumber(cx, *v, &ret);
  return ret;
}

const char *api_jsI::jsvalToString(JSContext *cx, jsval *v) {
  const char *ret = 0;
  JSString *js_str = JS_ValueToString(cx, *v);
  ret = JS_GetStringBytes(js_str);
  return ret;
}

bool api_jsI::jsvalToBool(JSContext *cx, jsval *v) {
  int ret = 0;
  JS_ValueToBoolean(cx, *v, &ret);
  return !!ret;
}

void api_jsI::jsvalToObject(JSContext *cx, jsval *v, JSObject **obj) {
  JS_ValueToObject(cx, *v, obj);
}

jsval api_jsI::intToJSval(JSContext *cx, int v) {
  return INT_TO_JSVAL(v);
}

jsval api_jsI::doubleToJSval(JSContext *cx, double v) {
  return DOUBLE_TO_JSVAL(v);
}

jsval api_jsI::stringToJSval(JSContext *cx, const char *v) {
  JSString *m_local_string = JS_NewString(cx, (char *)v, strlen(v));
  return STRING_TO_JSVAL(m_local_string);
}

jsval api_jsI::boolToJSval(JSContext *cx, bool v) {
  return BOOLEAN_TO_JSVAL(v);
}

jsval api_jsI::objectToJSval(JSContext *cx, JSObject *obj) {
   return OBJECT_TO_JSVAL(obj);
}

