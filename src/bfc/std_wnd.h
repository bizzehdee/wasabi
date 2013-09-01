#ifndef _STD_WIN_H
#define _STD_WIN_H

#include <bfc/platform/platform.h>
#include <bfc/std.h>
#include <bfc/string/string.h>

#ifdef WASABI_PLATFORM_WIN32
#include <ole2.h>
#endif

#ifndef SYSRGN
#define SYSRGN 4
#endif

#ifdef __cplusplus

namespace StdWnd {
#ifdef WIN32
  OSWINDOWHANDLE createWnd(int x, int y, int w, int h, int nochild, int acceptdrops, OSWINDOWHANDLE parent, OSMODULEHANDLE module, const char *classname, WNDPROC wndproc, unsigned long userdata=0, int want_dclick=FALSE, int alpha=255);
#elif defined(XWINDOW)
  OSWINDOWHANDLE createWnd(int x, int y, int w, int h, int nochild, int acceptdrops, OSWINDOWHANDLE parent, OSMODULEHANDLE module, const char *classname, unsigned long userdata);
#endif
// other OSes should put a createWnd() for their specific needs here
// and port BaseWnd to it please :)

  void destroyWnd(OSWINDOWHANDLE wnd);

  int isValidWnd(OSWINDOWHANDLE wnd);

  void setWndPos(OSWINDOWHANDLE wnd, OSWINDOWHANDLE zorder,
    int x, int y, int w, int h,
    int nozorder, int noactive, int nocopybits, int nomove, int noresize);
  void getWndPos(OSWINDOWHANDLE wnd, int *x=NULL, int *y=NULL);
  void bringToFront(OSWINDOWHANDLE wnd);
  void sendToBack(OSWINDOWHANDLE wnd);
  int isWndVisible(OSWINDOWHANDLE wnd);
  void showWnd(OSWINDOWHANDLE wnd, int noactivate=FALSE);
  void hideWnd(OSWINDOWHANDLE wnd);
  void minimizeWnd(OSWINDOWHANDLE wnd);
  void maximizeWnd(OSWINDOWHANDLE wnd);
  void restoreWnd(OSWINDOWHANDLE wnd);
  int isPopup(OSWINDOWHANDLE wnd);
  void setEnabled(OSWINDOWHANDLE wnd, int enabled);
  void setFocus(OSWINDOWHANDLE wnd);
  OSWINDOWHANDLE getFocus();
  void setTopmost(OSWINDOWHANDLE, int topmost);
  
  void setParent(OSWINDOWHANDLE child, OSWINDOWHANDLE newparent);
  OSWINDOWHANDLE getParent(OSWINDOWHANDLE wnd);
//  void reparent(OSWINDOWHANDLE child, OSWINDOWHANDLE newparent);
  OSWINDOWHANDLE getTopmostChild(OSWINDOWHANDLE wnd);

  DWORD getOwningProcess(OSWINDOWHANDLE wnd);

  void invalidateRect(OSWINDOWHANDLE wnd, RECT *r=NULL);
  void invalidateRegion(OSWINDOWHANDLE wnd, OSREGIONHANDLE region);
  void validateRect(OSWINDOWHANDLE wnd, RECT *r=NULL);
  void validateRegion(OSWINDOWHANDLE wnd, OSREGIONHANDLE region);
  void update(OSWINDOWHANDLE wnd);
  int getUpdateRect(OSWINDOWHANDLE wnd, RECT *r);
  void getUpdateRegion(OSWINDOWHANDLE wnd, OSREGIONHANDLE region);
  int haveGetRandomRgn();	// only true on win32
  void getRandomRgn(HDC hdc, OSREGIONHANDLE region);	// sorry, HDC, win32only

  void setWndRegion(OSWINDOWHANDLE wnd, OSREGIONHANDLE region, int redraw=FALSE);
  
  HDC getDC(OSWINDOWHANDLE wnd);
  int releaseDC(OSWINDOWHANDLE wnd, HDC dc);

  int isDesktopAlphaAvailable();
  int isTransparencyAvailable();
  void setLayeredWnd(OSWINDOWHANDLE wnd, int layered);
  int isLayeredWnd(OSWINDOWHANDLE wnd);
  void setLayeredAlpha(OSWINDOWHANDLE wnd, int amount);	// 0..255
  void updateLayeredWnd(OSWINDOWHANDLE wnd, int x, int y, int w, int h, HDC surfdc, int alpha);

  int getClientRect(OSWINDOWHANDLE wnd, RECT *r);
  int getWindowRect(OSWINDOWHANDLE wnd, RECT *r);
  void clientToScreen(OSWINDOWHANDLE wnd, int *x, int *y);
  void clientToScreen(OSWINDOWHANDLE wnd, RECT *r);
  void screenToClient(OSWINDOWHANDLE wnd, int *x, int *y);
  void screenToClient(OSWINDOWHANDLE wnd, RECT *r);
  OSWINDOWHANDLE windowFromPoint(POINT p);

  void setCapture(OSWINDOWHANDLE wnd);
  void releaseCapture();
  OSWINDOWHANDLE getCapture();

  void revokeDragNDrop(OSWINDOWHANDLE wnd);

  void setWndName(OSWINDOWHANDLE wnd, const char *name);
  void getWndName(OSWINDOWHANDLE wnd, char *name, int maxlen); // maxlen ignored for XWINDOW
  void setIcon(OSWINDOWHANDLE wnd, OSICONHANDLE icon, int large=FALSE);

  OSWINDOWHANDLE getActiveWindow();
  void setActiveWindow(OSWINDOWHANDLE wnd);
  void clipOSChildren(OSWINDOWHANDLE wnd, OSREGIONHANDLE reg);

  void getViewport(RECT *r, POINT *p, int full = 0);
  void getViewport(RECT *r, RECT *sr, int full = 0);
  void getViewport(RECT *r, OSWINDOWHANDLE wnd, int full = 0);
  void getViewport(RECT *r, POINT *p, RECT *sr, OSWINDOWHANDLE wnd, int full = 0);
  int enumViewport(int monitor_n, RECT *r, int full = 0);
  RECT getAllMonitorsRect();

  void getMousePos(POINT *p);
  void getMousePos(int *x, int *y);
  void getMousePos(long *x, long *y);
  void setMousePos(POINT *p);
  void setMousePos(int x, int y);

  int getDoubleClickDelay();	// in ms
  int getDoubleClickX();	// in pixels
  int getDoubleClickY();	// in pixels

  int getLastInputInfo(); // time since last input in ms, or -1 if not supported

  int messageBox(const char *txt, const char *title, int flags);

  // returns how many lines to scroll for wheelie mice (from the OS)
  int mousewheelGetScrollLines();
  int mousewheelGetSmoothScroll();

  enum {
    SCROLLBAR_VWIDTH,
    SCROLLBAR_VHEIGHT,
    SCROLLBAR_HWIDTH,
    SCROLLBAR_HHEIGHT
  };
  int getMetric(int m);

  // clipboard
  // note: on win32, you need to enable WASABI_COMPILE_UTF
  int setClipboard(const char *txt);
  String getClipboard();

};//namespace StdWnd

#endif
#endif
