#include <precomp.h>
#include "widgets/xuiobject.h"
#include "xuiattrfw.h"

XUIAttribForwarder::XUIAttribForwarder(XUIObject *source, const char *sourceattrib, XUIObject *dest, const char *destattrib) {
  m_source = source;
  m_dest = dest;
  m_sourceattr = sourceattrib;
  m_destattr = destattrib;
  eventSink_registerTo(source);
  const char *curvalue = source->getAttribute(sourceattrib);
  if (curvalue)
    dest->setAttribute(destattrib, curvalue);
}

XUIAttribForwarder::~XUIAttribForwarder() {
  eventSink_unregisterTo(m_source);
}

void XUIAttribForwarder::eventSink_onAttributeChanged(XUIObjectEventSource *source, const char *attributeName, const char *attributeValue) {
  if (STRCASEEQL(attributeName, m_sourceattr))
    m_dest->setAttribute(m_destattr, attributeValue);
}

