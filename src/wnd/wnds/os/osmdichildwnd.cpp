#include <precomp.h>
#include "osmdichildwnd.h"
#include "osmdiparentwnd.h"

//#define DETACHABLE

#ifdef DETACHABLE
// has to be < 0xf000
#define MDICHILD_SYSMENU_DETACH 0xeffe
#endif

OSMDIChildWnd::OSMDIChildWnd() {
  m_parent = NULL;
  detached = 0;
}

OSMDIChildWnd::~OSMDIChildWnd() {
  if (!detached) {
    if (m_parent) m_parent->onMDIChildDelete(this);
  }
}

int OSMDIChildWnd::onInit() {
  int r = OSMDICHILDWND_PARENT::onInit();

#ifdef DETACHABLE
  HMENU sysm = GetSystemMenu(getOsWindowHandle(), FALSE);
  AppendMenu(sysm, MF_SEPARATOR, -1, NULL);
  AppendMenu(sysm, MF_STRING, MDICHILD_SYSMENU_DETACH, "Detach from main window");
#endif

  return r;
}

void OSMDIChildWnd::setWindowFlags() {
  #ifdef WIN32
  SetWindowLong(gethWnd(), GWL_STYLE, WS_CHILDWINDOW | WS_OVERLAPPED | WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | (isVisible(1) ? WS_VISIBLE : 0));
  SetWindowLong(gethWnd(), GWL_EXSTYLE, WS_EX_MDICHILD | WS_EX_WINDOWEDGE);
  SetWindowRgn(gethWnd(), NULL, TRUE);
  #endif
}

int OSMDIChildWnd::callDefProc(HWND wnd, UINT msg, int wparam, int lparam) {
#ifdef DETACHABLE
  if (detached) return DefWindowProc(wnd, msg, wparam, lparam);
#endif
  return DefMDIChildProc(wnd, msg, wparam, lparam);
}

LRESULT OSMDIChildWnd::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
#ifdef DETACHABLE
  if (detached) {
    if ( uMsg == WM_SYSCOMMAND && wParam == MDICHILD_SYSMENU_DETACH && m_oldparent) {
      attachMDI();
      return 1;
    }       
    return OSMDICHILDWND_PARENT::wndProc(hWnd, uMsg, wParam, lParam);
  }
#endif
  if (uMsg == WM_MOVE) {
    if (isMaximized() && getMDIParent()->isMDIScrolling()) {
      getMDIParent()->remaximizeMDI(this);
    }
  }
  switch (uMsg) {
    case WM_NCACTIVATE:
      if ( wParam == FALSE ) 
        return callDefProc(hWnd, uMsg, wParam, lParam);
    case WM_CHILDACTIVATE: {
      OSMDIParentWnd *p = getMDIParent();
      if (p) p->onMDIChildActivate(this);
      onActivate();
      // NO break
    }
    case WM_GETMINMAXINFO:
    case WM_MENUCHAR:
    case WM_MOVE:
    case WM_SETFOCUS:
      if ( uMsg == WM_SETFOCUS && wantFocus() ) 
        onGetFocus();
    case WM_SIZE: {
//CUT    case WM_SYSCOMMAND:
      int r = callDefProc(hWnd, uMsg, wParam, lParam);
//CUT      if (uMsg == WM_SYSCOMMAND) {
//CUT        switch (wParam & 0xFFF0) {
//CUT          case SC_MAXIMIZE:
//CUT            onMaximize();
//CUT            break;
//CUT          case SC_MINIMIZE:
//CUT            onMinimize();
//CUT            break;
//CUT          case SC_RESTORE:
//CUT            onRestore();
//CUT            break;
//CUT        }
//CUT      }
      return r;
    }

#ifdef DETACHABLE
    case WM_SYSCOMMAND: {
      if (wParam == MDICHILD_SYSMENU_DETACH) {
        detachMDI();
        return 1;
      }
    }
    break;
#endif
    
    case WM_ERASEBKGND: return 1;	// wayyyyy faster -BU
  }
  return OSMDICHILDWND_PARENT::wndProc(hWnd, uMsg, wParam, lParam);
}

OSMDIParentWnd *OSMDIChildWnd::getMDIParent() {
  if (detached) return NULL;
  return m_parent;
}

OSMDIParentWnd *OSMDIChildWnd::getOldMDIParent() {
  return m_oldparent;
}

void OSMDIChildWnd::setMDIParent(OSMDIParentWnd *parent) {
  if (parent == m_parent) return;
  if (m_parent) m_parent->onMDIChildDetach(this);
  m_parent = parent;
  if (m_parent) m_parent->onMDIChildAttach(this);
}

void OSMDIChildWnd::activate() {
#ifdef WIN32
  // slightly different for us
  RootWnd * parent = getParent();
  if ( parent )
    SendMessage(parent->gethWnd(), WM_MDIACTIVATE, (WPARAM)gethWnd(), 0);
#else
#warning port me
#endif
}

void OSMDIChildWnd::onMaximize() {
  OSMDICHILDWND_PARENT::onMaximize();
  if (!detached) {
    getMDIParent()->updateMDIControlButtons(1);
    getMDIParent()->onMDIChildMaximize(this);
  }
}

void OSMDIChildWnd::onMinimize() {
  OSMDICHILDWND_PARENT::onMinimize();
  if (!detached) {
    if (!getMDIParent()->areAllMDIMinimized())
      getMDIParent()->nextMDI();
    else
      getMDIParent()->updateMDIControlButtons(0);
  }
}

void OSMDIChildWnd::onRestore() {
  OSMDICHILDWND_PARENT::onRestore();
  if (!detached) {
    getMDIParent()->updateMDIControlButtons(isMaximized());
    getMDIParent()->onMDIChildRestore(this);
  }
}

void OSMDIChildWnd::deleteDeferred() {
  if (m_parent) m_parent->deleteMDIChildDeferred(this);
  else if (m_oldparent) m_oldparent->deleteMDIChildDeferred(this);
  else {
    ASSERTPR(0, "can't delete deferred without an mdi parent");
  }
}

void OSMDIChildWnd::onSetName() {
  if (getMDIParent() != NULL) getMDIParent()->onMDIChildNameChange(this);
  if (!isMaximized()) SetWindowText(gethWnd(), getName());
}

void OSMDIChildWnd::detachMDI() {
  if (detached) return;
  detached = 1;
  if(m_parent) {
    OSMDIClientWnd * realparent = m_parent->getMDIClient();
    if ( realparent )
      realparent->unregisterRootWndChild(this);
    m_parent->updateMDIControlButtons(isMaximized());
  }
  m_oldparent = m_parent;
  setMDIParent(NULL);
  reparent(NULL);
#ifdef DETACHABLE
  HMENU sysm = GetSystemMenu(getOsWindowHandle(), FALSE);
  UINT currentID = MDICHILD_SYSMENU_DETACH;
  ModifyMenu(sysm,MDICHILD_SYSMENU_DETACH,MF_STRING,(UINT_PTR)currentID,"Attach to main window");
#endif
  //if (getMenu()) SetMenu(gethWnd(), getMenu()); fucko
}

void OSMDIChildWnd::attachMDI(OSMDIParentWnd *mdiparent) {
  if (!detached) return;
  if (!mdiparent) mdiparent = m_oldparent;
  setMDIParent(mdiparent);
  detached = 0;
  reparent(mdiparent->getMDIClient());
#ifdef DETACHABLE
  HMENU sysm = GetSystemMenu(getOsWindowHandle(), FALSE);
  UINT currentID = MDICHILD_SYSMENU_DETACH;
  ModifyMenu(sysm,MDICHILD_SYSMENU_DETACH,MF_STRING,(UINT_PTR)currentID,"Detach from main window");
#endif
  //SetMenu(gethWnd(), NULL); fucko
}

int OSMDIChildWnd::isDetached() {
  return detached;
}

int OSMDIChildWnd::onResize() {
  OSMDICHILDWND_PARENT::onResize();
 
  return 1;
}

int OSMDIChildWnd::onPostedMove() {
  OSMDICHILDWND_PARENT::onPostedMove();
  if (detached) return 1;
  getMDIParent()->onMDIChildMove(this);
  return 1;
}

int OSMDIChildWnd::onKeyDown(int vk) {
  if (vk == VK_F4 && Std::keyDown(VK_CONTROL)) {
    onUserClose();
  } else if (vk == VK_TAB && Std::keyDown(VK_CONTROL)) {
    if (!Std::keyDown(VK_SHIFT)) m_parent->nextMDI();
    else m_parent->previousMDI();
  } else {
    return OSMDICHILDWND_PARENT::onKeyDown(vk);
  }
  return 1;
}

OSMDIChildWndHoster::OSMDIChildWndHoster(BaseWnd *wnd) : mywnd(wnd) {
  ASSERT(mywnd != NULL);
  mywnd->setParent(this);
}

OSMDIChildWndHoster::~OSMDIChildWndHoster() {
  delete mywnd;
  mywnd = NULL;
}

int OSMDIChildWndHoster::onInit() {
  int r = OSMDICHILDWNDHOSTER_PARENT::onInit();

  mywnd->init(this);
  
  return r;
}

int OSMDIChildWndHoster::onResize() {
  int r = OSMDICHILDWNDHOSTER_PARENT::onResize();

  resizeWndToClient(mywnd);

  return r;
}

void OSMDIChildWndHoster::onUserClose() {
  // die
  deleteDeferred();
//CUT  ASSERT(getMDIParent() != NULL);
//CUT  getMDIParent()->deleteMDIChildDeferred(this);
}

