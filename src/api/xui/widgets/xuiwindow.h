#ifndef XUIWINDOW_H
#define XUIWINDOW_H

#include <api/service/svcs/svc_xuiobject.h>
#include "xuiobject.h"
#include <api/xui/xuiwnd.h>
#include <wnd/basewnd.h>
#include "sourcesink/XUIWindowEventSource.h"
#include "js/JSXUIWindow.h"

#define XUIWindow_PARENT XUIObject

class XUIWindow : public XUIWindow_PARENT, 
                  public XUIWindowEventSource,
                  public JSXUIWindow {
public:
  XUIWindow() { m_flex_orientation = FLEXBOX_ORIENTATION_VERTICAL; }
  virtual void onAttributeChanged(const char *attributeName, const char *attributeValue);
};

MAKE_XUISERVICE_CLASS(XUIWindowSvc, BaseWnd, XUIWindow, "window");

#endif
