#ifndef OSMDICLIENTWND_H
#define OSMDICLIENTWND_H

#include <wnd/basewnd.h>

class OSMDIParentWnd;

#define OSMDICLIENTWND_PARENT BaseWnd

class OSMDIClientWnd : public BaseWnd {
public:
  OSMDIClientWnd(OSMDIParentWnd *parent);
  virtual ~OSMDIClientWnd();

  virtual int onInit();
  virtual LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int onResize();

  virtual int excludeFromTabList() { return 1; }
  
protected:
  virtual const char *getWindowClass() { return "MDICLIENT"; }
private:
  OSMDIParentWnd *m_parentframe;
};

#endif

