// ----------------------------------------------------------------------------
// Generated by SourceSink Factory - DO NOT EDIT
// 
// File        : ../sourcesink/XUIObjectEventSink.h
// Class       : XUIObject
// class layer : Event Sink for XUIObject
// ----------------------------------------------------------------------------

#ifndef XUIOBJECTEVENTSINK_H
#define XUIOBJECTEVENTSINK_H

#include <bfc/eventsink.h>
#include "XUIObjectEventSource.h"

class XUIDndSession;
class XUIObject;


// ----------------------------------------------------------------------------

class XUIObjectEventSink {
  protected:
    XUIObjectEventSink() {}
    virtual ~XUIObjectEventSink() {
      while (m_sources.getNumItems()>0) {
        eventSink_unregisterTo(m_sources.enumItem(0));
      }
    }
  
  public:
    void eventSink_registerTo(XUIObjectEventSource *eventSource) {
      eventSource->registerXUIObjectEventSink(this);
      m_sources.addItem(eventSource);
    }
    void eventSink_unregisterTo(XUIObjectEventSource *eventSource) {
      eventSource->unregisterXUIObjectEventSink(this);
      m_sources.removeItem(eventSource);
    }
    virtual void eventSink_onAttributeChanged(XUIObjectEventSource *source, const char *attributeName, const char *attributeValue) {} 
    virtual void eventSink_onBeginDrag(XUIObjectEventSource *source, XUIDndSession *s) {} 
    virtual void eventSink_onEndDrag(XUIObjectEventSource *source, XUIDndSession *s, bool success) {} 
    virtual void eventSink_onDragEnter(XUIObjectEventSource *source, XUIDndSession *dndSession) {} 
    virtual void eventSink_onDragLeave(XUIObjectEventSource *source, XUIDndSession *dndSession) {} 
    virtual void eventSink_onDragOver(XUIObjectEventSource *source, XUIDndSession *dndSession, int x, int y) {} 
    virtual void eventSink_onDragDrop(XUIObjectEventSource *source, XUIDndSession *dndSession, int x, int y) {} 
    virtual void eventSink_onInit(XUIObjectEventSource *source) {} 
    virtual void eventSink_onDestroy(XUIObjectEventSource *source) {} 
    virtual void eventSink_onResize(XUIObjectEventSource *source) {} 
    virtual void eventSink_onLeftButtonDown(XUIObjectEventSource *source, int x, int y) {} 
    virtual void eventSink_onLeftButtonUp(XUIObjectEventSource *source, int x, int y) {} 
    virtual void eventSink_onRightButtonDown(XUIObjectEventSource *source, int x, int y) {} 
    virtual void eventSink_onRightButtonUp(XUIObjectEventSource *source, int x, int y) {} 
    virtual void eventSink_onMiddleButtonDown(XUIObjectEventSource *source, int x, int y) {} 
    virtual void eventSink_onMiddleButtonUp(XUIObjectEventSource *source, int x, int y) {} 
    virtual void eventSink_onMouseWheelDown(XUIObjectEventSource *source, int click, int lines) {} 
    virtual void eventSink_onMouseWheelUp(XUIObjectEventSource *source, int click, int lines) {} 
    virtual void eventSink_onMouseMove(XUIObjectEventSource *source, int x, int y) {} 
    virtual void eventSink_onMouseEnter(XUIObjectEventSource *source) {} 
    virtual void eventSink_onMouseLeave(XUIObjectEventSource *source) {} 
    virtual void eventSink_onGetFocus(XUIObjectEventSource *source) {} 
    virtual void eventSink_onLoseFocus(XUIObjectEventSource *source) {} 
    virtual void eventSink_onShow(XUIObjectEventSource *source) {} 
    virtual void eventSink_onHide(XUIObjectEventSource *source) {} 
    virtual void eventSink_onActivate(XUIObjectEventSource *source) {} 
    virtual void eventSink_onDeactivate(XUIObjectEventSource *source) {} 
    virtual void eventSink_onEnable(XUIObjectEventSource *source) {} 
    virtual void eventSink_onDisable(XUIObjectEventSource *source) {} 
    virtual void eventSink_onChar(XUIObjectEventSource *source, unsigned int c) {} 
    virtual void eventSink_onKeyDown(XUIObjectEventSource *source, unsigned int keyCode) {} 
    virtual void eventSink_onKeyUp(XUIObjectEventSource *source, unsigned int keyCode) {} 
    virtual void eventSink_onMinimize(XUIObjectEventSource *source) {} 
    virtual void eventSink_onRestore(XUIObjectEventSource *source) {} 
    virtual void eventSink_onMaximize(XUIObjectEventSource *source) {} 
    virtual void eventSink_onCancelCapture(XUIObjectEventSource *source) {} 
  
  private:
    PtrList<XUIObjectEventSource> m_sources;
};

#endif // XUIOBJECTEVENTSINK_H
