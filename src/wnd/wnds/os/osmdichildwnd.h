#ifndef OSMDICHILDWND_H
#define OSMDICHILDWND_H

#include "osframewnd.h"

class OSMDIParentWnd;

#define OSMDICHILDWND_PARENT OSFrameWnd
class OSMDIChildWnd : public OSMDICHILDWND_PARENT {
public:
  OSMDIChildWnd();
  virtual ~OSMDIChildWnd();

  virtual int onInit();

  virtual void setWindowFlags();
  virtual int callDefProc(HWND wnd, UINT msg, int wparam, int lparam);
  virtual int wantEraseBackground() { return 1; }
  virtual LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual OSMDIParentWnd *getMDIParent();
  virtual OSMDIParentWnd *getOldMDIParent();
  virtual void setMDIParent(OSMDIParentWnd *parent);
  virtual void detachMDI();
  virtual void attachMDI(OSMDIParentWnd *mdiparent=NULL);
  virtual int isDetached();
  virtual int onResize();
  virtual int onPostedMove();

  virtual void activate();

  virtual void onMaximize();
  virtual void onMinimize();
  virtual void onRestore();

  virtual void deleteDeferred();

  virtual COLORREF getTitleColor() { return RGB(0,0,0); }

  virtual void onSetName();

  virtual int onKeyDown(int vk);
  virtual int excludeFromTabList() { return 1; }

private:
  OSMDIParentWnd *m_parent;
  OSMDIParentWnd *m_oldparent;
  int detached;
};

// a thin wrapper so any BaseWnd can be an mdi child wnd
// todo: auto-set name from child's name
#define OSMDICHILDWNDHOSTER_PARENT OSMDIChildWnd
class OSMDIChildWndHoster : public OSMDICHILDWNDHOSTER_PARENT {
public:
  OSMDIChildWndHoster(BaseWnd *mywnd);
  virtual ~OSMDIChildWndHoster();

  virtual int onInit();
  virtual int onResize();

  virtual void onUserClose();

protected:
  BaseWnd *mywnd;
};

#endif
