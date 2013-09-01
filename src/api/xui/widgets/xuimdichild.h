#ifndef XUIMDICHILDWND_H
#define XUIMDICHILDWND_H

#include "xuiwindow.h"
#include <wnd/wnds/os/osmdichildwnd.h>
#include <wnd/wnds/os/osmdiparentwnd.h>

#define XUIMDIChild_PARENT XUIWindow

class XUIMDIChild : public XUIMDIChild_PARENT {
public:
  virtual int customTopLevelInit(RootWnd *parentWindow) { 
    OSMDIParentWnd *parent = reinterpret_cast<OSMDIParentWnd *>(parentWindow);
    parent->addMDIChild(reinterpret_cast<OSMDIChildWnd*>(getRootWnd()));
    return 1;
  }
};

MAKE_XUISERVICE_CLASS(XUIOSMDIWindowSvc, OSMDIChildWnd, XUIMDIChild, "mdichildwindow");



#endif
