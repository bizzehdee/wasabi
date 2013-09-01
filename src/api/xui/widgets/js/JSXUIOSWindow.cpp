// ----------------------------------------------------------------------------
// Generated by JSScriptable Factory - DO NOT EDIT
// 
// File        : ../js/JSXUIOSWindow.cpp
// Class       : XUIOSWindow
// class layer : Javascript Dispatcher
// ----------------------------------------------------------------------------

#include "precomp.h"
#include "JSXUIOSWindow.h"
#include "..\xuioswindow.h"


#pragma warning(push)
#pragma warning(disable: 4800)


JSXUIOSWindow::JSXUIOSWindow() {
  XUIOSWindow *pthis = (static_cast<XUIOSWindow *>(this));
  pthis->js_addInterface(JS_XUIOSWindow_GUID, pthis);
  pthis->js_addClassDescriptor(&JSXUIOSWindow::classDescriptor);
  pthis->js_addFunctionTable(JSXUIOSWindow::functionTable);
  jsSink = new JSXUIOSWindowEventSink(this);
  jsSink->eventSink_registerTo(pthis);
}

JSXUIOSWindow::~JSXUIOSWindow() {
  delete jsSink;
}

JSClass JSXUIOSWindow::classDescriptor = {
  "XUIOSWindow", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
  JSCLASS_NO_OPTIONAL_MEMBERS
};

JSFunctionSpec JSXUIOSWindow::functionTable[] = {
  {0,0,0,0,0},
};

// ----------------------------------------------------------------------------

void JSXUIOSWindowEventSink::eventSink_onUserClose(XUIOSWindowEventSource *source) {
  PtrList<JSEventListener> *listeners = (static_cast<XUIOSWindow *>(m_jsClass))->js_getEventListeners("onuserclose");
  if (!listeners) return;
  jsval rv;
  foreach(*listeners)
    JSEventListener *listener = (*listeners).getfor();
    JS_CallFunctionValue(listener->m_context, listener->m_object, listener->m_function, 0, NULL, &rv);
  endfor
}


