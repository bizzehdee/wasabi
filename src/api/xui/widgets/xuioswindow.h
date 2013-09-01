#ifndef XUIOSWINDOW_H
#define XUIOSWINDOW_H

#include "xuiwindow.h"
#include <wnd/wnds/os/osframewnd.h>
#include "sourcesink/XUIOSWindowEventSource.h"
#include "js/JSXUIOSWindow.h"

#define XUIOSWindow_PARENT XUIWindow

class XUIOSWindow : public XUIOSWindow_PARENT, 
                    public XUIOSWindowEventSource,
                    public JSXUIOSWindow {
public:
  SCRIPT EVENT void onUserClose() { eventSource_onUserClose(); }
};


class XUIOSWindowSvc: public XUIWnd<OSFrameWnd, XUIOSWindow> {
public:
  virtual void onUserClose() {
    static_cast<XUIOSWindow *>(getXUIObject())->onUserClose();
  }
  
  static const char *getXmlTag() { return "oswindow"; }
};


#endif
