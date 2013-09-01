#ifndef JSEVENTLISTENER_H
#define JSEVENTLISTENER_H

#include "jsapi.h"

class JSEventListener {
public:
  JSEventListener(JSContext *context, jsval function, JSObject *object) {
    m_context = context;
    m_function = function;
    m_object = object;
    JS_AddRoot(context, &m_function);
    JS_AddRoot(context, &m_object);
  }

  virtual ~JSEventListener() {
    JS_RemoveRoot(m_context, &m_function);
    JS_RemoveRoot(m_context, &m_object);
  }

  JSContext *m_context;
  JSObject *m_object;
  jsval m_function;
};

#endif
