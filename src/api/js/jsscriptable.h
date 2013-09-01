#ifndef JSSCRIPTABLE_H
#define JSSCRIPTABLE_H

#include <bfc/map.h>
#include <bfc/tlist.h>
#include "jsapi.h"
#include "jseventlistener.h"

class JSScriptable {
protected:
  JSScriptable();
  virtual ~JSScriptable();

public:
  // throw a custom exception message
  void js_throwException(const char *message);
  // param 'n' should be of type 'type' while calling function foo on class var
  void js_throwParamTypeException(int n, const char *type, const char *function, const char *classname);
  // invalid object for parameter n while trying to call function foo on class bar
  void js_throwNullParamException(int n, const char *function, const char *classname);
  // this one is static due to the nature of the exception being thrown
  static void js_throwNullCallException(const char *classname, const char *methodname);

  // register a class descriptor table override
  void js_addClassDescriptor(JSClass *classDescriptor);
  // register function tables
  void js_addFunctionTable(JSFunctionSpec *functionTable);
  // return the JSObject associated with this c++ object, creates one if needed
  JSObject *js_getJSObject(JSContext *context);
  // returns the list of event listeners on a particular event
  PtrList<JSEventListener> *js_getEventListeners(const char *eventname);
  // record current context
  void js_setCurrentContext(JSContext *context);
  // returns current context
  JSContext *js_getCurrentContext();
  // cleanup
  void js_onDestroy();
  // registers an interface guid/pointer pair, so we can do type checking
  void js_addInterface(GUID guid, void *iface);
  // get the interface pointer associated with an interface guid, if if exists
  void *js_getInterface(GUID guid);

  // js calls, to add, remove, and trigger events
  static JSBool fromjs_addEventListener(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
  static JSBool fromjs_removeEventListener(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
  static JSBool fromjs_triggerEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
  static JSBool fromjs_triggerEventArgs(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

  // js base class function table, for eventlistener functions
  static JSFunctionSpec baseTable[];


private:
  void js_addEventListener(const char *eventname, JSContext *context, jsval function, JSObject *object);
  void js_removeEventListener(const char *eventname, JSContext *context, jsval function, JSObject *object);
  void clearEventListeners();

  JSClass *m_classDescriptor;
  PtrList<JSClass> m_classDescriptorHierarchy;
  PtrList<JSFunctionSpec> m_functionTables;
  JSObject *m_jsObject;
  JSContext *m_originalContext;
  JSContext *m_currentContext;
  Map<String, PtrList<JSEventListener>* > m_eventMap;
  Map<GUID, void *, GUIDSort> m_interfaces;
  bool m_isGlobalObject;
  static JSScriptable *s_globalObject;

 friend class JSSandbox;
  void setGlobalObject();
};

// a dynamic vector of jsvals, each of which is rooted to a context. 
// automatically unroots the jsvals when the vector is destroyed, or its elements removed
class JSRootedVector : public TList<jsval> {
public:
  JSRootedVector(JSContext *cx) {
    m_context = cx;
  }
  virtual ~JSRootedVector() {
    unrootAll();
  }
  void unrootAll() {
    for (int i=0;i<getNumItems();i++) {
      JS_RemoveRoot(m_context, enumRef(i));
    }
  }
  jsval *newVal() {
    jsval v = JSVAL_VOID;
    jsval *p = addItem(v);
    JS_AddRoot(m_context, p);
    return p;
  }
  int delItem(const jsval &item) {
    jsval *last = enumRef(getNumItems()-1);
    int n = TList<jsval>::delItem(item);
    while (n--) {
      JS_RemoveRoot(m_context, last--);
    }
  }
  int delByPos(int pos) {
    jsval *last = enumRef(getNumItems()-1);
    if (TList<jsval>::delByPos(pos)) {
      JS_RemoveRoot(m_context, last);
    }
  }
  void removeAll() {
    unrootAll();
    TList<jsval>::removeAll();
  }
private:
  void freeAll() {}
  JSContext *m_context;
};


#endif
