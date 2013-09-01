// ----------------------------------------------------------------------------
// Generated by SourceSink Factory - DO NOT EDIT
// 
// File        : ../sourcesink/XUIDndSessionEventSink.h
// Class       : XUIDndSession
// class layer : Event Sink for XUIDndSession
// ----------------------------------------------------------------------------

#ifndef XUIDNDSESSIONEVENTSINK_H
#define XUIDNDSESSIONEVENTSINK_H

#include <bfc/eventsink.h>
#include "XUIDndSessionEventSource.h"

class XUIObject;


// ----------------------------------------------------------------------------

class XUIDndSessionEventSink {
  protected:
    XUIDndSessionEventSink() {}
    virtual ~XUIDndSessionEventSink() {
      while (m_sources.getNumItems()>0) {
        eventSink_unregisterTo(m_sources.enumItem(0));
      }
    }
  
  public:
    void eventSink_registerTo(XUIDndSessionEventSource *eventSource) {
      eventSource->registerXUIDndSessionEventSink(this);
      m_sources.addItem(eventSource);
    }
    void eventSink_unregisterTo(XUIDndSessionEventSource *eventSource) {
      eventSource->unregisterXUIDndSessionEventSink(this);
      m_sources.removeItem(eventSource);
    }
    virtual void eventSink_onDragEnter(XUIDndSessionEventSource *source, XUIObject *target) {} 
    virtual void eventSink_onDragLeave(XUIDndSessionEventSource *source, XUIObject *target) {} 
    virtual void eventSink_onDragOver(XUIDndSessionEventSource *source, XUIObject *target, int x, int y) {} 
    virtual void eventSink_onDragDrop(XUIDndSessionEventSource *source, XUIObject *target, int x, int y) {} 
  
  private:
    PtrList<XUIDndSessionEventSource> m_sources;
};

#endif // XUIDNDSESSIONEVENTSINK_H
