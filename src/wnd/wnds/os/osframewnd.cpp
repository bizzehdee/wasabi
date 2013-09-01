#include <precomp.h>
#include <bfc/std_wnd.h>
#include <wnd/popup.h>
#include <wnd/region.h>
#include "osframewnd.h"
#include "commandcb.h"

OSFrameWnd::OSFrameWnd() {
  setVirtual(0);
  m_cc = NULL;
  #ifdef WIN32
  everbeenvisible = 0;
  m_menubar = NULL;
  m_icon_small = NULL;
  m_icon_big = NULL;
  #endif
  allowDocking();
  m_statusbarwnd = NULL;
  m_statusbar = 0;
}

OSFrameWnd::~OSFrameWnd() {

  if (m_statusbarwnd) {
    delete m_statusbarwnd;
    m_statusbarwnd = NULL;
  }
  #ifdef WIN32
  if (m_menubar) DestroyMenu(m_menubar);
  if (m_icon_small) DestroyIcon(m_icon_small); 
  if (m_icon_big) DestroyIcon(m_icon_big); 
  #endif
  
  m_menulist.deleteAll();
}

int OSFrameWnd::onInit() {
  OSFRAMEWND_PARENT::onInit();
  
  setWindowFlags();

  if (m_icon_small) SendMessage(gethWnd(), WM_SETICON, ICON_SMALL, (int)m_icon_small);
  if (m_icon_big) SendMessage(gethWnd(), WM_SETICON, ICON_BIG, (int)m_icon_big);

  updateStatusBarVisibility();
  updateMenu(GetMenu(gethWnd()));

  return 1;
}

void OSFrameWnd::setWindowFlags() {
#ifdef WIN32
  SetWindowLong(gethWnd(), GWL_STYLE, (GetWindowLong(gethWnd(), GWL_STYLE) | WS_OVERLAPPEDWINDOW | WS_OVERLAPPED | WS_CLIPCHILDREN | WS_CLIPSIBLINGS) & ~WS_POPUP);
  SetWindowLong(gethWnd(), GWL_EXSTYLE, (GetWindowLong(gethWnd(), GWL_EXSTYLE) | WS_EX_APPWINDOW) & ~WS_EX_TOOLWINDOW );
  SetWindowRgn(gethWnd(), NULL, TRUE);
#endif
}

LRESULT OSFrameWnd::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (uMsg == WM_CLOSE) {
    onUserClose();
    return 0;
  }
  if (uMsg == WM_INITMENUPOPUP) {
    if (m_cc) updateMenu((HMENU)wParam);
  }
  if (uMsg == WM_ENTERMENULOOP) {
    if (m_cc) updateMenu(GetMenu(gethWnd()));
  }
  if (uMsg >= WM_NCCREATE && uMsg <= WM_NCMBUTTONDBLCLK) return callDefProc(hWnd, uMsg, wParam, lParam);
  if (uMsg == WM_SETCURSOR) {
    if (LOWORD(lParam) != HTCLIENT) 
      return callDefProc(hWnd, uMsg, wParam, lParam);
  }
  if (uMsg == WM_COMMAND) {
    if (HIWORD(wParam) == 0) {
      if (LOWORD(wParam) > 0xF000)
        onMenuSysCommand(LOWORD(wParam));
      else
        onMenuCommand(LOWORD(wParam)-1);
    }
  }
  if (uMsg == WM_GETICON) {
    if (wParam == ICON_BIG && m_icon_big) return (int)m_icon_big;
    if (wParam == ICON_SMALL && m_icon_small) return (int)m_icon_small;
  }
  //if (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP) return callDefProc(hWnd, uMsg, wParam, lParam);
  if (uMsg == WM_GETMINMAXINFO) {
    if (getMinFrameWidth() != -1 && getMinFrameHeight() != -1) {
      MINMAXINFO *mmi = (MINMAXINFO*)lParam;
      mmi->ptMinTrackSize.x = getMinFrameWidth();
      mmi->ptMinTrackSize.y = getMinFrameHeight();
      return 0;
    }
  }
  if (uMsg == WM_DRAWITEM) {
    LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
    OSStatusBarWnd * sb = getStatusBarWnd();
    if ( lpdis && sb && sb->gethWnd() == lpdis->hwndItem )
      sb->renderPart(lpdis->itemID,lpdis->hDC,&lpdis->rcItem,lpdis->itemData);
  }

  return OSFRAMEWND_PARENT::wndProc(hWnd, uMsg, wParam, lParam);
}

void OSFrameWnd::addMenu(const char *title, int pos, PopupMenu *menu, int deleteondestroy) {
#ifdef WIN32
  if (!m_menubar) {
    m_menubar = CreateMenu();
  }
  SetMenu(gethWnd(), NULL);
  InsertMenu(m_menubar, pos, MF_BYPOSITION|MF_POPUP, (int)menu->getOSMenuHandle(), title);
  resetMenu();
  onMenuBarChanged();
#endif
  if (deleteondestroy) m_menulist.addItem(menu);
}

void OSFrameWnd::setIcon(int resourceid) {
  #ifdef WIN32
  if (m_icon_small) { DestroyIcon(m_icon_small); m_icon_small = NULL; }
  if (m_icon_big) { DestroyIcon(m_icon_big); m_icon_big = NULL; }
  m_icon_small = (HICON) LoadImage(WASABI_API_APP->main_gethInstance(), MAKEINTRESOURCE(resourceid), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
  m_icon_big = (HICON) LoadImage(WASABI_API_APP->main_gethInstance(), MAKEINTRESOURCE(resourceid), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
  if (isInited()) {
    SendMessage(gethWnd(), WM_SETICON, ICON_SMALL, (int)m_icon_small);
    SendMessage(gethWnd(), WM_SETICON, ICON_BIG, (int)m_icon_big);
  }
  #endif
}

void OSFrameWnd::resetMenu() {
  SetMenu(gethWnd(), m_menubar);
  if (isPostOnInit()) onResize();
}

OSMENUHANDLE OSFrameWnd::getMenu() {
  return m_menubar;
}

int OSFrameWnd::onMenuCommand(int cmd) {
  if (m_cc) return m_cc->onCommand(cmd);
  return 0;
}

void OSFrameWnd::onMenuSysCommand(int cmd) {
  SendMessage(gethWnd(), WM_SYSCOMMAND, cmd, 0);
}

void OSFrameWnd::setVisible(int show) {
  OSFRAMEWND_PARENT::setVisible(show);
  if (!isPostOnInit()) return;
  if (show && !everbeenvisible) { everbeenvisible = 1; onResize(); }
}

void OSFrameWnd::setStatusBar(int enabled) {
  if (enabled == m_statusbar) return;
  m_statusbar = enabled;
  m_statusbarvisible = enabled;
  if (isInited()) updateStatusBarVisibility();
}

void OSFrameWnd::showStatusBar(int show) {
  if (show == m_statusbarvisible) return;
  m_statusbarvisible = show;
  if (isInited()) updateStatusBarVisibility();
}

void OSFrameWnd::updateStatusBarVisibility() {
  if (m_statusbar) {
    if (m_statusbarwnd)  {
      m_statusbarwnd->setVisible(m_statusbarvisible);
      if (isPostOnInit()) onResize();
      return;
    }
    m_statusbarwnd = new OSStatusBarWnd();
    m_statusbarwnd->setParent(this);
    m_statusbarwnd->setVisible(m_statusbarvisible);
    m_statusbarwnd->init(this);
  } else {
    if (!m_statusbarwnd) return;
    delete m_statusbarwnd;
    m_statusbarwnd = NULL;
  }
  if (isPostOnInit()) onResize();
}

int OSFrameWnd::getBottomOffset() {
  int o = OSFRAMEWND_PARENT::getBottomOffset();
  if (m_statusbarwnd && m_statusbarvisible) {
    o += 20;
  }
  return o;
}

int OSFrameWnd::onResize() {
  OSFRAMEWND_PARENT::onResize();
  if (m_statusbarwnd && m_statusbarvisible) {
    RECT rect;
    BaseWnd::getClientRect(&rect);
    m_statusbarwnd->resize(rect.left, rect.bottom-20, rect.right-rect.left, 20);
  }
  return 1;
}

void OSFrameWnd::updateMenu(HMENU menu, int ismenubar) {
  if (menu == NULL) return;
  if (!m_cc) return;

  int n = GetMenuItemCount(menu);
  for (int i=0;i<n;i++) {
    MENUITEMINFO mii;
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STATE|MIIM_SUBMENU|MIIM_ID;
    GetMenuItemInfo(menu, i, TRUE, &mii);
    if (mii.hSubMenu) {
      PopupMenu *pmenu = PopupMenu::findMenuByOSHandle(mii.hSubMenu);
      if (pmenu) {
        int disable = !m_cc->onGetEnabled(pmenu->getMenuCommandCallbackId(), !(mii.fState & MF_GRAYED));
        EnableMenuItem(menu, i, MF_BYPOSITION|(disable?MF_GRAYED:MF_ENABLED));
        if (!ismenubar) {
          int checked = m_cc->onGetChecked(pmenu->getMenuCommandCallbackId(), (mii.fState & MF_CHECKED));
          CheckMenuItem(menu, i, MF_BYPOSITION|(checked?MF_CHECKED:MF_UNCHECKED));
          if (!disable) 
            updateMenu(mii.hSubMenu);
        } 
      }
    } else {
      int disable = !m_cc->onGetEnabled(mii.wID-1, !(mii.fState & MF_GRAYED));
      EnableMenuItem(menu, i, MF_BYPOSITION|(disable?MF_GRAYED:MF_ENABLED));
      if (!ismenubar) {
        int checked = m_cc->onGetChecked(mii.wID-1, (mii.fState & MF_CHECKED));
        CheckMenuItem(menu, i, MF_BYPOSITION|(checked?MF_CHECKED:MF_UNCHECKED));
      }
    }
  }
}

void OSFrameWnd::onMenuBarChanged() {
  if (isInited()) {
    updateMenu(GetMenu(gethWnd()), 1);
    DrawMenuBar(gethWnd());
  }
}
