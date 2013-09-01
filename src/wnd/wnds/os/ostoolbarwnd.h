#ifndef OSTOOLBARWND_H
#define OSTOOLBARWND_H

#include "osdockwnd.h"
#include "ostooltip.h"

class OSToolBarWnd;
class CommandCallback;

class OSToolBarEntry {
protected:
  friend class OSToolBarWnd;
  OSToolBarEntry(OSToolBarWnd *parent, const char *tooltip=NULL);

public:
  virtual ~OSToolBarEntry();

  void setDisabled(int disabled) { m_disabled = disabled; }
  int isDisabled();
  void setCommand(int cmd) { m_cmd = cmd; }
  int getCommand() { return m_cmd; }
  void setTooltip(const char *tooltip) { m_tip = tooltip; }
  const char *getTooltip() { return m_tip; }
  void setLastRect(RECT r);
  RECT getLastRect();

  virtual void render(Canvas *canvas, const RECT &ir, int hilited, int pushed);
  virtual int getRenderWidth()=0;
  virtual int getRenderHeight()=0;
  virtual void setStretch(int stretch=1) { m_stretch = stretch; }
  virtual int getStretch() { return m_stretch; }
  virtual void onClicked() { }

  virtual void onParentInit() { }
  virtual void onParentResize() { }

  virtual int getIconResource() { return -1; }	// useful for switch statements

  OSToolBarWnd *getParent() { return m_parent; }

private:
  int m_disabled;
  int m_stretch;
  String m_tip;
  RECT m_rect;
  OSToolBarWnd *m_parent;
  int m_cmd;
};

class OSToolBarEntryIcon : public OSToolBarEntry {
public:
  OSToolBarEntryIcon(OSToolBarWnd *parent, int icon_resource, int icon_width, const char *tooltip=NULL); // icon entry
  ~OSToolBarEntryIcon();

  virtual void render(Canvas *canvas, const RECT &ir, int hilited, int pushed);

  HICON getIcon() { return m_icon; }
  virtual int getIconResource() { return m_icon_resource; }

  virtual int getRenderWidth() { return icon_width+8; }
  virtual int getRenderHeight() { return icon_width+8; }

private:
  int m_icon_resource;
  int icon_width;
  HICON m_icon;
};

class OSToolBarEntrySeparator : public OSToolBarEntry {
public:
  OSToolBarEntrySeparator(OSToolBarWnd *parent);

  virtual void render(Canvas *canvas, const RECT &ir, int hilited, int pushed);
  virtual int getRenderWidth() { return 9; }
  virtual int getRenderHeight() { return 9; }
};

class OSToolBarEntryWndHoster : public OSToolBarEntry {
public:
  OSToolBarEntryWndHoster(OSToolBarWnd *parent, BaseWnd *mywnd, int dockedwidth=-1);
  OSToolBarEntryWndHoster(OSToolBarWnd *parent, OSWINDOWHANDLE wnd, int dockedwidth=-1);
  OSToolBarEntryWndHoster(OSToolBarWnd *parent, int dockedwidth=-1);

  virtual ~OSToolBarEntryWndHoster();
  virtual void render(Canvas *canvas, const RECT &ir, int hilited, int pushed);
  virtual void onParentInit();
  virtual void onParentResize();

  void setWindowHandle(OSWINDOWHANDLE myhwnd);
  virtual int getRenderWidth();
  virtual int getRenderHeight();
  virtual int getWidth() { return m_dockedwidth; }
  virtual void setWidth(int width) { m_dockedwidth = width; }

private:
  OSWINDOWHANDLE myhwnd;
  BaseWnd *mywnd;
  int m_dockedwidth;
};

#define OSTOOLBARWND_PARENT OSDockWnd
class OSToolBarWnd : public OSTOOLBARWND_PARENT {
public:
  OSToolBarWnd();
  virtual ~OSToolBarWnd();

  virtual int onInit();
  virtual int onResize();
  virtual int onPaint(Canvas *c);
  virtual int getDockedHeight() { return 30; }
  virtual int getDockedWidth() { return 30; }

  OSToolBarEntry *addIconEntry(int icon_resource, int icon_width=16, const char *tooltip=NULL);
  OSToolBarEntry *addSeparator();
  OSToolBarEntry *addUserClass(OSToolBarEntry *wnd);
  void removeUserClass(OSToolBarEntry *wnd);

  int getNumEntries() { return m_entries.getNumItems(); }
  OSToolBarEntry *enumEntry(int n) { return m_entries.enumItem(n); }

  void setCommandCallback(CommandCallback *cc) { m_cc = cc; }
  CommandCallback *getCommandCallback() { return m_cc; }

  virtual int onMouseMove(int x, int y);

  virtual void onLeaveIcon(OSToolBarEntry *entry);
  virtual void onEnterIcon(OSToolBarEntry *entry);
  virtual void onClickIcon(OSToolBarEntry *entry);

  void invalidateIcon(OSToolBarEntry *entry);
  virtual int hitTest(int x, int y); // return 0 to let the dockwnd handle click for drag&dock
  virtual int onLeftButtonDown(int x, int y);
  virtual int onLeftButtonUp(int x, int y);

  virtual int onMiddleButtonDown(int x, int y);
  virtual int onMiddleButtonUp(int x, int y);

  OSToolBarEntry *getEntryAt(int x, int y);
  int getEntryPos(OSToolBarEntry *entry);
  virtual int wantEraseBackground() { return 0; }

protected:
  int want_middle_clicks, is_middle_click;

private:
  void localMouseMove(int x, int y);
  PtrList<OSToolBarEntry> m_entries;
  OSToolBarEntry *m_hilite;
  int m_down, m_last_pushed;
  CommandCallback *m_cc;
  OSToolTip *m_tip;
};

#endif
