#include <precomp.h>
#include "ostooltip.h"

#ifndef TTM_ADDTOOL
#define TTM_ADDTOOL WM_USER+4
#endif

#ifndef TTM_SETDELAYTIME
#define TTM_SETDELAYTIME WM_USER+3
#endif

#ifndef TTDT_AUTOPOP
#define TTDT_AUTOPOP 2
#endif

#ifndef TTF_SUBCLASS
#define TTF_SUBCLASS 0x0010
#endif

#ifndef TTF_IDISHWND
#define TTF_IDISHWND 0x0001
#endif

OSToolTip::OSToolTip(const char *text) {
  INITCOMMONCONTROLSEX icce = { sizeof(INITCOMMONCONTROLSEX), ICC_WIN95_CLASSES };
  InitCommonControlsEx(&icce); 
  m_text = text;
  setNonBuffered(1);
  setVirtual(0);
}

int OSToolTip::init(RootWnd *parent) {
  setStartHidden(TRUE);
  BaseWnd::init(WASABI_API_APP->main_gethInstance(), parent->gethWnd(), TRUE);
  SetWindowLong(gethWnd(), GWL_STYLE, GetWindowLong(gethWnd(), GWL_STYLE) | TTS_ALWAYSTIP | TTS_NOPREFIX);
  TOOLINFO ti;
  ti.uFlags = TTF_SUBCLASS|TTF_IDISHWND;
  ti.hwnd = gethWnd();
  ti.lpszText = m_text.getNonConstVal();
  ti.cbSize = sizeof(ti);
  ti.uId = (UINT)parent->gethWnd();
  SendMessage(gethWnd(), TTM_ADDTOOL, 0, (int)&ti);
  SendMessage(gethWnd(), TTM_SETDELAYTIME, TTDT_AUTOPOP, -1);
  SendMessage(gethWnd(), TTM_SETMAXTIPWIDTH, 0, (UINT)(WORD)-1);
  return 1;
}

LRESULT OSToolTip::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  return callDefProc(hWnd, uMsg, wParam, lParam);
}

int OSToolTip::setTool(int id, RECT *r, const char *text) {
  if ( getParent() )  {
    TOOLINFO ti;
    ti.cbSize = sizeof(ti);
    ti.hwnd = getParent()->gethWnd();
    ti.uId = (UINT) id;
    ti.lpszText = NULL;
    LRESULT res = SendMessage(gethWnd(), TTM_GETTOOLINFO, 0, (LPARAM)&ti);
    if ( res )  {
      if ( r )  {
        ti.rect = *r;
      }
      if ( text )  {
        ti.lpszText = const_cast<char *>(text);
      }
      res = SendMessage(gethWnd(), TTM_SETTOOLINFO, 0, (LPARAM)&ti);
    }
    else  {
      if ( r && text )  {
        ti.rect = *r;
        ti.lpszText = const_cast<char *>(text);
        ti.uFlags = TTF_SUBCLASS;
        res = SendMessage(gethWnd(), TTM_ADDTOOL, 0, (LPARAM)&ti);
      }
    }
  }
  return 1;
}

int OSToolTip::setToolRect(int id, RECT *r)  {
  if ( getParent() && r )  {
    TOOLINFO ti;
    ti.cbSize = sizeof(ti);
    ti.hwnd = getParent()->gethWnd();
    ti.uId = (UINT) id;
    ti.rect = *r;
    LRESULT res = SendMessage(gethWnd(), TTM_NEWTOOLRECT, 0, (LPARAM)&ti);
  }
  return 1;
}
