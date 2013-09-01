#include <precomp.h>
#include "osmdichildwnd.h"
#include "osmdiparentwnd.h"
#include <wnd/popup.h>
#include <bfc/std_wnd.h>

// {9F1E1BA1-25C1-4a6b-8661-7D64B30D7721}
static const GUID GUID_OSMDIPARENT_CLASS = 
{ 0x9f1e1ba1, 0x25c1, 0x4a6b, { 0x86, 0x61, 0x7d, 0x64, 0xb3, 0xd, 0x77, 0x21 } };


#define DEFER_DELETE_CHILD WM_USER+420

OSMDIParentWnd::OSMDIParentWnd() {
  m_client = NULL;
  m_curchild = NULL;
  m_last_inserted_child = NULL;
  mdi_button_down = 0;
  mdi_button_inarea = 0;
  m_lastsysmenu = (HMENU)-1;
  last_was_maximized = 0;
  inmenuupdate = 0;
  m_scrolling = 0;
  m_mixedmode = 1;
  setNonBuffered(1);

  // we do this because if we were to use DrawFrameControl during 
  // a WM_NCPAINT, winXP would drop the theme
  m_mdi_buttons[0][0] = 
    m_mdi_buttons[0][1] = 
    m_mdi_buttons[0][2] = 
    m_mdi_buttons[1][0] = 
    m_mdi_buttons[1][1] = 
    m_mdi_buttons[1][2] = NULL;
  cacheBitmaps();
}

OSMDIParentWnd::~OSMDIParentWnd() {
  if (m_mdi_buttons[0][0]) DeleteObject(m_mdi_buttons[0][0]);
  if (m_mdi_buttons[1][1]) DeleteObject(m_mdi_buttons[1][1]);
  if (m_mdi_buttons[0][2]) DeleteObject(m_mdi_buttons[0][2]);
  if (m_mdi_buttons[1][0]) DeleteObject(m_mdi_buttons[1][0]);
  if (m_mdi_buttons[0][1]) DeleteObject(m_mdi_buttons[0][1]);
  if (m_mdi_buttons[1][2]) DeleteObject(m_mdi_buttons[1][2]);
  delete m_client; m_client = NULL;
}

int OSMDIParentWnd::onInit() {
  OSMDIPARENTWND_PARENT::onInit();
  m_client = new OSMDIClientWnd(this);

  RECT r;
  getClientRect(&r);

  m_client->setParent(this);
  m_client->resize(&r);
  m_client->setVisible(1);
  m_client->init(this);

  return 1;
}

int OSMDIParentWnd::onPostOnInit() {
  OSMDIPARENTWND_PARENT::onPostOnInit();
  if (m_last_inserted_child != NULL)
    onMDIChildActivate(m_last_inserted_child, 1);
  else
    onResize(); // also done by onMDIChildActivate 
  return 1;
}

int OSMDIParentWnd::onResize() {
  OSMDIPARENTWND_PARENT::onResize();
  RECT r;
  getClientRect(&r);
  m_client->resize(&r);
  return 1;
}

OSMDIChildWnd *OSMDIParentWnd::addMDIChild(OSMDIChildWnd *child, int width, int height, int inback, int novis) {
  ASSERT(!child->isInited());
  if (m_mdis.haveItem(child)) return child;

  OSMDIChildWnd *prev_curchild = m_curchild;
  bool maximize = !m_mixedmode && numMDIChildMaximized() || last_was_maximized ;
  child->setParent(m_client);
  child->setMDIParent(this);
  child->setStartHidden(TRUE);
  child->init(m_client);
  if ( !maximize ) {
    int n = m_mdis.getNumItems();
    int x = (n%16)*16;
    if (width != -1 && height != -1) {
      child->resize(x, x, width, height);
    } else {
      width = child->getPreferences(SUGGESTED_W);
      height = child->getPreferences(SUGGESTED_H);
      if (width != AUTOWH && height != AUTOWH) {
        POINT pt;
        child->getPosition(&pt);
        child->resize(x, x, width, height);
      }
    }
  }
  if (!inback && maximize && !novis) child->setVisible(TRUE);
  if ( maximize ) {
    maximizeMDIchild(child);
    if (!inback) {
      child->bringToFront();
    } else {
      child->bringToBack();
      child->setVisible(TRUE);
      // keep prev active
      if (prev_curchild) prev_curchild->OSMDIChildWnd::activate();
    }
  } else {
    if( !novis ) child->setVisible(TRUE);
  }

  last_was_maximized = 0;

  if (isPostOnInit()) onResize();
  m_last_inserted_child = child;
//  if (m_mdis.getNumItems() == 1 && isPostOnInit())
    onMenuBarChanged();
  return child;
}

void OSMDIParentWnd::deleteMDIChildDeferred(OSMDIChildWnd *child) {
  ASSERT(m_mdis.haveItem(child) || child->isDetached());
  if (!delchildlist.haveItem(child)) {
    delchildlist.addItem(child);
    onDeleteMDIChildDeferred(child);
    
    // this used to use timerclient defercb but it can be delayed up to 500ms
    PostMessage(gethWnd(), DEFER_DELETE_CHILD, 0, 0);
    // lone> use this again when timerclient_defercb is tweaked to never have any delay
    //    timerclient_postDeferredCallback(DEFER_DELETE_CHILD);
  }
}

void OSMDIParentWnd::onUserClose() {
  WASABI_API_APP->main_shutdown();
}

void OSMDIParentWnd::onMDIChildActivate(OSMDIChildWnd *child, int forced) {
  if (!forced && m_curchild == child) return;
  updateMenuForMDI(0);
  if ( m_curchild ) m_curchild->onDeactivate();
  m_curchild = child;
  m_last_inserted_child = m_curchild; // in case of reinit
  if (isPostOnInit()) {
    OSMENUHANDLE menu = m_curchild ? m_curchild->getMenu() : NULL;
    if (menu) {
      tailorMenu(menu);
      SetMenu(gethWnd(), menu); 
    } else {
      tailorMenu(getMenu());
      resetMenu();
    }
    onResize();
  }

  if (child) updateMDIControlButtons(child->isMaximized());
  updateTitle();
  invalidateDockedWindows();
}

void OSMDIParentWnd::onMDIChildDelete(OSMDIChildWnd *child) {
  onMDIChildDetach(child);
  updateMenuForMDI(0);
}

void OSMDIParentWnd::onMDIChildAttach(OSMDIChildWnd *child) {
  if (!m_mdis.haveItem(child)) {
    m_mdis.addItem(child);
    m_sorted_mdis.addItem(child);
  }
  updateScrollbars();
}

void OSMDIParentWnd::onMDIChildDetach(OSMDIChildWnd *child) {
  previousMDI(child);
  int wassel = 0;
  int wasmax = child->isMaximized();
  if (child == m_curchild) {
    wassel = 1;
    m_curchild = NULL;
  }
  m_mdis.removeItem(child);
  m_sorted_mdis.removeItem(child);
  if (wassel) {
    StdWnd::setWndName(gethWnd(), getName());
    tailorMenu(getMenu());
    onMenuBarChanged();
    invalidateDockedWindows();
  }
  updateScrollbars();

  // last window to detach, so remember its maximization state
  if (m_mdis.n() == 0) last_was_maximized = wasmax;
}

int OSMDIParentWnd::onMenuCommand(int cmd) {
  switch (cmd) {
    case MENU_COMMAND_CASCADE: cascadeMDI(); return 1;
    case MENU_COMMAND_VTILE: tileMDIVertically(); return 1;
    case MENU_COMMAND_HTILE: tileMDIHorizontally(); return 1;
    case MENU_COMMAND_ARRANGE: arrangeMDIIcons(); return 1;
  }
#ifdef DETACHABLE
  if (cmd == MDICHILD_SYSMENU_DETACH-1 ) 
    if ( m_curchild ) {
      m_curchild->detachMDI();
      updateMenuForMDI(0); //removes the sysmenu if the child was maximized
    }
#endif
  if (cmd >= 0xEF10 && cmd <= 0xEFFF) {
    int id = cmd - 0xEF10;
    OSMDIChildWnd *child = m_mdis.enumItem(id);
    if (!child) return 0;
    SendMessage(m_client->gethWnd(), WM_MDIACTIVATE, (WPARAM)child->gethWnd(), 0);
    return 1;
  }
  
  if (m_curchild && m_curchild->onMenuCommand(cmd)) return 1;
  return OSMDIPARENTWND_PARENT::onMenuCommand(cmd);
}

void OSMDIParentWnd::onMenuSysCommand(int cmd) {
  if (m_curchild) SendMessage(m_curchild->gethWnd(), WM_SYSCOMMAND, cmd, 0);
}

int OSMDIParentWnd::callDefProc(HWND wnd, UINT msg, int wparam, int lparam) {
  if (m_client) return DefFrameProc(wnd, m_client->gethWnd(), msg, wparam, lparam);
  return DefWindowProc(wnd, msg, wparam, lparam);
}

LRESULT OSMDIParentWnd::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (uMsg == WM_SETTINGCHANGE) {
    cacheBitmaps();
  } else if (uMsg == DEFER_DELETE_CHILD) {
    delchildlist.deleteAll();	// it's just that easy
  } else if (GetMenu(hWnd) && m_curchild && m_curchild->isMaximized()) {
    switch (uMsg) {
      case WM_DRAWITEM: {
        DRAWITEMSTRUCT *dps = (DRAWITEMSTRUCT *)lParam;
        if (dps->CtlType == ODT_MENU && dps->itemID == (UINT)m_lastsysmenu) {
          int w = dps->rcItem.right-dps->rcItem.left-2;
          int h = dps->rcItem.bottom-dps->rcItem.top-2;
          DrawIconEx(dps->hDC, dps->rcItem.left+1, dps->rcItem.top+1, m_curchild->getSmallIcon(), w, h, 0, NULL, DI_NORMAL);        
        }
        return OSMDIPARENTWND_PARENT::wndProc(hWnd, uMsg, wParam, lParam);
      }
      case WM_MEASUREITEM: {
        MEASUREITEMSTRUCT *mis = (MEASUREITEMSTRUCT *)lParam;
        if (mis->itemID == (UINT)m_lastsysmenu) {
          mis->itemWidth = GetSystemMetrics(SM_CXMENUSIZE) - 11;
          mis->itemHeight = GetSystemMetrics(SM_CXMENUSIZE) - 11;
        }
        return 0;
      }

      case WM_NCACTIVATE: {
        OSMDIPARENTWND_PARENT::wndProc(hWnd, uMsg, wParam, lParam);
        drawControlButtons();
        return TRUE;
      }
      case WM_NCPAINT: {
        int r = OSMDIPARENTWND_PARENT::wndProc(hWnd, uMsg, wParam, lParam);
        drawControlButtons();
        return r;
      }
      case WM_NCLBUTTONDOWN: {
        POINTS pts = MAKEPOINTS(lParam);
        RECT wr;
        GetWindowRect(hWnd, &wr);
        POINT p = {pts.x-wr.left, pts.y-wr.top};
        int r = hitTestMDIControlButtons(&p);
        if (r != 0) {
          SetCapture(hWnd);
          mdi_button_down = mdi_button_inarea = r;
          drawControlButtons();
          return 0;
        }
        break;
      }
      case WM_LBUTTONUP:
      case WM_NCLBUTTONUP: {
        int old_down = mdi_button_down;
        if (old_down) {
          ReleaseCapture();
          mdi_button_down = 0;
          drawControlButtons();

          POINTS pts = MAKEPOINTS(lParam);
          POINT p = {pts.x, pts.y};
          if (uMsg == WM_LBUTTONUP) {
            ClientToScreen(hWnd, &p);
          }
          RECT wr;
          GetWindowRect(hWnd, &wr);
          p.x -= wr.left;
          p.y -= wr.top;
          mdi_button_inarea = hitTestMDIControlButtons(&p);
          if (mdi_button_inarea == old_down) {
            switch (mdi_button_inarea) {
              case 3:
                onMDICloseClicked();
                return 0;
              case 2:
                onMDIRestoreClicked();
                return 0;
              case 1:
                onMDIMinimizeClicked();
                return 0;
            }
          }
        }
        break;
      }
      case WM_CAPTURECHANGED: {
        if (mdi_button_down) {
          mdi_button_down = mdi_button_inarea = 0;
          drawControlButtons();
        }
        break;
      }
      case WM_MOUSEMOVE: 
      case WM_NCMOUSEMOVE: {
        if (mdi_button_down) {
          POINTS pts = MAKEPOINTS(lParam);
          POINT p = {pts.x, pts.y};
          if (uMsg == WM_MOUSEMOVE) {
            ClientToScreen(hWnd, &p);
          }
          RECT wr;
          GetWindowRect(hWnd, &wr);
          p.x -= wr.left;
          p.y -= wr.top;
          mdi_button_inarea = hitTestMDIControlButtons(&p);
          drawControlButtons();
          return 0;
        }
        break;
      }
    }
  }
  return OSMDIPARENTWND_PARENT::wndProc(hWnd, uMsg, wParam, lParam);
}

void OSMDIParentWnd::updateMDIControlButtons(int curzoomed) {
  HWND hWnd = gethWnd();
  if (GetMenu(hWnd) && curzoomed) {
    drawControlButtons();
  } else if (GetMenu(hWnd) && !curzoomed) {
    undrawControlButtons();
  }
}

void OSMDIParentWnd::updateMenuForMDI(int on) {
  if (inmenuupdate) return;
  if (!GetMenu(gethWnd())) return;
  inmenuupdate = 1;
  if (on) {
    MENUITEMINFO mii = {sizeof(MENUITEMINFO), 0};
    mii.fMask = MIIM_SUBMENU;
    GetMenuItemInfo(GetMenu(gethWnd()), 0, TRUE, &mii);
    if (mii.hSubMenu == m_lastsysmenu) { syncSysMenuForMDI(); inmenuupdate = 0; return; }
    m_lastsysmenu = GetSystemMenu(m_curchild->gethWnd(), FALSE);
    syncSysMenuForMDI();
    InsertMenu(GetMenu(gethWnd()), 0, MF_BYPOSITION|MFT_OWNERDRAW|MF_POPUP, (UINT)m_lastsysmenu, "X");
    onMenuBarChanged();
  } else {
    MENUITEMINFO mii = {sizeof(MENUITEMINFO), 0};
    mii.fMask = MIIM_SUBMENU;
    GetMenuItemInfo(GetMenu(gethWnd()), 0, TRUE, &mii);
    if (mii.hSubMenu != m_lastsysmenu) { inmenuupdate = 0; return; }
    RemoveMenu(GetMenu(gethWnd()), 0, MF_BYPOSITION);
    onMenuBarChanged();
  }
  inmenuupdate = 0;
}

void OSMDIParentWnd::syncSysMenuForMDI() {
  if (!m_curchild) return;
  if (!IsMenu(m_lastsysmenu)) return;
  if (m_curchild->isMaximized()) {
    EnableMenuItem(m_lastsysmenu, SC_RESTORE, MF_ENABLED);
    EnableMenuItem(m_lastsysmenu, SC_MAXIMIZE, MF_GRAYED);
    EnableMenuItem(m_lastsysmenu, SC_MOVE, MF_GRAYED);
    EnableMenuItem(m_lastsysmenu, SC_SIZE, MF_GRAYED);
  } else if (m_curchild->isMinimized()) {
    EnableMenuItem(m_lastsysmenu, SC_RESTORE, MF_ENABLED);
    EnableMenuItem(m_lastsysmenu, SC_MINIMIZE, MF_GRAYED);
    EnableMenuItem(m_lastsysmenu, SC_MOVE, MF_GRAYED);
    EnableMenuItem(m_lastsysmenu, SC_SIZE, MF_GRAYED);
  } else {
    EnableMenuItem(m_lastsysmenu, SC_RESTORE, MF_GRAYED);
    EnableMenuItem(m_lastsysmenu, SC_MINIMIZE, MF_ENABLED);
    EnableMenuItem(m_lastsysmenu, SC_MAXIMIZE, MF_ENABLED);
    EnableMenuItem(m_lastsysmenu, SC_MOVE, MF_ENABLED);
    EnableMenuItem(m_lastsysmenu, SC_SIZE, MF_ENABLED);
  }
}

void OSMDIParentWnd::drawControlButtons() {
  updateScrollbars();
  updateMenuForMDI(1);

  HDC hdc;
  HWND hWnd = gethWnd();
  hdc = GetWindowDC(hWnd);

  RECT r;

  getMDICloseButtonRect(&r);

  HDC bdc = CreateCompatibleDC(hdc);
  HBITMAP oldbmp = (HBITMAP)SelectObject(bdc, m_mdi_buttons[(mdi_button_down == 3 && mdi_button_inarea == 3) ? 1 : 0][0]);
  BitBlt(hdc, r.left, r.top, r.right-r.left, r.bottom-r.top, bdc, 0, 0, SRCCOPY);

  getMDIRestoreButtonRect(&r);
  SelectObject(bdc, m_mdi_buttons[(mdi_button_down == 2 && mdi_button_inarea == 2) ? 1 : 0][1]);
  BitBlt(hdc, r.left, r.top, r.right-r.left, r.bottom-r.top, bdc, 0, 0, SRCCOPY);

  getMDIMinimizeButtonRect(&r);
  SelectObject(bdc, m_mdi_buttons[(mdi_button_down == 1 && mdi_button_inarea == 1) ? 1 : 0][2]);
  BitBlt(hdc, r.left, r.top, r.right-r.left, r.bottom-r.top, bdc, 0, 0, SRCCOPY);

  SelectObject(bdc, oldbmp);
  DeleteDC(bdc);

  ReleaseDC(hwnd, hdc);
}

void OSMDIParentWnd::undrawControlButtons() {
  updateScrollbars();
  updateMenuForMDI(0);

  HDC hdc;
  HWND hWnd = gethWnd();
  hdc = GetWindowDC(hWnd);

  RECT r;

  getMDICloseButtonRect(&r);
  FillRect(hdc, &r, GetSysColorBrush(COLOR_3DFACE));

  getMDIRestoreButtonRect(&r);
  FillRect(hdc, &r, GetSysColorBrush(COLOR_3DFACE));

  getMDIMinimizeButtonRect(&r);
  FillRect(hdc, &r, GetSysColorBrush(COLOR_3DFACE));

  ReleaseDC(hwnd, hdc);
}

void OSMDIParentWnd::getMDICloseButtonRect(RECT *rr) {
  RECT r;
  HWND hWnd = gethWnd();
  GetWindowRect(hWnd, &r);
  OffsetRect(&r, -r.left, -r.top);
  r.right -= GetSystemMetrics(SM_CXBORDER) + GetSystemMetrics(SM_CXSIZEFRAME);
  RECT cr;
  cr.right = r.right;
  cr.left = cr.right - (GetSystemMetrics(SM_CXMENUSIZE)-2);
  cr.top = r.top + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXBORDER) + GetSystemMetrics(SM_CXSIZEFRAME) + 1;
  cr.bottom = cr.top + GetSystemMetrics(SM_CYMENUSIZE)-4;
  *rr = cr;
}

void OSMDIParentWnd::getMDIRestoreButtonRect(RECT *rr) {
  getMDICloseButtonRect(rr);
  OffsetRect(rr, -(GetSystemMetrics(SM_CXMENUSIZE)-1), 0);
}

void OSMDIParentWnd::getMDIMinimizeButtonRect(RECT *rr) {
  getMDIRestoreButtonRect(rr);
  OffsetRect(rr, -(GetSystemMetrics(SM_CXMENUSIZE)-1), 0);
}

void OSMDIParentWnd::onMDICloseClicked() {
  if (m_curchild) m_curchild->onUserClose();
}

void OSMDIParentWnd::onMDIRestoreClicked() {
  if (m_curchild) SendMessage(m_curchild->gethWnd(), WM_SYSCOMMAND, SC_RESTORE, 0);
}

void OSMDIParentWnd::onMDIMinimizeClicked() {
  if (m_curchild) SendMessage(m_curchild->gethWnd(), WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

int OSMDIParentWnd::hitTestMDIControlButtons(POINT *pt) {
  RECT r;
  getMDICloseButtonRect(&r);
  if (PtInRect(&r, *pt)) return 3;
  getMDIRestoreButtonRect(&r);
  if (PtInRect(&r, *pt)) return 2;
  getMDIMinimizeButtonRect(&r);
  if (PtInRect(&r, *pt)) return 1;
  return 0;
}

void OSMDIParentWnd::nextMDI(OSMDIChildWnd *child) {
  if (child == NULL) child = m_curchild;
  if (!child) return;
  OSMDIChildWnd *next = NULL;
  foreach(m_mdis)
    if (m_mdis.getfor() == child) {
      next = m_mdis.enumItem(foreach_index+1);
      break;
    }
  endfor;
  if (!next) next = m_mdis.getFirst();
  if (!next) return;
  next->activate();
}

void OSMDIParentWnd::previousMDI(OSMDIChildWnd *child) {
  if (child == NULL) child = m_curchild;
  if (!child) return;
  OSMDIChildWnd *previous = NULL;
  foreach(m_mdis)
    if (m_mdis.getfor() == child) {
      previous = m_mdis.enumItem(foreach_index-1);
      break;
    }
  endfor;
  if (!previous) previous = m_mdis.getLast();
  if (!previous) return;
  previous->activate();
}

int OSMDIParentWnd::areAllMDIMinimized() {
  foreach(m_mdis)
    if (!m_mdis.getfor()->isMinimized()) return 0;
  endfor
  return 1;
}

void OSMDIParentWnd::tailorMenu(HMENU menu) {
  int n = GetMenuItemCount(menu);
  for (int i=0;i<n;i++) {
    char buf[256];
    MENUITEMINFO mii = {sizeof(MENUITEMINFO), 0};
    mii.dwTypeData = buf;
    mii.cch = 256;
    mii.fMask = MIIM_DATA|MIIM_ID|MIIM_STATE|MIIM_SUBMENU|MIIM_TYPE;
    GetMenuItemInfo(menu, i, TRUE, &mii);
    if (mii.dwTypeData) {
      mii.dwTypeData[255] = 0;
      if (STRCASEEQL(mii.dwTypeData, "&Window") && mii.hSubMenu != NULL) {
        tailorSubmenu(mii.hSubMenu);
        return;
      }
    }
  }
  // not found, add it
  HMENU windowmenu = CreateMenu();
  tailorSubmenu(windowmenu);
  InsertMenu(menu, n+1, MF_BYPOSITION|MF_POPUP, (UINT)windowmenu, "&Window");
}

void OSMDIParentWnd::tailorSubmenu(HMENU menu) {
  int n = GetMenuItemCount(menu);
  if (n == 0) {
    InsertMenu(menu, 0, MF_BYPOSITION|MF_STRING, 0xEF00+1, "&Cascade");
    InsertMenu(menu, 1, MF_BYPOSITION|MF_STRING, 0xEF01+1, "&Tile Vertically");
    InsertMenu(menu, 2, MF_BYPOSITION|MF_STRING, 0xEF02+1, "Tile &Horizontally");
    InsertMenu(menu, 3, MF_BYPOSITION|MF_STRING, 0xEF03+1, "&Arrange Icons");
    InsertMenu(menu, 4, MF_BYPOSITION|MF_SEPARATOR, 0, NULL);
    insertWindowList(menu, 5);
    return;
  } else {
    // find the last menu separator and delete window list
    MENUITEMINFO mii = {sizeof(MENUITEMINFO), 0};
    mii.fMask = MIIM_TYPE|MIIM_ID;
    for (int i=n;i>=0;i--) {
      mii.cch = 0;
      GetMenuItemInfo(menu, i, TRUE, &mii);
      if (mii.fType & MFT_SEPARATOR) {
        insertWindowList(menu, i+1);
        return;
      }
      if (mii.wID >= 0xEF10+1 && mii.wID <= 0xEFFF+1) DeleteMenu(menu, mii.wID, MF_BYCOMMAND);
    }
  }
  // not found, add it
  InsertMenu(menu, n, MF_BYPOSITION|MF_SEPARATOR, 0, NULL);
  insertWindowList(menu, n+1);
}

void OSMDIParentWnd::insertWindowList(HMENU menu, int pos) {
  foreach(m_mdis)
    if (foreach_index >= 0xE0) break;
    OSMDIChildWnd *child = m_mdis.getfor();
    const char *name = child->getName();
    if (!name || !*name) name = "Untitled";
    InsertMenu(menu, pos+foreach_index, MF_BYPOSITION, 0xEF10+1+foreach_index, name);
  endfor
}

void OSMDIParentWnd::cascadeMDI() {
  if (m_curchild && m_curchild->isMaximized()) undrawControlButtons();
  SendMessage(m_client->gethWnd(), WM_MDICASCADE, MDITILE_SKIPDISABLED, 0);
}

void OSMDIParentWnd::tileMDIVertically() {
  foreach(m_mdis)
    OSMDIChildWnd *child = m_mdis.getfor();
    if (child->isMaximized() || child->isMinimized()) child->onRestore();
  endfor
  undrawControlButtons();
  SendMessage(m_client->gethWnd(), WM_MDITILE, MDITILE_SKIPDISABLED|MDITILE_VERTICAL, 0);
}

void OSMDIParentWnd::tileMDIHorizontally() {
  foreach(m_mdis)
    OSMDIChildWnd *child = m_mdis.getfor();
    if (child->isMaximized() || child->isMinimized()) child->onRestore();
  endfor
  undrawControlButtons();
  SendMessage(m_client->gethWnd(), WM_MDITILE, MDITILE_SKIPDISABLED|MDITILE_HORIZONTAL, 0);
}

void OSMDIParentWnd::arrangeMDIIcons() {
  SendMessage(m_client->gethWnd(), WM_MDIICONARRANGE, 0, 0);
}

OSMDIClientWnd *OSMDIParentWnd::getMDIClient() {
  return m_client;
}

#define DIVISOR 8

void OSMDIParentWnd::onMDIChildMove(OSMDIChildWnd *child) {
  updateScrollbars();
}


void OSMDIParentWnd::updateScrollbars() {
  if (m_curchild && IsZoomed(m_curchild->gethWnd())) {
    SCROLLINFO si={sizeof(SCROLLINFO),0};
    si.fMask = SIF_PAGE|SIF_RANGE;
    si.nPage = 0;
    si.nMin = si.nMax = 0; 
    SetScrollInfo(getMDIClient()->gethWnd(), SB_HORZ, &si, TRUE);
    SetScrollInfo(getMDIClient()->gethWnd(), SB_VERT, &si, TRUE);
    remaximizeMDIs();
    return;
  }

  RECT pr;
  getMDIClient()->getWindowRect(&pr);
  getMDIClient()->screenToClient(&pr);

  int w = pr.right-pr.left;
  int h = pr.bottom-pr.top;

  int needv = 0;
  int needh = 0;

  RECT wr={0,0,0,0};

  redo:

  foreach(m_mdis)

    RECT tr;
    if (m_mdis.getfor()->isMaximized()) continue;

    m_mdis.getfor()->getWindowRect(&tr);
    getMDIClient()->screenToClient(&tr);

    if (!needh) {
      if (tr.right > w || tr.left < 0) {
        needh = !!1;
        pr.bottom -= GetSystemMetrics(SM_CYVSCROLL);
        h = pr.bottom-pr.top;
        goto redo;
      }
    }

    if (!needv) {
      if (tr.bottom > h || tr.top < 0) {
        needv = !!1;
        pr.right -= GetSystemMetrics(SM_CXVSCROLL);
        w = pr.right-pr.left;
        goto redo;
      }
    }

    wr.left = MIN(tr.left, wr.left);
    wr.top = MIN(tr.top, wr.top);
    wr.right = MAX(tr.right, wr.right);
    wr.bottom = MAX(tr.bottom, wr.bottom);
  
  endfor


  SCROLLINFO si={sizeof(SCROLLINFO),0};
  si.fMask = SIF_PAGE|SIF_RANGE;
  si.nPage = w / DIVISOR;
  if (!needh) { 
    si.nMin = si.nMax = 0; 
  } else {
    si.nMin = MIN<int>(wr.left, 0) / DIVISOR;
    if (wr.left < 0) si.nMin--;
    si.nMax = MAX<int>(w, wr.right) / DIVISOR; 
    if (wr.right > w) si.nMax++; else si.nMax--;
  }
  SetScrollInfo(getMDIClient()->gethWnd(), SB_HORZ, &si, TRUE);
  si.nPage = h / DIVISOR;
  if (!needv) {
    si.nMin = si.nMax = 0; 
  } else {
    si.nMin = MIN<int>(wr.top, 0) / DIVISOR;
    if (wr.top < 0) si.nMin--;
    si.nMax = MAX<int>(h, wr.bottom) / DIVISOR; 
    if (wr.bottom > h) si.nMax++; else si.nMax--;
  }
  SetScrollInfo(getMDIClient()->gethWnd(), SB_VERT, &si, TRUE);
  remaximizeMDIs();
}

void OSMDIParentWnd::onMDIScroll() {
  m_scrolling = 1;
}

void OSMDIParentWnd::onMDIEndScroll() {
  m_scrolling = 0;
  updateScrollbars();
  if (GetCapture() == gethWnd()) SetCapture(NULL);
}

int OSMDIParentWnd::isMDIScrolling() {
  return m_scrolling;
}

void OSMDIParentWnd::remaximizeMDIs() {
  foreach(m_mdis)
    if (IsZoomed(m_mdis.getfor()->gethWnd())) remaximizeMDI(m_mdis.getfor());
  endfor;
}

void OSMDIParentWnd::remaximizeMDI(OSMDIChildWnd *child) {
  RECT mcr;
  getMDIClient()->getWindowRect(&mcr);

  int hash = GetWindowLong(getMDIClient()->gethWnd(), GWL_STYLE) & WS_HSCROLL;
  int hasv = GetWindowLong(getMDIClient()->gethWnd(), GWL_STYLE) & WS_VSCROLL;

  if (hash) mcr.bottom -= GetSystemMetrics(SM_CXVSCROLL);
  if (hasv) mcr.right -= GetSystemMetrics(SM_CYVSCROLL);

  RECT cr, wr;
  GetClientRect(child->gethWnd(), &cr); // do NOT make it child->getClientRect(&cr);
  child->clientToScreen(&cr);
  child->getWindowRect(&wr);
  int x = cr.left - wr.left;
  int y = cr.top - wr.top;
  int x2 = wr.right - cr.right;
  int y2 = wr.bottom - cr.bottom;

  SetWindowPos(child->gethWnd(), NULL, -x, -y, mcr.right-mcr.left+x2+x, mcr.bottom-mcr.top+y2+y, SWP_NOACTIVATE|SWP_NOZORDER);
  UpdateWindow(child->gethWnd());
}

void OSMDIParentWnd::maximizeMDIchild(OSMDIChildWnd *child) {
#if 0 // BU> this doesn't always work for some reason
    SendMessage(child->gethWnd(), WM_SYSCOMMAND, SC_MAXIMIZE, 0);
#else
  StdWnd::maximizeWnd(child->gethWnd());	// this always works afaict
  child->onMaximize();	// have to call by hand since using StdWnd
#endif
}

void OSMDIParentWnd::cacheBitmaps() {
  if (m_mdi_buttons[0][0]) DeleteObject(m_mdi_buttons[0][0]);
  if (m_mdi_buttons[1][1]) DeleteObject(m_mdi_buttons[1][1]);
  if (m_mdi_buttons[0][2]) DeleteObject(m_mdi_buttons[0][2]);
  if (m_mdi_buttons[1][0]) DeleteObject(m_mdi_buttons[1][0]);
  if (m_mdi_buttons[0][1]) DeleteObject(m_mdi_buttons[0][1]);
  if (m_mdi_buttons[1][2]) DeleteObject(m_mdi_buttons[1][2]);

  RECT r;

  r.left = 0;
  r.top = 0;
  r.right = GetSystemMetrics(SM_CXMENUSIZE)-2;
  r.bottom= GetSystemMetrics(SM_CYMENUSIZE)-4;

  HDC sysdc = GetDC(NULL);
  m_mdi_buttons[0][0] = CreateCompatibleBitmap(sysdc, r.right, r.bottom);
  m_mdi_buttons[0][1] = CreateCompatibleBitmap(sysdc, r.right, r.bottom);
  m_mdi_buttons[0][2] = CreateCompatibleBitmap(sysdc, r.right, r.bottom);
  m_mdi_buttons[1][0] = CreateCompatibleBitmap(sysdc, r.right, r.bottom);
  m_mdi_buttons[1][1] = CreateCompatibleBitmap(sysdc, r.right, r.bottom);
  m_mdi_buttons[1][2] = CreateCompatibleBitmap(sysdc, r.right, r.bottom);

  HDC dc = CreateCompatibleDC(sysdc);

  HBITMAP oldbmp = (HBITMAP)SelectObject(dc, m_mdi_buttons[0][0]);
  DrawFrameControl(dc, &r, DFC_CAPTION, DFCS_CAPTIONCLOSE);
  SelectObject(dc, m_mdi_buttons[1][0]);
  DrawFrameControl(dc, &r, DFC_CAPTION, DFCS_CAPTIONCLOSE | DFCS_PUSHED);
  SelectObject(dc, m_mdi_buttons[0][1]);
  DrawFrameControl(dc, &r, DFC_CAPTION, DFCS_CAPTIONRESTORE);
  SelectObject(dc, m_mdi_buttons[1][1]);
  DrawFrameControl(dc, &r, DFC_CAPTION, DFCS_CAPTIONRESTORE | DFCS_PUSHED);
  SelectObject(dc, m_mdi_buttons[0][2]);
  DrawFrameControl(dc, &r, DFC_CAPTION, DFCS_CAPTIONMIN);
  SelectObject(dc, m_mdi_buttons[1][2]);
  DrawFrameControl(dc, &r, DFC_CAPTION, DFCS_CAPTIONMIN | DFCS_PUSHED);

  ReleaseDC(NULL, sysdc);

  SelectObject(dc, oldbmp);
  DeleteDC(dc);
}

void OSMDIParentWnd::onMDIChildMaximize(OSMDIChildWnd *child) {
  if (!m_mixedmode) {
    // maximize all the rest
    foreach(m_mdis)
      if (m_mdis.getfor() != child && !m_mdis.getfor()->isMaximized()) {
        // disallow maximize animation
#if 1
        LockWindowUpdate(m_mdis.getfor()->gethWnd());
        ShowWindow(m_mdis.getfor()->gethWnd(), SW_HIDE);
        SendMessage(m_mdis.getfor()->gethWnd(), WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        LockWindowUpdate(NULL);
#else
        StdWnd::maximizeWnd(m_mdis.getfor()->gethWnd());
        m_mdis.getfor()->onMaximize();
#endif
      }
    endfor;
    child->bringToFront();
  }
  updateTitle();
}

void OSMDIParentWnd::onMDIChildRestore(OSMDIChildWnd *child) {
  if (!m_mixedmode) {
    foreach(m_mdis)
      if (m_mdis.getfor() != child && m_mdis.getfor()->isMaximized()) {
        // disallow maximize animation
        LockWindowUpdate(m_mdis.getfor()->gethWnd());
        ShowWindow(m_mdis.getfor()->gethWnd(), SW_HIDE);
        SendMessage(m_mdis.getfor()->gethWnd(), WM_SYSCOMMAND, SC_RESTORE, 0);
        LockWindowUpdate(NULL);
      }
    endfor;
    child->bringToFront();
  }
  updateTitle();
}

int OSMDIParentWnd::numMDIChildMaximized() {
  int r = 0;
  foreach(m_mdis)
    if (m_mdis.getfor()->isMaximized()) r++;
  endfor
  return r;
}

void OSMDIParentWnd::allowMixedMode(int allowed) {
  if (m_mixedmode == allowed) return;
  m_mixedmode = allowed;
  if (!m_mixedmode) {
    if (numMDIChildMaximized()) {	// at least one child maximized
      foreach(m_mdis)
        if (!m_mdis.getfor()->isMaximized())
          SendMessage(m_mdis.getfor()->gethWnd(), WM_SYSCOMMAND, SC_MAXIMIZE, 0);
      endfor;
    }
  }
}

/* lone> disabled for now
int OSMDIParentWnd::timerclient_onDeferredCallback(int param1, int param2) {
  switch (param1) {
    case DEFER_DELETE_CHILD:
      delchildlist.deleteAll();	// it's just that easy
    break;
  }
  return 1;
}*/

void OSMDIParentWnd::updateTitle() {
  if (!m_curchild || !m_curchild->isMaximized() || m_curchild->getName() == NULL) 
    SetWindowText(gethWnd(), getName());
  else 
    SetWindowText(gethWnd(), StringPrintf("%s - [%s]", getName(), m_curchild->getName()));
}

void OSMDIParentWnd::onMDIChildNameChange(OSMDIChildWnd *child) {
  updateTitle(); 
}

int OSMDIParentWnd::onKeyDown(int kc) {
  if (kc == VK_TAB && Std::keyDown(VK_CONTROL)) {
    if (!Std::keyDown(VK_SHIFT)) nextMDI();
    else previousMDI();
  }
  if (m_curchild) {
    return m_curchild->onKeyDown(kc);
  }
  return 0;
}

int OSMDIParentWnd::onKeyUp(int kc) {
  if (m_curchild) {
    return m_curchild->onKeyUp(kc);
  }
  return 0;
}

void *OSMDIParentWnd::getInterface(GUID interfaceGuid) {
  if (interfaceGuid == GUID_OSMDIPARENT_CLASS) return this;
  return OSMDIPARENTWND_PARENT::getInterface(interfaceGuid);
}
