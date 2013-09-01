#ifndef OSDOCKWND_H
#define OSDOCKWND_H

#include <wnd/wnds/dockwnd.h>

// BU 6 looks better IMO
// FG not in mine :P so made it customizable
#define DEFAULT_RESIZEBAR_SIZE 8	 // back to 8 heh
#define GRIP_WIDTH	9
#define SNAPBTN_SIZE 64

#define OSDOCKWND_PARENT DockWnd
class OSDockWnd : public OSDOCKWND_PARENT {
public:
  OSDockWnd();

  virtual int onPaint(Canvas *c);
  virtual void getClientRect(RECT *r);
  void drawHGrip(Canvas *c, RECT r);
  void drawVGrip(Canvas *c, RECT r);

  virtual int wantBorder() { return m_bordervisible; }
  void setBorderVisible(int v);
  virtual int wantGrips() { return m_gripvisible; }
  void setGripVisible(int v);

  virtual int onLeftButtonDown(int x, int y);
  virtual int onLeftButtonUp(int x, int y);
  virtual int onMouseMove(int x, int y);
  virtual int hitTest(int x, int y);
  virtual void getDockResizeRect(RECT *r);

  virtual void loadCustomState(const char *inifile, const char *section);
  virtual void saveCustomState(const char *inifile, const char *section);

  // override for snap stuff
  virtual int getDockedWidth();
  virtual int getDockedHeight();
  virtual void setDockedWidth(int w);
  virtual void setDockedHeight(int h);

  void setDockSnapable(int snapable);

  virtual void onSnapClick();
  virtual void onDockChange();

  virtual int getResizeBarSize() { return DEFAULT_RESIZEBAR_SIZE; }

private:
  void drawSnapArrow(Canvas *c);
  int m_bordervisible;
  int m_gripvisible;
  int m_snapable;
  int m_snap_width;
  int m_snap_height;
  int m_snap_collapsed;
  RECT m_resizer;
  RECT m_snapbtn;
  int m_snap_bdown;
  int m_snap_inarea;
};

// this is a dockbar that can host another (non-dockable) wnd
#define OSDOCKWNDHOSTER_PARENT OSDockWnd
class OSDockWndHoster : public OSDOCKWNDHOSTER_PARENT {
public:
  OSDockWndHoster(BaseWnd *mywnd, int dockedwidth=-1);
  OSDockWndHoster(OSWINDOWHANDLE wnd, int dockedwidth=-1);
  OSDockWndHoster(int dockedwidth=-1);
  virtual ~OSDockWndHoster();

  virtual int onInit();
  virtual int onResize();

  void setWindowHandle(OSWINDOWHANDLE myhwnd);

private:
  OSWINDOWHANDLE myhwnd;
  BaseWnd *mywnd;
};

#endif
