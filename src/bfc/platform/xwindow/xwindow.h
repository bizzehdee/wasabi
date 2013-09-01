#ifndef _WASABI_XWINDOW_H
#define _WASABI_XWINDOW_H

#ifndef WASABI_PLATFORM_XWINDOW
#error this file is only for the XWINDOW (X11) Subplatform
#endif

#ifndef _WASABI_PLATFORM_H
#ifndef _WASABI_XWINDOW_H_ALLOW_INCLUDE // You must know what you are doing to include this file.
#error only include from bfc/platform/platform.h
#endif
#endif

#include "types.h"

#ifdef __cplusplus

class XWindow {
 private:
  static Display *display;
  static XContext context;

 public:
  static Display *getDisplay();
  static int getScreenNum();
  static Window RootWin();
  static Window RootWin(Window wnd);
  static Visual *DefaultVis();
  
  static int usingXinerama();
  static int enumScreen(int i, RECT *screen);
  static int MessageBox(Display *dis, const char *text, const char *title, /*Ignored for now:*/ int flags = 0);

  static int convertEvent( MSG *, XEvent * );
  static void initContextData( Window h );
  static void nukeContextData( Window h );
  static XContext getContext();
  static void setCursor( Window h, int cursor );
};

#endif

enum contextdata {
  GWL_HINSTANCE = 0,
  GWL_HWND,
  GWL_CLASSNAME,
  GWL_USERDATA,
  GWL_INVALIDREGION,
  GWL_RECT_LEFT,
  GWL_RECT_TOP,
  GWL_RECT_RIGHT,
  GWL_RECT_BOTTOM,
  //CUT GWL_PARENT,

  GWL_ENUM_SIZE
};

//CUT void MoveWindowRect( OSWINDOWHANDLE, int, int );
//CUT void SetWindowRect( OSWINDOWHANDLE, RECT * );
//CUT int GetWindowRect( OSWINDOWHANDLE, RECT * ); // moved to StdWnd
void SetWindowLong( OSWINDOWHANDLE, enum contextdata, LONG );
LONG GetWindowLong( OSWINDOWHANDLE, enum contextdata );
int GetClassName( OSWINDOWHANDLE hWnd, char *lpClassName, int nMaxCount );
//int GetUpdateRect( OSWINDOWHANDLE, RECT *, BOOL ); // moved to StdWnd
//void GetUpdateRgn( OSWINDOWHANDLE, HRGN, BOOL ); // moved to StdWnd
void InvalidateRgn( OSWINDOWHANDLE, OSREGIONHANDLE, BOOL );
void InvalidateRect( OSWINDOWHANDLE, const RECT *, BOOL );
void ValidateRgn( OSWINDOWHANDLE, OSREGIONHANDLE );
void ValidateRect( OSWINDOWHANDLE, const RECT * );
//OSWINDOWHANDLE WindowFromPoint( POINT p ); // Unused. Add to StdWnd if needed.

void TranslateMessage( MSG * );
void PostMessage( OSWINDOWHANDLE, UINT, WPARAM, LPARAM );
void PostQuitMessage( int );

#endif //_WASABI_XWINDOW_H
