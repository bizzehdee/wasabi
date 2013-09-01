#ifndef XUILABEL_H
#define XUILABEL_H

#include <api/service/svcs/svc_xuiobject.h>
#include "xuiobject.h"
#include <api/xui/xuiwnd.h>
#include <wnd/wnds/statictextwnd.h>
#include "sourcesink/XUILabelEventSource.h"
#include "js/JSXUILabel.h"

#define XUILabel_PARENT XUIObject

class XUILabel : public XUILabel_PARENT, 
                 public XUILabelEventSource,
                 public JSXUILabel {
public:
  virtual void onAttributeChanged(const char *attributeName, const char *attributeValue);
  ACCESS_SOURCE_WND(m_statictext, StaticTextWnd, XUILabel_PARENT);
};

MAKE_XUISERVICE_CLASS(XUILabelSvc, StaticTextWnd, XUILabel, "statictext");

#endif
