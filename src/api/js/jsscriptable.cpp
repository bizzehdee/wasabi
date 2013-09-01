#include <precomp.h>
#include "jsscriptable.h"
#include "jssandbox.h"
#include <jsobj.h>

JSScriptable * JSScriptable::s_globalObject = NULL;

JSScriptable::JSScriptable() : m_jsObject(NULL),
                               m_originalContext(NULL),
                               m_currentContext(NULL),
                               m_classDescriptor(NULL),
                               m_isGlobalObject(false) {
}

JSScriptable::~JSScriptable() {
  if (m_jsObject)
    js_onDestroy();
}

void JSScriptable::js_onDestroy() {
  clearEventListeners();
  if (m_jsObject) {
    JS_SetPrivate(m_originalContext, m_jsObject, NULL);
    JS_RemoveRoot(m_originalContext, &m_jsObject);
  }
  m_jsObject = NULL;
}

JSFunctionSpec JSScriptable::baseTable[] = {
  {"addEventListener", JSScriptable::fromjs_addEventListener, 3, 0, 0},
  {"removeEventListener", JSScriptable::fromjs_removeEventListener, 3, 0, 0},
  {"triggerEvent", JSScriptable::fromjs_triggerEvent, 1, 0, 0},
  {"triggerEventArgs", JSScriptable::fromjs_triggerEventArgs, 2, 0, 0},
  {0,0,0,0,0},
};

void JSScriptable::setGlobalObject() {
  m_isGlobalObject = true;
}

JSObject *JSScriptable::js_getJSObject(JSContext *context) {
  if (m_jsObject) return m_jsObject;

  m_originalContext = context;
  
  JSObject *glob = NULL;
  if (!m_isGlobalObject)
    glob = s_globalObject->js_getJSObject(context);

  m_jsObject = JS_NewObject(context, m_classDescriptor, NULL, glob);

  if (m_isGlobalObject) {
    if (!s_globalObject) s_globalObject = this;
    JS_InitStandardClasses(context, m_jsObject);
  }

  JS_AddNamedRoot(context, &m_jsObject, m_classDescriptor->name);
  JS_SetPrivate(context, m_jsObject, this);
  JS_DefineFunctions(context, m_jsObject, baseTable);
  foreach(m_functionTables)
    JS_DefineFunctions(context, m_jsObject, m_functionTables.getfor());
  endfor;
  js_setCurrentContext(context);

  return m_jsObject;
}

void JSScriptable::js_addClassDescriptor(JSClass *classDescriptor) {
  m_classDescriptor = classDescriptor;
  m_classDescriptorHierarchy.addItem(classDescriptor);
}

void JSScriptable::js_addFunctionTable(JSFunctionSpec *functionTable) {
  m_functionTables.addItem(functionTable);
}

void JSScriptable::js_addInterface(GUID guid, void *iface) {
  m_interfaces.addItem(guid, iface);
}

void *JSScriptable::js_getInterface(GUID guid) {
  void *ptr = NULL;
  m_interfaces.getItem(guid, &ptr);
  return ptr;
}

void JSScriptable::js_addEventListener(const char *eventName, JSContext *context, jsval function, JSObject *object) {
  PtrList<JSEventListener> *eventList = NULL;
  m_eventMap.getItem(String(eventName), &eventList);
  if (!eventList) {
    eventList = new PtrList<JSEventListener>;
    m_eventMap.addItem(String(eventName), eventList);
  }
  eventList->addItem(new JSEventListener(context, function, object));
}

void JSScriptable::js_removeEventListener(const char *eventName, JSContext *context, jsval function, JSObject *object) {
  PtrList<JSEventListener> *eventList = NULL;
  m_eventMap.getItem(String(eventName), &eventList);
  if (!eventList) return;
  foreach(eventList)
    JSEventListener *listener = eventList->getfor();
    if (listener->m_context == context &&
        listener->m_function == function &&
        listener->m_object == object) {
      eventList->removeByPos(foreach_index);
      break;
    }
  endfor;
}

void JSScriptable::clearEventListeners() {
  for (int i=0;i<m_eventMap.getNumPairs();i++) {
    PtrList<JSEventListener> m_listeners = m_eventMap.enumItemByPos(i, NULL);
    m_listeners.deleteAll();
  }
  m_eventMap.deleteAll();
}

PtrList<JSEventListener> *JSScriptable::js_getEventListeners(const char *eventName) {
  PtrList<JSEventListener> *eventList = NULL;
  m_eventMap.getItem(String(eventName), &eventList);
  return eventList;
}

void JSScriptable::js_setCurrentContext(JSContext *context) {
  m_currentContext = context;
}

JSContext *JSScriptable::js_getCurrentContext() {
  return m_currentContext;
}

void JSScriptable::js_throwException(const char *message) {
  JS_SetPendingException(m_currentContext, STRING_TO_JSVAL(JS_NewStringCopyZ(m_currentContext, message)));
}

void JSScriptable::js_throwParamTypeException(int n, const char *type, const char *method, const char *classname) {
  js_throwException(StringPrintf("Parameter %d of function '%s' in class '%s' should be of type '%s'", n, method, classname, type));
}

void JSScriptable::js_throwNullParamException(int n, const char *method, const char *classname) {
  js_throwException(StringPrintf("Invalid object for parameter %d while trying to call function '%s' on class '%s'", n, method, classname));
}


void JSScriptable::js_throwNullCallException(const char *classname, const char *methodname) {
  JS_SetPendingException(JSSandbox::getContext(), 
                         STRING_TO_JSVAL(
                          JS_NewStringCopyZ(
                            JSSandbox::getContext(), 
                            StringPrintf("Invalid 'this' object while trying to call function '%s' on class '%s'", methodname, classname)
                          )
                         )
                        );
}

JSBool JSScriptable::fromjs_addEventListener(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
  // const char *eventname
  JSString *p0;
  p0 = JS_ValueToString(cx, argv[0]);
  argv[0] = STRING_TO_JSVAL(p0);
  const char *eventName = JS_GetStringBytes(p0);

  // jsval function
  jsval function = argv[1];

  // JSObject *object
  JSObject *object;
  if (!JS_ValueToObject(cx, argv[2], &object))
    return JS_FALSE;
  argv[2] = OBJECT_TO_JSVAL(object);

  // get instance
  JSScriptable *scriptable = reinterpret_cast<JSScriptable *>(JS_GetPrivate(cx, obj));

  if (!scriptable) {
    js_throwNullCallException("Object", StringPrintf("addEventListener(%s)", eventName));
    return JS_FALSE;
  }

  // call method
  scriptable->js_addEventListener(eventName, cx, function, object);

  // success
  return JS_TRUE;
}

JSBool JSScriptable::fromjs_removeEventListener(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
  // const char *eventname
  JSString *p0;
  p0 = JS_ValueToString(cx, argv[0]);
  argv[0] = STRING_TO_JSVAL(p0);
  const char *eventName = JS_GetStringBytes(p0);

  // jsval function
  jsval function = argv[1];

  // JSObject *object
  JSObject *object;
  if (!JS_ValueToObject(cx, argv[2], &object))
    return JS_FALSE;
  argv[2] = OBJECT_TO_JSVAL(object);

  // get instance
  JSScriptable *scriptable = reinterpret_cast<JSScriptable *>(JS_GetPrivate(cx, obj));

  if (!scriptable) {
    js_throwNullCallException("Object", StringPrintf("removeEventListener(%s)", eventName));
    return JS_FALSE;
  }

  // call method
  scriptable->js_removeEventListener(eventName, cx, function, object);

  // success
  return JS_TRUE;
}

JSBool JSScriptable::fromjs_triggerEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
  // const char *eventname
  JSString *p0;
  p0 = JS_ValueToString(cx, argv[0]);
  argv[0] = STRING_TO_JSVAL(p0);
  const char *eventName = JS_GetStringBytes(p0);

  // get instance
  JSScriptable *scriptable = reinterpret_cast<JSScriptable *>(JS_GetPrivate(cx, obj));

  PtrList<JSEventListener> *listeners = scriptable->js_getEventListeners(eventName);
  if (!listeners) return JS_TRUE;

  // callback to js listener
  jsval rv;
  foreach(listeners)
    JSEventListener *listener = (*listeners).getfor();
    JS_CallFunctionValue(listener->m_context, listener->m_object, listener->m_function, argc-1, argv+1, &rv);
  endfor;

  // success
  return JS_TRUE;
}

JSBool JSScriptable::fromjs_triggerEventArgs(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
  // const char *eventname
  JSString *p0;
  p0 = JS_ValueToString(cx, argv[0]);
  argv[0] = STRING_TO_JSVAL(p0);
  const char *eventName = JS_GetStringBytes(p0);

  // get instance
  JSScriptable *scriptable = reinterpret_cast<JSScriptable *>(JS_GetPrivate(cx, obj));

  PtrList<JSEventListener> *listeners = scriptable->js_getEventListeners(eventName);
  if (!listeners) return JS_TRUE;

  // arguments object
  JSObject *p1;
  if (!JS_ValueToObject(cx, argv[1], &p1))
    return JS_FALSE;
  argv[1] = OBJECT_TO_JSVAL(p1);

  // enumerate properties as jsval and build arguments array
  TList<jsval> newargs;
  JSObject *iter = JS_NewPropertyIterator(cx, p1); // GC'ed
  jsid id;
  while (JS_NextProperty(cx, iter, &id)) {
    if (id == JSVAL_VOID) break;
    jsval val;
    if (!OBJ_GET_PROPERTY(cx, p1, id, &val)) 
      return JS_FALSE;
    newargs.addItem(val);
  }

  // callback to js listener
  jsval rv;
  foreach(listeners)
    JSEventListener *listener = (*listeners).getfor();
    JS_CallFunctionValue(listener->m_context, listener->m_object, listener->m_function, newargs.getNumItems(), newargs.enumRef(0), &rv);
  endfor;

  // success
  return JS_TRUE;
}
