#ifndef DOCKWND_H
#define DOCKWND_H

#include <wnd/virtualwnd.h>
#include <wnd/dockhostwnd.h>

#define DOCKWND_PARENT VirtualWnd

class DockWnd : public DOCKWND_PARENT, public DockableWnd {
public:
  DockWnd();
  virtual ~DockWnd();

  // dockablewnd
  virtual int getDockedWidth();
  virtual int getDockedHeight();
  virtual void setDockedWidth(int w);
  virtual void setDockedHeight(int h);
  virtual int getDockSites();
  virtual void onDock(DockHostWnd *host, int site);
  virtual RootWnd *getWindow();
  virtual int getCurDockSite();
  virtual int getDockResizable();
  virtual void loadCustomState(const char *inifile, const char *section) {}
  virtual void saveCustomState(const char *inifile, const char *section) {}

  // impl

  void setDockResizable(int r);
  void setDockSites(int allowed_sites=DOCK_ALL) ;

  virtual int onLeftButtonDown(int x, int y);
  virtual int onLeftButtonUp(int x, int y);
  virtual int onRightButtonUp(int x, int y);
  virtual int onMouseMove(int x, int y);
  virtual int onResize();
  virtual int onInit();

  virtual int wantEraseBackground() { return 1; }
  virtual void onCancelCapture();
  virtual int hitTest(int x, int y) { return HITTEST_DRAGANDDOCK; } 
  virtual int isDragging() { return m_dragging; }
  virtual int getCursorType(int x, int y);

  virtual void getDockResizeRect(RECT *r); // override!
  DockHostWnd *getDockHost() { return m_dockhost; }

  enum {
    HITTEST_DRAGANDDOCK = 0,
    HITTEST_NONE,
    HITTEST_RESIZE_LEFT,
    HITTEST_RESIZE_TOP,
    HITTEST_RESIZE_RIGHT,
    HITTEST_RESIZE_BOTTOM,
  };

private:
  void drawDragRect(int x, int y);
  void eraseDragRect();
  void drawXorRect(RECT *r, int dropallowed);

  void drawResizeLine(int x, int y);
  void eraseResizeLine();
  void drawXorLine(int x, int y);

  int getDockResizeBarSize();

  enum {
    RESIZE_NONE = 0,
    RESIZE_LEFT,
    RESIZE_TOP,
    RESIZE_RIGHT,
    RESIZE_BOTTOM,
  };

  int m_allowed_sites;
  int m_docked_w;
  int m_docked_h;
  int m_cursite;
  int m_dragging;
  int m_dragrect_x;
  int m_dragrect_y;
  int m_dragrect_offset_x;
  int m_dragrect_offset_y;
  int m_dragrect_dropallowed;
  int m_drag_lastdocksite;
  int m_drag_lastdockpos;
  int m_dock_resizable;
  int m_resizing;
  int m_resize_clickx;
  int m_resize_clicky;
  int m_resize_x;
  int m_resize_y;
  int m_resize_offset;
  RECT m_dragrect;
  DockHostWnd *m_dockhost;
};

#endif
