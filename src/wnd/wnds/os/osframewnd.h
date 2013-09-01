#ifndef OSFRAMEWND_H
#define OSFRAMEWND_H

#include <wnd/dockhostwnd.h>
#include <wnd/canvas.h>
#include <wnd/popup.h>
#include <wnd/wnds/os/osstatusbarwnd.h>

class PopupMenu;
class CommandCallback;

#define OSFRAMEWND_PARENT DockHostWnd
class OSFrameWnd : public OSFRAMEWND_PARENT {
public:
  OSFrameWnd();
  virtual ~OSFrameWnd();

  virtual int onInit();
  virtual LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  virtual void setWindowFlags();

#ifdef WIN32
  void setIcon(int resourceid);
  HICON getSmallIcon() { return m_icon_small; }
  HICON getBigIcon() { return m_icon_big; }
#endif
  
  virtual void addMenu(const char *title, int pos, PopupMenu *menu, int deleteondestroy=1);
  virtual int onMenuCommand(int cmd);
  virtual void onMenuSysCommand(int cmd);
  virtual void onUserClose() {}

  OSMENUHANDLE getMenu();
  void resetMenu();

  virtual void setVisible(int show);
  virtual void assignWindowRegion(Region *wr) {}

  void setStatusBar(int enabled);
  void showStatusBar(int show);
  OSStatusBarWnd *getStatusBarWnd() { return m_statusbarwnd; }
  
  virtual int onResize();
  virtual int getBottomOffset();

  void setCommandCallback(CommandCallback *cc) { m_cc = cc; }
  CommandCallback *getCommandCallback() { return m_cc; }

  virtual void onMenuBarChanged();

  virtual int getMinFrameWidth() { return -1; }
  virtual int getMinFrameHeight() { return -1; }

private:
  void updateStatusBarVisibility();
#ifdef WIN32
  void updateMenu(HMENU menu, int ismenubar=0);
  HMENU m_menubar;
  HICON m_icon_small;
  HICON m_icon_big;
  int everbeenvisible;
#endif
  CommandCallback *m_cc;
  PtrList<PopupMenu> m_menulist;
  int m_statusbar;
  int m_statusbarvisible;
  OSStatusBarWnd *m_statusbarwnd;
};

#endif
