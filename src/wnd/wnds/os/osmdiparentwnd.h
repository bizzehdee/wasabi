#ifndef OSMDIPARENTWND_H
#define OSMDIPARENTWND_H

#include "osframewnd.h"
#include "osmdiclientwnd.h"
#include "osmdichildwnd.h"

#define MENU_COMMAND_CASCADE 0xEF00
#define MENU_COMMAND_VTILE   0xEF01
#define MENU_COMMAND_HTILE   0xEF02
#define MENU_COMMAND_ARRANGE 0xEF03

class OSMDIChildWnd;

#define OSMDIPARENTWND_PARENT OSFrameWnd

class OSMDIChildWndSorter {
public:
  // comparator for sorting
  static int compareItem(OSMDIChildWnd *p1, OSMDIChildWnd* p2) {
    return CMP3(p1->gethWnd(), p2->gethWnd());
  }
  // comparator for searching
  static int compareAttrib(const char *attrib, OSMDIChildWnd *item) {
    return CMP3(*(HWND*)attrib, item->gethWnd());
  }
};

class OSMDIParentWnd : public OSFrameWnd {
public:
  OSMDIParentWnd();
  virtual ~OSMDIParentWnd();

  virtual int onInit();
  virtual int onResize();

  OSMDIChildWnd *addMDIChild(OSMDIChildWnd *child, int width=-1, int height=-1, int inback=FALSE, int novis=FALSE);
  void deleteMDIChildDeferred(OSMDIChildWnd *child);
  virtual void onDeleteMDIChildDeferred(OSMDIChildWnd *child) { }

  virtual void onUserClose();

  virtual void onMDIChildActivate(OSMDIChildWnd *child, int forced=0);
  virtual void onMDIChildDelete(OSMDIChildWnd *child);

  virtual void onMDIChildDetach(OSMDIChildWnd *child);
  virtual void onMDIChildAttach(OSMDIChildWnd *child);

  virtual void onMDIChildMaximize(OSMDIChildWnd *child);
  virtual void onMDIChildRestore(OSMDIChildWnd *child);

  virtual void onMDIChildNameChange(OSMDIChildWnd *child);

  int onMenuCommand(int cmd);
  void onMenuSysCommand(int cmd);

  virtual int onPostOnInit();

  void getMDICloseButtonRect(RECT *r);
  void getMDIRestoreButtonRect(RECT *r);
  void getMDIMinimizeButtonRect(RECT *r);

  virtual void onMDICloseClicked();
  virtual void onMDIRestoreClicked();
  virtual void onMDIMinimizeClicked();
  virtual int callDefProc(HWND wnd, UINT msg, int wparam, int lparam);
  virtual LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  void updateMDIControlButtons(int curzoomed);
  int hitTestMDIControlButtons(POINT *pt);

  int areAllMDIMinimized();
  void nextMDI(OSMDIChildWnd *child=NULL);
  void previousMDI(OSMDIChildWnd *child=NULL);
  void cascadeMDI();
  void tileMDIVertically();
  void tileMDIHorizontally();
  void arrangeMDIIcons();

  void updateMenuForMDI(int onoff);

  OSMDIClientWnd *getMDIClient();
  OSMDIChildWnd *getCurMDIChild() { return m_curchild; }
  int getNumMDIChildren() { return m_mdis.getNumItems(); }
  OSMDIChildWnd *enumMDIChild(int i) { return m_mdis.enumItem(i); }
  void onMDIChildMove(OSMDIChildWnd *child);
  void updateScrollbars();

  void onMDIScroll();
  void onMDIEndScroll();
  int isMDIScrolling();

  void remaximizeMDIs();
  void remaximizeMDI(OSMDIChildWnd *child);

  void maximizeMDIchild(OSMDIChildWnd *child);

  int numMDIChildMaximized();

  void allowMixedMode(int allowed);

  virtual int onKeyDown(int kc);
  virtual int onKeyUp(int kc);

  virtual void *getInterface(GUID interfaceGuid);

private:
  //virtual int timerclient_onDeferredCallback(int param1, int param2);

  void drawControlButtons();
  void undrawControlButtons();
  void syncSysMenuForMDI();
  void tailorMenu(HMENU menu);
  void tailorSubmenu(HMENU menu);
  void insertWindowList(HMENU menu, int pos);
  void cacheBitmaps();
  void updateTitle();

  int inmenuupdate;
  OSMDIClientWnd *m_client;
  OSMDIChildWnd *m_curchild;
  OSMDIChildWnd *m_last_inserted_child;
  int last_was_maximized;
  PtrList<OSMDIChildWnd> m_mdis;
  PtrListQuickSorted<OSMDIChildWnd, OSMDIChildWndSorter> m_sorted_mdis;
  int mdi_button_down;
  int mdi_button_inarea;
  HMENU m_lastsysmenu;
  int m_scrolling;
  int m_mixedmode;
  PtrList<OSMDIChildWnd> delchildlist;

  HBITMAP m_mdi_buttons[2][3];
};

extern const GUID GUID_OSMDIPARENT_CLASS;

#endif
