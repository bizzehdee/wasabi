#include <precomp.h>
#include <bfc/std_wnd.h>
#include "osmdiclientwnd.h"
#include "osmdiparentwnd.h"

OSWINDOWHANDLE g_mdi = NULL;

OSMDIClientWnd::OSMDIClientWnd(OSMDIParentWnd *parent) {
  m_parentframe = parent;
  setNonBuffered(1);
}

OSMDIClientWnd::~OSMDIClientWnd() {
  g_mdi = NULL;
}

extern LRESULT CALLBACK staticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int OSMDIClientWnd::onInit() {
  OSMDICLIENTWND_PARENT::onInit();
  
#ifdef WIN32
  SetWindowLong(gethWnd(), GWL_STYLE, WS_HSCROLL | WS_VSCROLL | WS_CHILDWINDOW | WS_GROUP | WS_TABSTOP | MDIS_ALLCHILDSTYLES | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | (isVisible(1) ? WS_VISIBLE : 0));
  SetWindowLong(gethWnd(), GWL_EXSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CLIENTEDGE);
#endif

  g_mdi = gethWnd();

  return 1;
}

LRESULT OSMDIClientWnd::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_SETCURSOR: 
      if (LOWORD(lParam) == HTCLIENT) {
        SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
        return 0;
      }
      break;
    case WM_HSCROLL: 
    case WM_VSCROLL: {
      int nScrollCode = (int) LOWORD(wParam); 
      if (nScrollCode == SB_ENDSCROLL) {
        m_parentframe->onMDIEndScroll();
        for (int i=0;i<m_parentframe->getNumMDIChildren();i++)
          UpdateWindow(m_parentframe->enumMDIChild(i)->gethWnd());
        return 0;
      } else {
        m_parentframe->onMDIScroll();
      }
      break;
    }
    case WM_SIZE: {
      return 0; 
    }
  }
  return callDefProc(hWnd, uMsg, wParam, lParam);
}

int OSMDIClientWnd::onResize() {
  OSMDICLIENTWND_PARENT::onResize();
  m_parentframe->updateScrollbars();
  m_parentframe->remaximizeMDIs();
  return 1;
}
