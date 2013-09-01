#ifndef XUIATTRIBFORWARD_H
#define XUIATTRIBFORWARD_H

#include "widgets/sourcesink/XUIObjectEventSink.h"

class XUIAttribForwarder : public XUIObjectEventSink {
public:
  XUIAttribForwarder(XUIObject *source, const char *sourceattrib, XUIObject *dest, const char *destattrib);
  virtual ~XUIAttribForwarder();
  virtual void eventSink_onAttributeChanged(XUIObjectEventSource *source, const char *attributeName, const char *attributeValue);

private:
  XUIObject *m_source;
  XUIObject *m_dest;
  String m_sourceattr;
  String m_destattr;
};

#endif
