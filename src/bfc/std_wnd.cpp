#include "precomp.h"
#ifdef WASABI_COMPILE_UTF
#include <bfc/string/encodedstr.h>
#endif
#include "std_wnd.h"
#include "rect.h"
#include "wnd/gwl64.h"

#ifdef WASABI_PLATFORM_WIN32
#include <multimon.h>
#endif

#ifdef WASABI_PLATFORM_XWINDOW
#include <X11/Xlib.h>
#endif

#ifndef WS_EX_LAYERED
const int WS_EX_LAYERED=0x80000;
#endif
#ifndef LWA_ALPHA
const int LWA_ALPHA=2;
#endif
#ifndef AC_SRC_ALPHA
const int AC_SRC_ALPHA=1;
#endif
#ifndef LWA_COLORKEY
const int LWA_COLORKEY=1;
#endif
#ifndef ULW_ALPHA
const int ULW_ALPHA=2;
#endif

#ifndef SPI_GETWHEELSCROLLLINES
#  define SPI_GETWHEELSCROLLLINES  104
#endif

#ifndef SPI_GETLISTBOXSMOOTHSCROLLING
#  define SPI_GETLISTBOXSMOOTHSCROLLING  0x1006
#endif

#ifdef WIN32
static int nreal=0;

static HINSTANCE gdi32instance=NULL;
static HINSTANCE user32instance=NULL;

static void (__stdcall *setLayeredWindowAttributes)(HWND, int, int, int);
static BOOL (__stdcall *updateLayeredWindow)(
  HWND hwnd,             // handle to layered window
  HDC hdcDst,            // handle to screen DC
  POINT *pptDst,         // new screen position
  SIZE *psize,           // new size of the layered window
  HDC hdcSrc,            // handle to surface DC
  POINT *pptSrc,         // layer position
  COLORREF crKey,        // color key
  BLENDFUNCTION *pblend, // blend function
  DWORD dwFlags          // options
  );
static int (WINAPI *getRandomRgnFn)(HDC dc, HRGN rgn, int i)=NULL;
#if(_WIN32_WINNT < 0x0500) //only time this struct wont be here
typedef struct tagLASTINPUTINFO {
    UINT cbSize;
    DWORD dwTime;
} LASTINPUTINFO, *PLASTINPUTINFO;
#endif
static BOOL (WINAPI *getLastInputInfoFn)(PLASTINPUTINFO)=NULL;

static int checked_for_alpha_proc;
static int grrfailed=0, glii_failed=0;

static void register_wndClass(HINSTANCE hInstance, const char *classname, WNDPROC wndproc, int want_dclicks=FALSE);
#endif // WIN32

#if defined(WASABI_PLATFORM_WIN32)
OSWINDOWHANDLE StdWnd::createWnd(int x, int y, int w, int h, int nochild,
  int acceptdrops, OSWINDOWHANDLE parent, OSMODULEHANDLE module,
  const char *classname, WNDPROC wndproc, unsigned long userdata, int want_dclick, int alpha) {

  register_wndClass(module, classname, wndproc, want_dclick);

  int style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
  int exstyle=0;
  if (parent == NULL) {
    style |= WS_POPUP;
  } else
    style |= WS_CHILD;

  if (nochild) {
    style=WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    //WS_POPUP doesnt erase the stupid lil taskbar button
    //need to do this to fix
    exstyle |= WS_EX_TOOLWINDOW;
  }

  if (acceptdrops) exstyle |= WS_EX_ACCEPTFILES;
  if (alpha != 255) exstyle |= WS_EX_LAYERED;

  HWND ret = CreateWindowEx(exstyle, classname, NULL, style,
    x, y, w, h, parent, NULL, module, (LPVOID)userdata);
  if (ret != INVALIDOSWINDOWHANDLE) {
    nreal++;
    setLayeredAlpha(ret, alpha);
  } else {
    int feh = GetLastError();
//    __asm int 3;
    DebugBreak();
  }
  return ret;
}
#elif defined(WASABI_PLATFORM_XWINDOW)
OSWINDOWHANDLE StdWnd::createWnd(int x, int y, int w, int h, int nochild,
  int acceptdrops, OSWINDOWHANDLE parent, OSMODULEHANDLE module,
  const char *classname, unsigned long userdata) {
  
  Window origp = parent;

  if ( parent == None || nochild )
    parent = XWindow::RootWin();

  Window win = XCreateSimpleWindow( XWindow::getDisplay(), parent, 
                                    x, y, w, h, 0,
                                    WhitePixel( XWindow::getDisplay(), XWindow::getScreenNum() ),
                                    BlackPixel( XWindow::getDisplay(), XWindow::getScreenNum() ) );

  ASSERT( win != None );

  XWindow::initContextData( win );
  SetWindowLong( win, GWL_HINSTANCE, (_GWL_LONGTYPE)module );
  SetWindowLong( win, GWL_USERDATA, (_GWL_LONGTYPE)userdata );
  SetWindowLong( win, GWL_CLASSNAME, (_GWL_LONGTYPE)classname );
  //CUT SETWINDOWLONG( win, GWL_PARENT, (_GWL_LONGTYPE)parent );

  RECT r = { x, y, x+w, y+h };
  //CUT SetWindowRect( win, &r );

  XSetWindowAttributes xwa;
  xwa.background_pixmap = None;
  xwa.backing_store = WhenMapped;
  //xwa.override_redirect = True;
  XChangeWindowAttributes( XWindow::getDisplay(), win, CWBackPixmap|CWBackingStore, &xwa );

  typedef struct {
    long flags;
    long functions;
    long decorations;
    long inputMode;
    long unknown;
  } MWM_Hints;
  Atom WM_HINTS;

  // Nue the border
  WM_HINTS = XInternAtom(XWindow::getDisplay(), "_MOTIF_WM_HINTS", True);
  if ( WM_HINTS != None ) { 
#   define    MWM_HINTS_DECORATIONS    (1L << 1)
    MWM_Hints MWMHints = { MWM_HINTS_DECORATIONS, 0, 0, 0, 0 };
    
    XChangeProperty(XWindow::getDisplay(), win, WM_HINTS, WM_HINTS, 32,
                    PropModeReplace, (unsigned char *)&MWMHints,
                    sizeof(MWMHints)/4);
  }  
  WM_HINTS = XInternAtom(XWindow::getDisplay(), "KWM_WIN_DECORATION", True);
  if ( WM_HINTS != None ) { 
    long KWMHints = 0;
    
    XChangeProperty(XWindow::getDisplay(), win, WM_HINTS, WM_HINTS, 32,
                    PropModeReplace, (unsigned char *)&KWMHints,
                    sizeof(KWMHints)/4);
  }
#define WIN_HINTS_SKIP_TASKBAR   (1 << 2)
#define WIN_HINTS_SKIP_WINLIST   (1 << 1)
  WM_HINTS = XInternAtom(XWindow::getDisplay(), "_WIN_HINTS", True);
  if ( WM_HINTS != None ) { 
    long GNOMEHints = 0;
    
    XChangeProperty(XWindow::getDisplay(), win, WM_HINTS, WM_HINTS, 32,
                    PropModeReplace, (unsigned char *)&GNOMEHints,
                    sizeof(GNOMEHints)/4);
  }
#define WIN_STATE_FIXED_POSITION (1L << 8)
#define WIN_STATE_ARRANGE_IGNORE (1L << 9)
  XClientMessageEvent e;
  
  e.type = ClientMessage;
  e.window = win;
  e.message_type = XInternAtom( XWindow::getDisplay(), "_WIN_STATE", True );
  e.format = 32;
  e.data.l[0] = WIN_STATE_ARRANGE_IGNORE; 

  XSendEvent(XWindow::getDisplay(), XWindow::RootWin(), False, 
             SubstructureNotifyMask, (XEvent *)&e );

  if(acceptdrops) {
    Atom DND = XInternAtom( XWindow::getDisplay(), "XdndAware", False );
    if ( DND ) {
      Atom version = 3;
      XChangeProperty( XWindow::getDisplay(), win, DND, XA_ATOM, 32,
                      PropModeReplace, (unsigned char *)&version, 1 );
    }
  }

  XSelectInput( XWindow::getDisplay(), win, ALL_EVENTS );

  if ( nochild && origp ) {
    /*
      XWMHints *xwmh = XAllocWMHints();
      xwmh->window_group = origp;
      xwmh->flags = WindowGroupHint;
      XSetWMHints( XWindow::getDisplay(), win, xwmh );
      XFree( xwmh );
    */

    XSetTransientForHint( XWindow::getDisplay(), win, origp );
  } else if ( ! origp ) {
    Atom NET_STATE = XInternAtom( XWindow::getDisplay(), "_NET_WM_STATE", False );
    Atom TASKBAR = XInternAtom( XWindow::getDisplay(), "_NET_WM_STATE_SKIP_TASKBAR", False );
    if ( NET_STATE && TASKBAR ) {
      XChangeProperty( XWindow::getDisplay(), win, NET_STATE, XA_ATOM, 32,
                       PropModeReplace, (unsigned char *)&TASKBAR, 1 );
    }
  }

  return win;
}
#else
#error port me!
#endif // OS

void StdWnd::destroyWnd(OSWINDOWHANDLE wnd) {
#ifdef WIN32
  DestroyWindow(wnd);
  nreal--;
  if (nreal == 0) {
    if (gdi32instance) FreeLibrary(gdi32instance);
    if (user32instance) FreeLibrary(user32instance);

    gdi32instance = NULL;
    user32instance = NULL;
    getRandomRgnFn = NULL;
    getLastInputInfoFn = NULL;
  }
#elif defined(XWINDOW)
  XWindow::nukeContextData( wnd );
  XDestroyWindow( XWindow::getDisplay(), wnd );
#else
#error port me!
#endif // platform
}

int StdWnd::isValidWnd(OSWINDOWHANDLE wnd) {
#ifdef WIN32
  return IsWindow(wnd);
#elif defined(XWINDOW)
  // XGetGeometry will return 0 if wnd does not exist.
  Window root;
  int x, y;
  unsigned int w, h, bw, d;
  return XGetGeometry(XWindow::getDisplay(), wnd, &root, &x, &y, &w, &h, &bw, &d);
#else
#error port me!
#endif // platform
}

void StdWnd::setLayeredWnd(OSWINDOWHANDLE wnd, int layered) {
#if defined(WIN32)
  // have to clear and reset, can't just set
  SETWINDOWLONG(wnd, GWL_EXSTYLE, GETWINDOWLONG(wnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
  if (layered)
    SETWINDOWLONG(wnd, GWL_EXSTYLE, GETWINDOWLONG(wnd, GWL_EXSTYLE) | WS_EX_LAYERED);
#else
#warning No layered alpha support.
#endif
}

int StdWnd::isLayeredWnd(OSWINDOWHANDLE wnd) {
#if defined(WIN32)
  DWORD dwLong = (DWORD)GETWINDOWLONG(wnd, GWL_EXSTYLE);
  return !!(dwLong & WS_EX_LAYERED);
#else
#warning No layered alpha support.
#endif
}

void StdWnd::setLayeredAlpha(OSWINDOWHANDLE wnd, int amount) {
  if (!isDesktopAlphaAvailable()) return;
#if defined(WIN32)  
  if (amount < 0) amount = 0;
  else if (amount > 255) amount = 255;
  if (amount != 255) {
    if (!isLayeredWnd(wnd)) setLayeredWnd(wnd, TRUE);
  }
  if (setLayeredWindowAttributes != NULL)
    (*setLayeredWindowAttributes)(wnd, RGB(0,0,0), amount, LWA_ALPHA); 
  if (amount == 255) {
    if (isLayeredWnd(wnd)) setLayeredWnd(wnd, FALSE);
  }
#else
#warning No layered alpha support.
#endif
}

void StdWnd::updateLayeredWnd(OSWINDOWHANDLE wnd, int x, int y, int w, int h, HDC surfdc, int alpha) {
  if (!isDesktopAlphaAvailable()) return;
#if defined(WIN32)
  BLENDFUNCTION blend= {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
  POINT sp={x,y}, pt={0,0};
  SIZE ss = { w, h };
  HDC sysdc = GetDC(NULL);
  (*updateLayeredWindow)(wnd, sysdc, &sp, &ss, surfdc, &pt, 0, &blend, ULW_ALPHA);
  ReleaseDC(NULL, sysdc);
#else
#warning No layered alpha support.
#endif
}

void StdWnd::setWndPos(OSWINDOWHANDLE wnd, OSWINDOWHANDLE zorder,
  int x, int y, int w, int h,
  int nozorder, int noactive, int nocopybits, int nomove, int noresize) {
#ifdef WIN32
  SetWindowPos(wnd, zorder, x, y, w, h,
    SWP_DEFERERASE |		// we ignore WM_SYNCPAINT anyway
    (nozorder ? SWP_NOZORDER : 0) |
    (noactive ? SWP_NOACTIVATE : 0) |
    (nocopybits ? SWP_NOCOPYBITS : 0) |
    (nomove ? SWP_NOMOVE : 0) |
    (noresize ? SWP_NOSIZE : 0) );
#elif defined (XWINDOW)
  //TODO deal with nocopybits?
  
  XSizeHints xsh;
  xsh.flags = (nomove ? 0 : PPosition) | (noresize ? 0 : PSize);
  xsh.x = x;
  xsh.y = y;
  xsh.base_width = w;
  xsh.base_height = h;
  XSetWMNormalHints( XWindow::getDisplay(), wnd, &xsh );

  XWindowChanges xwc;
  xwc.x = x; xwc.y = y; xwc.width = w; xwc.height = h;
  xwc.sibling = zorder;
  xwc.stack_mode = Above;
  XConfigureWindow( XWindow::getDisplay(), wnd,
      (nomove ? 0 : CWX | CWY) |
      (noresize ? 0 : CWWidth | CWHeight) |
      (nozorder ? 0 : CWSibling | CWStackMode),
      &xwc );
  if(!noactive)
    XRaiseWindow(XWindow::getDisplay(), wnd);
  XFlush(XWindow::getDisplay());
#else
#error port me!
#endif // platform
}

void StdWnd::getWndPos(OSWINDOWHANDLE wnd, int *x, int *y) {
  RECT r;
  StdWnd::getWindowRect(wnd, &r);
  Rect wr(r);
  if (x != NULL) *x = wr.left;
  if (y != NULL) *y = wr.top;
#if !(defined(WIN32) || defined(XWINDOW))
#warning StdWnd::getWndPos() depends on StdWnd::getWindowRect().
#endif // platform
}

void StdWnd::bringToFront(OSWINDOWHANDLE wnd) {
#ifdef WIN32
  if (isPopup(wnd))
    SetForegroundWindow(wnd);
  else
    SetWindowPos(wnd, HWND_TOP, 0, 0, 0, 0,
      SWP_NOMOVE|SWP_NOSIZE|SWP_DEFERERASE|SWP_NOOWNERZORDER);
#elif defined(XWINDOW)
  XRaiseWindow(XWindow::getDisplay(), wnd);
  XFlush(XWindow::getDisplay());
#else
#error port me!
#endif // platform
}

void StdWnd::sendToBack(OSWINDOWHANDLE wnd) {
#ifdef WIN32
  SetWindowPos(wnd, HWND_BOTTOM, 0, 0, 0, 0,
    SWP_NOMOVE|SWP_NOSIZE|SWP_DEFERERASE|SWP_NOOWNERZORDER);
#elif defined(XWINDOW)
  XLowerWindow(XWindow::getDisplay(), wnd);
  XFlush(XWindow::getDisplay());
#else
#error port me!
#endif // platform
}

int StdWnd::isWndVisible(OSWINDOWHANDLE wnd) {
#ifdef WIN32
  return IsWindowVisible(wnd);
#elif defined(XWINDOW)
  XWindowAttributes xwa;
  XGetWindowAttributes(XWindow::getDisplay(), wnd, &xwa);
  return xwa.map_state != IsUnmapped;
#else
#error port me!
#endif // platform
}

void StdWnd::showWnd(OSWINDOWHANDLE wnd, int noactivate) {
#ifdef WIN32
// BU was SHOWNORMAL but that's only for app startup
  ShowWindow(wnd, noactivate ? SW_SHOWNA : SW_SHOW);
#elif defined(XWINDOW)
  if(noactivate)
    XMapWindow(XWindow::getDisplay(), wnd); //just maps window.
  else
    XMapRaised(XWindow::getDisplay(), wnd); //maps and brings to top of stack.
  XFlush(XWindow::getDisplay());
#else
#error port me!
#endif // platform
}

void StdWnd::hideWnd(OSWINDOWHANDLE wnd) {
#ifdef WIN32
  ShowWindow(wnd, SW_HIDE);
#elif defined(XWINDOW)
  XLowerWindow(XWindow::getDisplay(), wnd);
  XFlush(XWindow::getDisplay());
#else
#error port me!
#endif // platform
}

void StdWnd::minimizeWnd(OSWINDOWHANDLE wnd) {
#ifdef WIN32
  SendMessage(wnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
#elif defined(XWINDOW)
  XIconifyWindow(XWindow::getDisplay(), wnd, XWindow::getScreenNum());
  XFlush(XWindow::getDisplay());
#else
#error port me!
#endif // platform
}

void StdWnd::maximizeWnd(OSWINDOWHANDLE wnd) {
#ifdef WIN32
//  SendMessage(wnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
  ShowWindow(wnd, SW_MAXIMIZE);	// this works better
#elif defined(XWINDOW)
#warning port me
#else
#error port me!
#endif // platform
}

void StdWnd::restoreWnd(OSWINDOWHANDLE wnd) {
#ifdef WIN32
//  SendMessage(wnd, WM_SYSCOMMAND, SC_RESTORE, 0);
  ShowWindow(wnd, SW_RESTORE);
#elif defined(XWINDOW)
  XMapWindow(XWindow::getDisplay(), wnd);
  XFlush(XWindow::getDisplay());
#else
#error port me!
#endif // platform
}

int StdWnd::isPopup(OSWINDOWHANDLE wnd) {
#if defined(WIN32)
//CUT  return !!(GetWindowLong(wnd, GWL_STYLE) & WS_POPUP);
  return (GetParent(wnd) == NULL);
#elif defined(XWINDOW)
  return StdWnd::getParent(wnd) == XWindow::RootWin(wnd);
#else
#error port me!
#endif // platform
}

void StdWnd::setEnabled(OSWINDOWHANDLE wnd, int enabled) {
#ifdef WIN32
  EnableWindow(wnd, enabled);
#elif defined(XWINDOW)
  long event_mask = 0;
  if(enabled) event_mask = ALL_EVENTS;
  else event_mask = NO_INPUT_EVENTS;
  XSelectInput(XWindow::getDisplay(), wnd, event_mask);
#else
#error port me!
#endif // platform
}

void StdWnd::setFocus(OSWINDOWHANDLE wnd) {
#ifdef WIN32
  SetFocus(wnd);
#elif defined(XWINDOW)
  XSetInputFocus( XWindow::getDisplay(), wnd, RevertToPointerRoot, CurrentTime );
#else
#error port me!
#endif // platform
}

OSWINDOWHANDLE StdWnd::getFocus() {
#ifdef WIN32
  return GetFocus();
#elif defined(XWINDOW)
  Window wnd; int revert;
  XGetInputFocus( XWindow::getDisplay(), &wnd, &revert);
  return wnd;
#else
#error port me!
#endif // platform
}

void StdWnd::setTopmost(OSWINDOWHANDLE wnd, int topmost) {
#ifdef WIN32
  SetWindowPos(wnd, topmost ? HWND_TOPMOST : HWND_NOTOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOOWNERZORDER);
#elif defined(XWINDOW)
  Atom WM_STATE = XInternAtom(XWindow::getDisplay(), "_NET_WM_STATE", False);
  Atom WM_STATE_ABOVE = XInternAtom(XWindow::getDisplay(), "_NET_WM_STATE_ABOVE", False);
  XEvent e;
  e.xclient.type = ClientMessage;
  e.xclient.window = wnd;
  e.xclient.message_type = WM_STATE;
  e.xclient.format = 32;
  e.xclient.data.l[0] = (long)(topmost ? /*_NET_WM_STATE_ADD*/ 1 : /*_NET_WM_STATE_REMOVE*/ 0);
  e.xclient.data.l[1] = (long)WM_STATE_ABOVE;
  e.xclient.data.l[2] = (long)0;
  e.xclient.data.l[3] = (long)0;
  e.xclient.data.l[4] = (long)0;
  
  XSendEvent(XWindow::getDisplay(), XWindow::RootWin(wnd), False, SubstructureNotifyMask|SubstructureRedirectMask, &e);
  XFlush(XWindow::getDisplay());
#else
#error port me!
#endif // platform
}

void StdWnd::setParent(OSWINDOWHANDLE child, OSWINDOWHANDLE newparent) {
#if defined(WIN32)
  SetParent(child, newparent);
#elif defined(XWINDOW)
  // FUCKO:  right coords?? (retains parent-relative coords atm)
  if (newparent == None) newparent = XWindow::RootWin(child);
  Window root;
  int x, y;
  unsigned int w, h, bw, d;
  XGetGeometry(XWindow::getDisplay(), child, &root, &x, &y, &w, &h, &bw, &d);
  XReparentWindow(XWindow::getDisplay(), child, newparent, x, y);
  XFlush(XWindow::getDisplay());
#else
#error port me!
#endif
}

OSWINDOWHANDLE StdWnd::getParent(OSWINDOWHANDLE wnd) {
#if defined(WIN32)
  return GetParent(wnd);
#elif defined(XWINDOW)
  Window root, parent, *children;
  unsigned int n;
  XQueryTree(XWindow::getDisplay(), wnd, &root, &parent, &children, &n);
  //? if (parent == XWindow::RootWin(wnd)) return INVALIDOSWINDOWHANDLE;
  return parent;
#else
#error port me!
#endif // platform
}

OSWINDOWHANDLE StdWnd::getTopmostChild(OSWINDOWHANDLE wnd) {
#if defined(WIN32)
  return GetWindow(wnd, GW_CHILD);
#elif defined(XWINDOW)
  Window root, parent, *children;
  unsigned int n;
  XQueryTree(XWindow::getDisplay(), wnd, &root, &parent, &children, &n);
  if( children ) return children[n-1];
  return INVALIDOSWINDOWHANDLE;
#else
#error port me!
#endif // platform
}

DWORD StdWnd::getOwningProcess(OSWINDOWHANDLE wnd) {
#if defined(WASABI_PLATFORM_WIN32)
  DWORD pid=0;
  GetWindowThreadProcessId(wnd, &pid);
  return pid;
#else
//hint here maybe:
//http://lists.freedesktop.org/archives/xorg/2005-March/007186.html
//this is not something X is designed to do, it seems.
#warning port me!
#endif
}

void StdWnd::invalidateRect(OSWINDOWHANDLE wnd, RECT *r) {
#if defined(WIN32)
  OSREGIONHANDLE reg = NULL;
  if (r == NULL) {
    RECT cr;
    StdWnd::getClientRect(wnd, &cr);
    reg = CreateRectRgnIndirect(&cr);
  } else reg = CreateRectRgnIndirect(r);
  invalidateRegion(wnd, reg);
  DeleteObject(reg);
#elif defined(XWINDOW)
  OSREGIONHANDLE reg = NULL;
  if (r == NULL) {
    RECT cr;
    StdWnd::getClientRect(wnd, &cr);
    reg = XCreateRegion();
#if defined(WASABI_WIN32RECT)
    XRectangle xcr;
    xcr.x = cr.left; xcr.y = cr.top;
    xcr.width = cr.right - cr.left;
    xcr.height = cr.bottom - cr.top;
    XUnionRectWithRegion(&xcr,reg,reg);
#else    
    XUnionRectWithRegion(&cr,reg,reg);
#endif
  } else {
  reg = XCreateRegion();
#if defined(WASABI_WIN32RECT)
    XRectangle xcr;
    xcr.x = r->left; xcr.y = r->top;
    xcr.width = r->right - r->left;
    xcr.height = r->bottom - r->top;
    XUnionRectWithRegion(&xcr,reg,reg);
#else    
    XUnionRectWithRegion(&r,reg,reg);
#endif
  }
  invalidateRegion(wnd, reg);
  XDestroyRegion(reg);
#endif //platform
}

void StdWnd::invalidateRegion(OSWINDOWHANDLE wnd, OSREGIONHANDLE region) {
  StdWnd::clipOSChildren(wnd, region);
  InvalidateRgn(wnd, region, FALSE);
}

void StdWnd::validateRect(OSWINDOWHANDLE wnd, RECT *r) {
  ValidateRect(wnd, r);
}

void StdWnd::validateRegion(OSWINDOWHANDLE wnd, OSREGIONHANDLE region) {
  ValidateRgn(wnd, region);
}

void StdWnd::update(OSWINDOWHANDLE wnd) {
#if defined(WIN32)
  if (wnd != NULL) UpdateWindow(wnd);
#elif defined(XWINDOW)
  PostMessage( wnd, WM_PAINT, 0, 0 );
#if 0
  //Expose the Update Rect.
  RECT r;
  StdWnd::getUpdateRect(wnd, &r);
  Rect wr(r);
  XEvent e;
  e.xexpose.type = Expose;
  e.xexpose.window = wnd;
  e.xexpose.x = wr.left;
  e.xexpose.y = wr.top;
  e.xexpose.width = wr.width();
  e.xexpose.height = wr.height();
  e.xexpose.count = 0;
  XSendEvent(XWindow::getDisplay(), wnd, TRUE, ExposureMask, &e);
#endif 
#else
#error port me!
#endif
}

int StdWnd::getUpdateRect(OSWINDOWHANDLE wnd, RECT *r) {
#if defined(WIN32)
  return GetUpdateRect(wnd, r, FALSE);
#elif defined(XWINDOW)
  OSREGIONHANDLE invalid = (OSREGIONHANDLE)GetWindowLong( wnd, GWL_INVALIDREGION );
  if ( ! invalid || XEmptyRegion( invalid ) )
    return 0;

  XRectangle xr;
  XClipBox( invalid, &xr );
#if defined(WASABI_WIN32RECT)
  (*r) = Std::makeRectWH(xr.x, xr.y, xr.width, xr.height);
#else
  (*r) = xr;
#endif
  return 1;
#else
#error port me!
#endif // platform
}

void StdWnd::getUpdateRegion(OSWINDOWHANDLE wnd, OSREGIONHANDLE region) {
#if defined(WIN32)
  GetUpdateRgn(wnd, region, FALSE);
#elif defined(XWINDOW)
  XSubtractRegion( region, region, region );

  OSREGIONHANDLE invalid = (OSREGIONHANDLE)GetWindowLong( wnd, GWL_INVALIDREGION );
  if ( ! invalid ) return;

  XUnionRegion( region, invalid, region );

  RECT rct;
  StdWnd::getClientRect( wnd, &rct );
  OSREGIONHANDLE tmp = XCreateRegion();
#if defined(WASABI_WIN32RECT)
  XRectangle xr;
  xr.x = 0;
  xr.y = 0;
  xr.width = rct.right - rct.left;
  xr.height = rct.bottom - rct.top;
  XUnionRectWithRegion( &xr, tmp, tmp );
#else
  XUnionRectWithRegion( &rct, tmp, tmp );
#endif
  XIntersectRegion( region, tmp, region );
  XDestroyRegion( tmp );
#else
#error port me!
#endif // platform
}

int StdWnd::haveGetRandomRgn() {
#ifdef WIN32
  // I assume linux will just return FALSE
  if (gdi32instance == NULL && !grrfailed) {
    gdi32instance=LoadLibrary("GDI32.dll");
    if (gdi32instance != NULL) {
      getRandomRgnFn = (int (WINAPI *)(HDC,HRGN,int)) GetProcAddress(gdi32instance,"GetRandomRgn");
      if (getRandomRgnFn == NULL) {
        grrfailed=1;
        FreeLibrary(gdi32instance);
        gdi32instance=NULL;
      }
    } else {
      grrfailed=1;
    }
  }
  return (getRandomRgnFn != NULL);
#else // WIN32-specific
  return FALSE;
#endif
}

void StdWnd::getRandomRgn(HDC hdc, OSREGIONHANDLE region) {
#ifdef WIN32
  if (!haveGetRandomRgn()) return;
  ASSERT(getRandomRgnFn != NULL);
  (*getRandomRgnFn)(hdc, region, SYSRGN);
#else // WIN32-specific
  return;
#endif
}

void StdWnd::setWndRegion(OSWINDOWHANDLE wnd, OSREGIONHANDLE region, int redraw) {
#if defined(WIN32)
  SetWindowRgn(wnd, region, !!redraw);
#elif defined(XWINDOW)
  if(region)
    XShapeCombineRegion( XWindow::getDisplay(), wnd, ShapeBounding, 0, 0, region, ShapeSet );
  else
    XShapeCombineMask( XWindow::getDisplay(), wnd, ShapeBounding, 0, 0, None, ShapeSet );
#else
#error port me!
#endif // platform
}

HDC StdWnd::getDC(OSWINDOWHANDLE wnd) {
#if defined(WIN32)
  return GetDC(wnd);
#elif defined(XWINDOW)
  HDC dc = (HDC)MALLOC( sizeof( hdc_typ ) );
  if(dc == NULL) return NULL;
  dc->d = wnd;
  dc->gc = XCreateGC( XWindow::getDisplay(), dc->d, 0, NULL );
  return dc;
#else
#error port me!
#endif // platform
}

int StdWnd::releaseDC(OSWINDOWHANDLE wnd, HDC dc) {
#if defined(WIN32)
  return ReleaseDC(wnd, dc);
#elif defined(XWINDOW)
  if( dc->d != wnd) return 0;
  XFreeGC( XWindow::getDisplay(), dc->gc );
  FREE( dc );
  return 1;
#else
#error port me!
#endif // platform
}

int StdWnd::isDesktopAlphaAvailable() {
#ifdef WIN32
  if (user32instance == NULL) user32instance = LoadLibrary("USER32.DLL");
  if (user32instance != NULL) {

    if (setLayeredWindowAttributes == NULL || updateLayeredWindow == NULL) {
      if (!checked_for_alpha_proc) {
        setLayeredWindowAttributes=(void (__stdcall *)(HWND,int,int,int))GetProcAddress(user32instance ,"SetLayeredWindowAttributes");
        updateLayeredWindow=(BOOL (__stdcall *)(HWND,HDC,POINT*,SIZE*,HDC,POINT*,COLORREF,BLENDFUNCTION*,DWORD))GetProcAddress(user32instance ,"UpdateLayeredWindow");
        checked_for_alpha_proc = 1;
      }
    }
  }
  return (updateLayeredWindow != NULL);
#elif defined(XWINDOW)
  //TODO WaI> We need to wait for X Composite extensions to come around, then we can check for a Compositing Manager.
  return FALSE;
#else
#error port me!
#endif // platform
}

int StdWnd::isTransparencyAvailable() {
// there is no win32 implementation that supports setLayeredWindowAttributes but not updateLayeredWindow
  return StdWnd::isDesktopAlphaAvailable();
}

int StdWnd::getClientRect(OSWINDOWHANDLE wnd, RECT *r) {
#if defined(WIN32)
  return GetClientRect(wnd, r);
#elif defined(XWINDOW)
  if (!r) return 0;
  Window root;
  int x, y;
  unsigned int w, h, bw, d;
  XGetGeometry(XWindow::getDisplay(), wnd, &root, &x, &y, &w, &h, &bw, &d);
  
  (*r) = Std::makeRectWH(0,0,w,h);
  
  return 1;
#else
#error port me!
#endif // platform
}

int StdWnd::getWindowRect(OSWINDOWHANDLE wnd, RECT *r) {
#if defined(WIN32)
  return GetWindowRect(wnd, r);
#elif defined(XWINDOW)
  if (!r) return 0;
  int x, y;
  unsigned int w, h, bw, d;
  Window root, parent, child;

  XGetGeometry(XWindow::getDisplay(), wnd, &root, &x, &y, &w, &h, &bw, &d);
  XTranslateCoordinates(XWindow::getDisplay(), wnd, root, 0, 0, &x, &y, &child);
  
  (*r) = Std::makeRectWH(x,y,w,h);  
  
  return 1;
#else
#error port me!
#endif // platform
}

void StdWnd::clientToScreen(OSWINDOWHANDLE wnd, int *x, int *y) {
#if defined(WIN32)
  POINT p = { x ? *x : 0, y ? *y : 0 };
  ClientToScreen(wnd, &p);
  if (x) *x = p.x;
  if (y) *y = p.y;
#elif defined(XWINDOW)
  Window child;
  int _x, _y;
  XTranslateCoordinates(XWindow::getDisplay(), wnd, XWindow::RootWin(wnd), 
                        x ? *x : 0, y ? *y : 0, &_x, &_y, &child);
  if(x) *x = _x;
  if(y) *y = _y;
#else
#error port me!
#endif // platform
}

void StdWnd::clientToScreen(OSWINDOWHANDLE wnd, RECT *r) {
  Rect rr(r);
  StdWnd::clientToScreen(wnd, (int*)&rr.left, (int*)&rr.top);
  StdWnd::clientToScreen(wnd, (int*)&rr.right, (int*)&rr.bottom);
  *r = rr;
}

void StdWnd::screenToClient(OSWINDOWHANDLE wnd, int *x, int *y) {
#if defined(WIN32)
  POINT p = { x ? *x : 0, y ? *y : 0 };
  ScreenToClient(wnd, &p);
  if (x) *x = p.x;
  if (y) *y = p.y;
#elif defined(XWINDOW)
  Window child;
  int _x, _y;
  XTranslateCoordinates(XWindow::getDisplay(), XWindow::RootWin(wnd), wnd, 
                        x ? *x : 0, y ? *y : 0, &_x, &_y, &child);
  if(x) *x = _x;
  if(y) *y = _y;
#else
#error port me!
#endif // platform
}

void StdWnd::screenToClient(OSWINDOWHANDLE wnd, RECT *r) {
  Rect rr(r);
  StdWnd::screenToClient(wnd, (int*)&rr.left, (int*)&rr.top);
  StdWnd::screenToClient(wnd, (int*)&rr.right, (int*)&rr.bottom);
  *r = rr;
}

OSWINDOWHANDLE StdWnd::windowFromPoint( POINT p ) {
#if defined(WIN32)
  return WindowFromPoint(p);
#elif defined(XWINDOW)
  Window child;
  int x, y;
  XTranslateCoordinates( XWindow::getDisplay(), XWindow::RootWin(), XWindow::RootWin(), 
                         p.x, p.y, &x, &y, &child );
  return child;
#else
#error port me!
#endif //platform
}


void StdWnd::setCapture(OSWINDOWHANDLE wnd) {
#if defined(WIN32)
  SetCapture(wnd);
#elif defined(XWINDOW)
  XGrabPointer(XWindow::getDisplay(), wnd, /*??*/True, 0/*??*/, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
  XFlush(XWindow::getDisplay());
#else
#error port me!
#endif // platform
}

void StdWnd::releaseCapture() {
#if defined(WIN32)
  ReleaseCapture();
#elif defined(XWINDOW)
  XUngrabPointer(XWindow::getDisplay(), CurrentTime);
  XFlush(XWindow::getDisplay());
#else
#error port me!
#endif // platform
}

OSWINDOWHANDLE StdWnd::getCapture() {
#if defined(WIN32)
  return GetCapture();
#elif defined(XWINDOW)
#warning StdWnd::getCapture() is nonfunctional.
  DebugString("StdWnd::getCapture: Don't call me, I don't work!"); // WaI> I almost want to assert(0).
  return None;
#else
#error port me!
#endif // platform
}

void StdWnd::revokeDragNDrop(OSWINDOWHANDLE wnd) {
#if defined(WIN32)
  if (wnd == NULL) return;
  RevokeDragDrop(wnd);
#elif defined(XWINDOW)
  Atom DND = XInternAtom( XWindow::getDisplay(), "XdndAware", False );
  if ( DND ) {
    Atom version = 3;
    XDeleteProperty( XWindow::getDisplay(), wnd, DND);
  }
#else
#error port me!
#endif // platform
}

void StdWnd::setWndName(OSWINDOWHANDLE wnd, const char *name) {
#if defined(WIN32)
  SetWindowText(wnd, name);
#elif defined(XWINDOW)
  XStoreName(XWindow::getDisplay(), wnd, name);
#else
#error port me!
#endif // platform
}

void StdWnd::getWndName(OSWINDOWHANDLE wnd, char *name, int maxlen) {
#if defined(WIN32)
  GetWindowText(wnd, name, maxlen);
#elif defined(XWINDOW)
  XFetchName(XWindow::getDisplay(), wnd, &name);
#else
#error port me!
#endif // platform
}

void StdWnd::setIcon(OSWINDOWHANDLE wnd, OSICONHANDLE icon, int large) {
#if defined(WIN32)
  SendMessage(wnd, WM_SETICON, !large ? ICON_SMALL : ICON_BIG, (int)icon);
#elif defined(XWINDOW)
  XWMHints *hints = XAllocWMHints();
  hints->flags = IconPixmapHint;
  hints->icon_pixmap = icon;
  XSetWMHints(XWindow::getDisplay(), wnd, hints);
  XFree(hints);
#else
#error port me!
#endif // platform
}

OSWINDOWHANDLE StdWnd::getActiveWindow() {
#if defined(WIN32)
  return GetActiveWindow();
#elif defined(XWINDOW)
  Window w; int revert;
  XGetInputFocus(XWindow::getDisplay(), &w, &revert);
  return w;
#else
#error port me!
#endif // platform
}

void StdWnd::setActiveWindow(OSWINDOWHANDLE wnd) {
#if defined(WIN32)
  SetActiveWindow(wnd);
#elif defined(XWINDOW)
  XSetInputFocus(XWindow::getDisplay(), wnd, RevertToParent, CurrentTime);
#else
#error port me!
#endif // platform
}

// clip oswindow children off the invalidation region
void StdWnd::clipOSChildren(OSWINDOWHANDLE wnd, OSREGIONHANDLE reg) {
  OSWINDOWHANDLE w = StdWnd::getTopmostChild(wnd);
#ifdef XWINDOW
  while (w != None)
#else
  while (w != NULL)
#endif
    {
    if (StdWnd::isWndVisible(w)) {
      RECT r;
      StdWnd::getClientRect(w, &r);
      POINT p= Rect(r).topLeft();
      StdWnd::clientToScreen(w, (int *)&p.x, (int *)&p.y);
      StdWnd::screenToClient(wnd, (int *)&p.x, (int *)&p.y);
      
      Std::offsetRect(&r, p.x, p.y);
//TODO: once Region is ported, use it in this #ifdef block:
#   if defined(WIN32)
      OSREGIONHANDLE cr = CreateRectRgnIndirect(&r);  // cr = Region(r);
      OSREGIONHANDLE or = CreateRectRgn(0, 0, 0, 0);  // or = 0;
      CombineRgn(or, reg, 0, RGN_COPY);               // or = reg;
      CombineRgn(reg, or, cr, RGN_DIFF);              // reg = or - cr;
      DeleteObject(cr);
      DeleteObject(or);
#   elif defined(XWINDOW)
#   if defined(WASABI_WIN32RECT)
      XRectangle xr;
      xr.x = r.left; xr.y = r.top;
      xr.width = r.right - r.left;
      xr.height = r.bottom - r.top;
#   endif
      OSREGIONHANDLE cr = XCreateRegion(); // cr = 0;
#   if defined(WASABI_WIN32RECT)
      XUnionRectWithRegion(&xr, cr, cr);    // cr = cr + Region(xr);
#   else
      XUnionRectWithRegion(&r, cr, cr);    // cr = cr + Region(xr);
#   endif
      OSREGIONHANDLE ur = XCreateRegion(); // ur = 0;
      XUnionRegion(ur, reg, ur);           // ur = ur + reg;
      XSubtractRegion(ur, cr, reg);        // reg = ur - cr;
      XDestroyRegion(ur);
      XDestroyRegion(cr);
#   else
#   error port me!
#   endif // platform 
    }
    w = StdWnd::getTopmostChild(w);
  }
}

//////////////////////////
#ifdef WIN32
static void register_wndClass(HINSTANCE hInstance, const char *classname, WNDPROC wndproc, int want_dclicks) {
  WNDCLASS wc;
  if (GetClassInfo(hInstance, classname, &wc)) return;

  // register wnd class
  wc.style = want_dclicks ? CS_DBLCLKS : 0;
  wc.lpfnWndProc = wndproc;
  wc.cbClsExtra		= 0;
  wc.cbWndExtra		= 0;
  wc.hInstance		= hInstance;
  wc.hIcon		= NULL;
  wc.hCursor		= NULL;
  wc.hbrBackground	= (HBRUSH)GetStockObject(NULL_BRUSH);
  wc.lpszMenuName	= NULL;
  wc.lpszClassName	= classname;

  int r = RegisterClass(&wc);
  if (r == 0) {
    int res = GetLastError();
    if (res != ERROR_CLASS_ALREADY_EXISTS) {
      char florp[WA_MAX_PATH];
      SPRINTF(florp, "Failed to register class, err %d", res);
      ASSERTPR(0, florp);
    }
  }
}
#endif // WIN32

void StdWnd::getViewport(RECT *r, POINT *p, int full) {
#if defined(WIN32)
  getViewport(r, p, NULL, (HWND)0, full);
#elif defined(XWINDOW)
  ASSERT(r != NULL);
  if(XWindow::usingXinerama()) {
    RECT screen;
    for(int i = 0; XWindow::enumScreen(i, &screen); i++) {
      Rect wr(screen);
      if(     p->x >= wr.left && p->x <= wr.right
           && p->y >= wr.top  && p->x <= wr.bottom ) 
      {
        (*r) = (RECT)wr;
        break;
      }
    }
  }
  else getViewport(r, NULL, NULL, None, full);
#else
#error port me!
#endif
}

void StdWnd::getViewport(RECT *r, RECT *sr, int full) {
#if defined(WIN32)
  getViewport(r, NULL, sr, (HWND)0, full);
#elif defined(XWINDOW)
  ASSERT(r != NULL);
  if(XWindow::usingXinerama()) {
    
    OSREGIONHANDLE rgna = XCreateRegion();
#if defined(WASABI_WIN32RECT)
    XRectangle xr;
    xr.x = sr->left;
    xr.y = sr->top;
    xr.width = sr->right - sr->left;
    xr.height = sr->bottom - sr->top;
    XUnionRectWithRegion(&xr, rgna, rgna);
#else
    XUnionRectWithRegion(sr, rgna, rgna);
#endif
    // find screen with greatest intersection area with sr.
    unsigned int area = 0;
    RECT screen;
    for(int i = 0; XWindow::enumScreen(i, &screen); i++) {
      OSREGIONHANDLE rgnb = XCreateRegion();
#if defined(WASABI_WIN32RECT)
      XRectangle xscreen;
      xscreen.x = screen.left; xscreen.y = screen.top;
      xscreen.width = screen.right - screen.left;
      xscreen.height = screen.bottom - screen.top;
      XUnionRectWithRegion(&xscreen, rgnb, rgnb);
#else
      XUnionRectWithRegion(&screen, rgnb, rgnb);
#endif
      OSREGIONHANDLE isect = XCreateRegion();
      XIntersectRegion(rgna, rgnb, rgnb);
      XRectangle box;
      XClipBox(isect, &box);
      XDestroyRegion(rgnb);
      
      unsigned int area_tmp = abs(box.width * box.height);
      if(area_tmp > area) {
        area = area_tmp;
        (*r) = screen;
      }
    }
    XDestroyRegion(rgna);
  } // usingXinerama
  else getViewport(r, NULL, NULL, None, full);
#else
#error port me!
#endif
}

void StdWnd::getViewport(RECT *r, OSWINDOWHANDLE wnd, int full) {
#if defined(WIN32)
  getViewport(r, NULL, NULL, wnd, full);
#elif defined(XWINDOW)
  ASSERT(r != NULL);
  if(XWindow::usingXinerama()) {
    Window root;
    int x,y;
    unsigned int width, height, bw, d;
    XGetGeometry(XWindow::getDisplay(), wnd, &root, &x, &y, &width, &height, &bw, &d);
    RECT sr = Std::makeRectWH(x, y, width, height);
    getViewport(r, &sr, full);
  }
  else getViewport(r, NULL, NULL, None, full);
#else
#error port me!
#endif
}

void StdWnd::getViewport(RECT *r, POINT *p, RECT *sr, OSWINDOWHANDLE wnd, int full) {
  ASSERT(r != NULL);
#ifdef WIN32
  if (p || sr || wnd) {
    HINSTANCE h=LoadLibrary("user32.dll");
    if (h) {
      HMONITOR (WINAPI *Mfp)(POINT pt, DWORD dwFlags) = (HMONITOR (WINAPI *)(POINT,DWORD)) GetProcAddress(h,"MonitorFromPoint");
      HMONITOR (WINAPI *Mfr)(LPCRECT lpcr, DWORD dwFlags) = (HMONITOR (WINAPI *)(LPCRECT, DWORD)) GetProcAddress(h, "MonitorFromRect");
      HMONITOR (WINAPI *Mfw)(HWND wnd, DWORD dwFlags)=(HMONITOR (WINAPI *)(HWND, DWORD)) GetProcAddress(h, "MonitorFromWindow");
      BOOL (WINAPI *Gmi)(HMONITOR mon, LPMONITORINFO lpmi) = (BOOL (WINAPI *)(HMONITOR,LPMONITORINFO)) GetProcAddress(h,"GetMonitorInfoA");
      if (Mfp && Mfr && Mfw && Gmi) {
        HMONITOR hm;
        if (p)
          hm=Mfp(*p,MONITOR_DEFAULTTONULL);
        else if (sr)
          hm=Mfr(sr,MONITOR_DEFAULTTONULL);
        else if (wnd)
          hm=Mfw(wnd,MONITOR_DEFAULTTONULL);
        if (hm) {
          MONITORINFOEX mi;
          ZERO(mi);
          mi.cbSize=sizeof(mi);

          if (Gmi(hm,&mi)) {
            if(!full) *r=mi.rcWork;
            else *r=mi.rcMonitor;
            FreeLibrary(h);
            return;
          }
        }
      }
      FreeLibrary(h);
    }
  }
  SystemParametersInfo(SPI_GETWORKAREA,0,r,0);
#endif
#ifdef XWINDOW
  if (p) getViewport(r,p,full);
  else if (sr) getViewport(r,sr,full);
  else if (wnd) getViewport(r,wnd,full);
  else {
    (*r) = Std::makeRectWH(0,0,DisplayWidth(XWindow::getDisplay(),XWindow::getScreenNum()),
                           DisplayHeight(XWindow::getDisplay(), XWindow::getScreenNum()) );
  }
#endif
}

#ifdef WIN32
class ViewportEnumerator {
  public:
    HMONITOR hm;
    int monitor_n;
    int counter;
};

static BOOL CALLBACK enumViewportsProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
  ViewportEnumerator *ve = reinterpret_cast<ViewportEnumerator *>(dwData);
  ASSERT(ve != NULL);
  if (ve->counter == ve->monitor_n) {
    ve->hm = hMonitor;
    return FALSE;
  }
  ve->counter++;
  return TRUE;
}
#endif

int StdWnd::enumViewport(int monitor_n, RECT *r, int full) {
  ASSERT(r != NULL);
#if defined(WIN32)
  HINSTANCE h=LoadLibrary("user32.dll");
  if (h) {
    BOOL (WINAPI *Gmi)(HMONITOR mon, LPMONITORINFO lpmi) = (BOOL (WINAPI *)(HMONITOR,LPMONITORINFO)) GetProcAddress(h,"GetMonitorInfoA");
    BOOL (WINAPI *Edm)(HDC hdc, LPCRECT clip, MONITORENUMPROC proc, LPARAM dwdata) = (BOOL (WINAPI *)(HDC,LPCRECT,MONITORENUMPROC,LPARAM)) GetProcAddress(h,"EnumDisplayMonitors");
    if (Gmi && Edm) {
      ViewportEnumerator ve;
      ve.monitor_n = monitor_n;
      ve.hm = NULL;
      ve.counter = 0;
      Edm(NULL, NULL, enumViewportsProc, reinterpret_cast<LPARAM>(&ve));
      HMONITOR hm = ve.hm;
      if (hm) {
        MONITORINFOEX mi;
        ZERO(mi);
        mi.cbSize=sizeof(mi);
        if (Gmi(hm,&mi)) {
          if(!full) *r=mi.rcWork;
          else *r=mi.rcMonitor;
          FreeLibrary(h);
          return 1;
        }
      }
    }
    FreeLibrary(h);
  }
  SystemParametersInfo(SPI_GETWORKAREA,0,r,0);
  return 0;
#elif defined(XWINDOW)
  if(XWindow::usingXinerama()) {
    return XWindow::enumScreen(monitor_n, r);
  } else {
    if ( monitor_n > 0 )
      return 0;
    (*r) = Std::makeRect(0,0,DisplayWidth(XWindow::getDisplay(),XWindow::getScreenNum()),
                         DisplayHeight(XWindow::getDisplay(), XWindow::getScreenNum()) );
    return 1;
  }
#else
#error port me!
#endif // platform
}

RECT StdWnd::getAllMonitorsRect() {
//Portable
  RECT ret = Std::makeRect(), r;
  for (int i = 0; ; i++) {
    if (!StdWnd::enumViewport(i, &r, TRUE)) break;
    ret = Std::makeRect(
            MIN(ret.left, r.left),
            MIN(ret.top, r.top),
            MAX(ret.right, r.right),
            MAX(ret.bottom, r.bottom) );
  }
  return ret;
}

void StdWnd::getMousePos(POINT *p) {
  ASSERT(p != NULL);
#if defined(WIN32)
  GetCursorPos(p);
#elif defined(XWINDOW)
  getMousePos(&p->x, &p->y);
#else
#error port me!
#endif // platform
}

void StdWnd::getMousePos(int *x, int *y) {
#if defined(WIN32)
  POINT p;
  getMousePos(&p);
  if (x != NULL) *x = p.x;
  if (y != NULL) *y = p.y;
#elif defined(XWINDOW)
  Window root, child;
  int rx, ry, wx, wy;
  unsigned int buttons;
  XQueryPointer( XWindow::getDisplay(), XWindow::RootWin(), &root, &child,
                 &rx, &ry, &wx, &wy, &buttons );
  if(x) *x = rx;
  if(y) *y = ry;
#else
#error port me!
#endif // platform
}

void StdWnd::getMousePos(long *x, long *y) {
  getMousePos((int *)x, (int *)y);
}

void StdWnd::setMousePos(POINT *p) {
  ASSERT(p != NULL);
  setMousePos(p->x, p->y);
}

void StdWnd::setMousePos(int x, int y) {
#ifdef WIN32
  SetCursorPos(x, y);
#elif defined(XWINDOW)
  int cx, cy;
  getMousePos( &cx, &cy );
  XWarpPointer( XWindow::getDisplay(), None, None,
    0, 0, 1, 1, x - cx, y - cy );
#else
#error port me!
#endif
}

int StdWnd::getDoubleClickDelay() {
#ifdef WIN32
  return GetDoubleClickTime();
#else
  //TODO
  return 200;
#endif
}

#undef GetSystemMetrics //FG> DUH!
int StdWnd::getDoubleClickX() {
#ifdef WIN32
  return ::GetSystemMetrics(SM_CYDOUBLECLK);
#else
  //TODO
  return 5;
#endif
}

int StdWnd::getDoubleClickY() {
#ifdef WIN32
  return ::GetSystemMetrics(SM_CXDOUBLECLK);
#else
  //TODO
  return 5;
#endif
}

int StdWnd::getLastInputInfo() {
#ifdef WIN32
  if (user32instance == NULL) user32instance = LoadLibrary("USER32.DLL");

  if (getLastInputInfoFn == NULL) {
    if (!glii_failed) getLastInputInfoFn = (BOOL (WINAPI *)(PLASTINPUTINFO))GetProcAddress(user32instance, "GetLastInputInfo");
  }

  // still NULL
  if (getLastInputInfoFn == NULL) {
    glii_failed = 1;
    return -1;	// failure
  }

  LASTINPUTINFO lii;
  lii.cbSize = sizeof(lii);
  lii.dwTime = 0;

  (*getLastInputInfoFn)(&lii);

  return GetTickCount() - lii.dwTime;	// in ms
#else
  return -1; 	// port me
#endif
}

int StdWnd::messageBox(const char *txt, const char *title, int flags) {
#ifdef WIN32
  return MessageBox(NULL, txt, title, flags);
#elif defined(XWINDOW)
  return XWindow::MessageBox(XWindow::getDisplay(), txt, title, flags);
#else
 DebugString(txt);
#endif
}

int StdWnd::mousewheelGetScrollLines() {
#ifdef WIN32
  int ret;
  SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ret, 0);
  return ret;
#elif defined(XWINDOW)
  //TODO
  return 3;
#else
#error port me!
#endif
}

int StdWnd::mousewheelGetSmoothScroll() {
#ifdef WIN32
  int ret;
  SystemParametersInfo(SPI_GETLISTBOXSMOOTHSCROLLING, 0, &ret, 0);
  return ret;
#elif defined(XWINDOW)
  //TODO
  return 1;
#else
#error port me!
#endif
}

int StdWnd::getMetric(int m) {
#ifdef WASABI_PLATFORM_WIN32
  switch (m) {
    case SCROLLBAR_VWIDTH:
      return ::GetSystemMetrics(SM_CXVSCROLL);

    case SCROLLBAR_VHEIGHT:
      return ::GetSystemMetrics(SM_CYVSCROLL);

    case SCROLLBAR_HWIDTH:
      return ::GetSystemMetrics(SM_CXHSCROLL);

    case SCROLLBAR_HHEIGHT:
      return ::GetSystemMetrics(SM_CYHSCROLL);
  }
#else
#endif
  return 0;
}

#ifdef WASABI_PLATFORM_WIN32
static int checkForUnicodeClipboard() {
  static int g_unicodeclip = -1;
  if (g_unicodeclip < 0) {
    OSVERSIONINFO osvi = {0};
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) g_unicodeclip = 1;
    else g_unicodeclip = 0;
  }
  ASSERT(g_unicodeclip >= 0);
  return g_unicodeclip;
}

// note: not sure how to mark this up for #ifdef WASABI_COMPILE_UTF
static HGLOBAL CopyStringToHGlobal(const char *str, int *type) {
  ASSERT(type != NULL);

  String s = str;
  if (s.isempty()) s = "";
#ifdef WASABI_COMPILE_UTF
  EncodedStr enc;

  if (checkForUnicodeClipboard()) {
    enc.convertFromUTF8(SvcStrCnv::UTF16, s);
    *type = CF_UNICODETEXT;
  } else {
    enc.convertFromUTF8(SvcStrCnv::OSNATIVE, s);
    *type = CF_TEXT;
  }

  if (enc.getBufferSize() <= 0 || enc.getEncodedBuffer() == NULL) return NULL;

  HGLOBAL hMem = GlobalAlloc(GHND, (DWORD)(enc.getBufferSize()+1)); 
#else
	*type = CF_TEXT;
	HGLOBAL hMem = GlobalAlloc(GHND, (DWORD)(s.len()+1));
#endif
  if (hMem == NULL) return NULL;

  char *dst = (char *)GlobalLock(hMem);
  if (!dst) {
    GlobalFree(hMem);
    return NULL;
  }
#ifdef WASABI_COMPILE_UTF
  MEMCPY(dst, enc.getEncodedBuffer(), enc.getBufferSize());
#else
	MEMCPY(dst, s.v(), s.len());
#endif
  GlobalUnlock(hMem);

  return hMem;
}
#endif

int StdWnd::setClipboard(const char *txt) {
  int ret = 0;
#ifdef WASABI_PLATFORM_WIN32
  int type=0;
  HGLOBAL hMem = CopyStringToHGlobal(txt, &type);
  if (hMem != NULL) {
    if (OpenClipboard(NULL)) {
      EmptyClipboard();
      ret = (SetClipboardData(type, hMem) != NULL);
    } else {
      GlobalFree(hMem);
      ret = 0;
    }
    CloseClipboard();
  }
#else
  DebugString("StdWnd::setClipboard: port me");
#endif
  return ret;
}

String StdWnd::getClipboard() {
  String ret;
#ifdef WASABI_PLATFORM_WIN32
  int type=0;
  if (OpenClipboard(NULL)) {
//BU: commented out stuff is for unicode, but doesn't quite work. seems like it should, though
//    if (checkForUnicodeClipboard()) {
//      type = CF_UNICODETEXT;
//    } else {
      type = CF_TEXT;
//    }
    HANDLE h = GetClipboardData(type);
    if (h) {
      ret.setValue((char *)GlobalLock(h));
      GlobalUnlock(h); h = NULL;
      CloseClipboard();
    
//      if (ret.notempty()) {
//        EncodedStr enc((type == CF_UNICODETEXT) ? SvcStrCnv::UTF16 : SvcStrCnv::OSNATIVE, (void*)ret.v(), ret.len(), FALSE);

//        String tmp;
//        enc.convertToUTF8(tmp);
//        ret = tmp;
//      }
    }
  }
#else
  DebugString("StdWnd::getClipboard: port me");
#endif
  return ret;
}
