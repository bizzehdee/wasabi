#include <bfc/platform/platform.h>
#include <bfc/std_string.h>
#include <bfc/std_wnd.h> // need some helpers from here. the interdependence shouldn't get too hairy.

#include <api/api.h>
  #include <api/linux/api_linux.h>

Display *XWindow::display = NULL;

int linux_atoms_loaded = 0;

Atom winamp_msg;
Atom dnd_enter, dnd_position, dnd_status, dnd_leave, dnd_drop, dnd_finished;
Atom dnd_selection, dnd_wa3drop, dnd_private, dnd_typelist;
Atom dnd_urilist, dnd_textplain, dnd_mozurl;

void LoadAtoms() {
  if ( !linux_atoms_loaded ) {
    linux_atoms_loaded = 1;
    winamp_msg = XInternAtom( XWindow::getDisplay(), "Winamp3", False );
    dnd_wa3drop = XInternAtom( XWindow::getDisplay(), "Winamp3_drop", False );
    dnd_enter = XInternAtom( XWindow::getDisplay(), "XdndEnter", True );
    dnd_position = XInternAtom( XWindow::getDisplay(), "XdndPosition", True );
    dnd_status = XInternAtom( XWindow::getDisplay(), "XdndStatus", True );
    dnd_leave = XInternAtom( XWindow::getDisplay(), "XdndLeave", True );
    dnd_drop = XInternAtom( XWindow::getDisplay(), "XdndDrop", True );
    dnd_finished = XInternAtom( XWindow::getDisplay(), "XdndFinished", True );
    dnd_selection = XInternAtom( XWindow::getDisplay(), "XdndSelection", True );
    dnd_private = XInternAtom( XWindow::getDisplay(), "XdndActionPrivate", True );
    dnd_typelist = XInternAtom( XWindow::getDisplay(), "XdndTypeList", True );
    dnd_urilist = XInternAtom( XWindow::getDisplay(), "text/uri-list", True );
    dnd_textplain = XInternAtom( XWindow::getDisplay(), "text/plain", True );
    dnd_mozurl = XInternAtom( XWindow::getDisplay(), "text/x-moz-url", True );
  }
}

Display *XWindow::getDisplay() {
  if ( ! display )
    display = WASABI_API_LINUX->linux_getDisplay();

  return display;
}

XContext XWindow::getContext() {
  static XContext context = 0;

  if ( context == 0 )
    context = WASABI_API_LINUX->linux_getContext();

  return context;
}

int XWindow::getScreenNum() { return DefaultScreen( getDisplay() ); }

Window XWindow::RootWin() {
  return RootWindow( getDisplay(), getScreenNum() );
}

Window XWindow::RootWin(Window wnd) {
  Window root;
  int x, y;
  unsigned int w, h, bw, d;
  XGetGeometry(XWindow::getDisplay(), wnd, &root, &x, &y, &w, &h, &bw, &d);
  return root;
}


Visual *XWindow::DefaultVis() {
  return DefaultVisual( getDisplay(), getScreenNum() );
}

void XWindow::setCursor( Window w, int cursor ) {
  OSCURSORHANDLE c = XCreateFontCursor( XWindow::getDisplay(), cursor );

  if ( cursor == None )
    XUndefineCursor( XWindow::getDisplay(), w );
  else
    XDefineCursor( XWindow::getDisplay(), w, c );

  XFreeCursor( XWindow::getDisplay(), c );
}

int XWindow::usingXinerama() {
  return XineramaQueryExtension( XWindow::getDisplay(), NULL, NULL)
         && XineramaIsActive( XWindow::getDisplay() );
}

static const XineramaScreenInfo *xsi;
static int xsi_num;

int XWindow::enumScreen(int i, RECT *screen) {
  if(!XWindow::usingXinerama()) return 0;
  if(!xsi) {
    xsi = WASABI_API_LINUX->linux_getXineramaInfo(&xsi_num);
  }
  ASSERT(xsi != NULL);
  if(i < 0 || i >= xsi_num) return 0;
  if(screen) (*screen) = Std::makeRectWH(xsi[i].x_org, xsi[i].y_org, xsi[i].width, xsi[i].height);
  return 1;
}

// This thing is ugly, but it gets the job done.
int XWindow::MessageBox(Display *dis, const char *text, const char *title, int flags) {
  int w = 300, h = 100;
  // Get some colors
  int blackColor = BlackPixel(dis, DefaultScreen(dis));
  int whiteColor = WhitePixel(dis, DefaultScreen(dis));
  
  Window win = XCreateSimpleWindow(dis, DefaultRootWindow(dis), 200, 100, w, h, 0, 0, whiteColor);
  XSelectInput(dis, win, StructureNotifyMask);  // We want MapNotify events
  XStoreName(dis, win, title);  // Title
  XMapWindow(dis, win);

  // Create a "Graphics Context"
  GC gc = XCreateGC(dis, win, 0, NULL);
  // Tell the GC we draw using the black color
  XSetForeground(dis, gc, blackColor);

  // Wait for the MapNotify event
  XEvent e;
  do XNextEvent(dis, &e);
  while (e.type != MapNotify);

  // Get a font
  XFontStruct *fnts = XLoadQueryFont(dis, "-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-iso8859-*");
  // Adjust the window width.
  int twidth = XTextWidth(fnts, text, strlen(text));
  if(twidth > (w - 20) ) {
    XWindowChanges xwc;
    xwc.width = twidth+30;
    XConfigureWindow( dis, win, CWWidth, &xwc );
    XFlush(dis);
  }
  
  XDrawString(dis, win, gc, 10, 50, text, strlen(text));
  
  XUnloadFont(dis, fnts->fid);
  
  XFlush(dis);
}


int XWindow::convertEvent( MSG *m, XEvent *e ) {
  m->hwnd = e->xany.window;

  if ( m->hwnd ) {
    RootWnd *rw =(RootWnd *)GetWindowLong( m->hwnd, GWL_USERDATA );
    if ( !rw ) {
      // This is to fix messages for dead windows...
      return 0;
    }
  }

  switch ( e->type ) {
  case ButtonPress:
    m->lParam = (e->xbutton.x & 0xffff) | (e->xbutton.y << 16);
    switch( e->xbutton.button ) {
    case Button1: 
      m->message = WM_LBUTTONDOWN;
      break;

    case Button2:
      m->message = WM_MBUTTONDOWN;
      break;

    case Button3:
      m->message = WM_RBUTTONDOWN;
      break;

    case Button4:
      m->message = WM_MOUSEWHEEL;
      m->wParam = 120 << 16 | 0; // 1 tick, no modifiers
      break;

    case Button5:
      m->message = WM_MOUSEWHEEL;
      m->wParam = (-120) << 16 | 0; // 1 tick, no modifiers
      break;
    default:
      m->lParam = 0;
      break;
    }
    break;
  case ButtonRelease:
    m->lParam = (e->xbutton.x & 0xffff) | (e->xbutton.y << 16);
    switch( e->xbutton.button ) {
    case Button1:
      m->message = WM_LBUTTONUP;
      break;

    case Button2:
      m->message = WM_MBUTTONUP;
      break;

    case Button3:
      m->message = WM_RBUTTONUP;
      break;
    default:
      m->lParam = 0;
      break;
    }
    break;
  case MotionNotify:
    {
      m->message = WM_MOUSEMOVE;
      do {
  // Spin...
      } while( XCheckTypedWindowEvent( XWindow::getDisplay(), m->hwnd, MotionNotify, e ) );

      RECT r;
      POINT offset = {0, 0};
      OSWINDOWHANDLE hwnd = m->hwnd;

      StdWnd::getWindowRect( hwnd, &r );

      m->lParam = ((e->xmotion.x_root - r.left) & 0xffff) |
                 ((e->xmotion.y_root - r.top) << 16);

      if ( ! (e->xmotion.state & ( Button1Mask | Button2Mask | Button3Mask )) )
        PostMessage( m->hwnd, WM_SETCURSOR, m->hwnd, 0 );
    }
    break;

  case KeyPress:
    m->message = WM_KEYDOWN;
    m->wParam = e->xkey.keycode;
    break;

  case KeyRelease:
    m->message = WM_KEYUP;
    m->wParam = e->xkey.keycode;
    break;

  case Expose:
    {
      //CUT RECT r;
      m->message = WM_PAINT;
      OSREGIONHANDLE invalid = (OSREGIONHANDLE)GetWindowLong( m->hwnd, GWL_INVALIDREGION );
      if ( ! invalid ) {
        invalid = XCreateRegion();
        SetWindowLong( m->hwnd, GWL_INVALIDREGION, (LONG)invalid );
      }
      do {
        XRectangle xr;
        xr.x = e->xexpose.x;
        xr.y = e->xexpose.y;
        xr.width = e->xexpose.width;
        xr.height = e->xexpose.height;
        XUnionRectWithRegion( &xr, invalid, invalid );
      } while( XCheckTypedWindowEvent( XWindow::getDisplay(), m->hwnd, Expose, e ) );
    }
    break;

  case ClientMessage: {
    static int coord = -1;
    static Atom supported = None;
    XClientMessageEvent cme;

    LoadAtoms();

    int message = e->xclient.message_type;
    if ( message == dnd_enter ) {
      if ( e->xclient.data.l[1] & 1 ) {
        Atom actual;
        int format;
        long unsigned int nitems, bytes;
        unsigned char *data = NULL;
      
        XGetWindowProperty( XWindow::getDisplay(), e->xclient.data.l[0],
                dnd_typelist, 0, 65536, True, XA_ATOM,
                &actual, &format, &nitems, &bytes, &data );
      
        Atom *atomdata = (Atom *)data;
      
        supported = None;
        for( int i = 0; i < nitems; i++ ) {
          if ( atomdata[i] == dnd_urilist ) {
            supported = dnd_urilist;
          }
        }
        if ( supported == None ) {
          for( int i = 0; i < nitems; i++ ) {
            if ( atomdata[i] == dnd_textplain ) {
              DebugString( "text/plain found\n" );
              supported = dnd_textplain;
            }
          }
        }
        if ( supported == None ) {
          for( int i = 0; i < nitems; i++ ) {
            if ( atomdata[i] == dnd_mozurl ) {
              supported = dnd_mozurl;
            }
          }
        }
      
        XFree( data );
      } else {
        if ( e->xclient.data.l[2] == dnd_urilist ||
            e->xclient.data.l[3] == dnd_urilist ||
            e->xclient.data.l[4] == dnd_urilist ) {
          supported = dnd_urilist;
        } else if ( e->xclient.data.l[2] == dnd_mozurl ||
              e->xclient.data.l[3] == dnd_mozurl ||
              e->xclient.data.l[4] == dnd_mozurl ) {
          supported = dnd_mozurl;
        }
      }


      // DnD Enter
      return 0;

    } else if ( message == dnd_position ) {
      // DnD Position Notify

      cme.type = ClientMessage;
      cme.message_type = dnd_status;
      cme.format = 32;
      cme.window = e->xclient.data.l[0];
      cme.data.l[0] = e->xclient.window;
      cme.data.l[1] = 1; // Can Accept
      cme.data.l[2] = cme.data.l[3] = 0; // Empty rectangle - give us moves
      cme.data.l[4] = dnd_private; // We're doing our own thing

      if ( coord == -1 && supported != None ) {
        XConvertSelection( XWindow::getDisplay(), dnd_selection, supported,
                           dnd_wa3drop, cme.window, CurrentTime );
      }

      coord = e->xclient.data.l[2];

      XSendEvent( XWindow::getDisplay(), e->xclient.data.l[0], False,
                  NoEventMask, (XEvent *)&cme );

      return 0;

    } else if ( message == dnd_leave ) {
      // DnD Leave
      coord = -1;
      supported = None;

      return 0;

    } else if ( message == dnd_drop ) {
      // DnD Drop

      Window win = e->xclient.data.l[0];

      cme.type = ClientMessage;
      cme.message_type = dnd_finished;
      cme.format = 32;
      cme.window = e->xclient.data.l[0];
      cme.data.l[0] = e->xclient.window;
      cme.data.l[1] = cme.data.l[2] = cme.data.l[3] = cme.data.l[4] = 0;

      XSendEvent( XWindow::getDisplay(), e->xclient.data.l[0], False,
      NoEventMask, (XEvent *)&cme );

      if ( supported != None ) {
        Atom actual;
        int format;
        long unsigned int nitems, bytes;
        unsigned char *data = NULL;
      
        XGetWindowProperty( XWindow::getDisplay(), cme.window, dnd_wa3drop,
                0, 65536, True, supported, &actual,
                &format, &nitems, &bytes,
                &data );
      
        DebugString( StringPrintf( "Drop data (%d):\n%s\n", nitems, data ) );
      
        m->message = WM_DROPFILES;
        m->wParam = coord;
        m->lParam = (LPARAM)data;
      
        coord = -1;
        supported = None;
      
      } else {
        coord = -1;
        supported = None;
        return 0;
      }

      break;

    } else if ( message == winamp_msg ) {
      // Internal Message ...

      m->message = e->xclient.data.l[0];
      m->wParam = e->xclient.data.l[1];
      m->lParam = e->xclient.data.l[2];
      break;

    } else {
      return 0;
    }
    break;
  }//ClientMessage

  case LeaveNotify:
  case EnterNotify:
    m->message = WM_MOUSEMOVE;
    m->lParam = (e->xcrossing.x & 0xffff) | (e->xcrossing.y << 16);

    if ( ! (e->xcrossing.state & ( Button1Mask | Button2Mask | Button3Mask )) )
      PostMessage( m->hwnd, WM_SETCURSOR, m->hwnd, 0 );
    break;

  case FocusIn:
    m->message = WM_SETFOCUS;
    break;

  case FocusOut:
    m->message = WM_KILLFOCUS;
    break;

  default:
    return 0;
  }

  return 1;
}

void XWindow::initContextData( Window w ) {
  long *data;
  XPointer xp;

  if(! XFindContext( XWindow::getDisplay(), w, XWindow::getContext(), &xp ) ) {
    ASSERTPR(0,"std_wnd: Couldn't find context");
  }

  data = (long *)MALLOC( GWL_ENUM_SIZE * sizeof( long ) );

  data[GWL_HWND] = w;
  data[GWL_CLASSNAME] = 0;

  XSaveContext( XWindow::getDisplay(), w, XWindow::getContext(), (char *)data );
}

void XWindow::nukeContextData( Window w ) {
  long *data;
  XPointer xp;

  if ( XFindContext( XWindow::getDisplay(), w, XWindow::getContext(), &xp ) )
    return;

  data = (long *)xp;

  ASSERTPR( data[GWL_HWND] == w, "Not a Wasabi window!"  );

  if ( data[GWL_INVALIDREGION] ) {
    XDestroyRegion( (OSREGIONHANDLE)data[GWL_INVALIDREGION] );
  }

  XDeleteContext( XWindow::getDisplay(), w, XWindow::getContext() );

  FREE( data );
}

void SetWindowLong( OSWINDOWHANDLE h, contextdata type, LONG value ) {
  XPointer data;

  if ( XFindContext( XWindow::getDisplay(), h, XWindow::getContext(), &data ) )
    return;

  ASSERTPR( ((long *)data)[GWL_HWND] == h, "Not a Wasabi window!" );

  ((long *)data)[type] = value;
}

LONG GetWindowLong( OSWINDOWHANDLE h, contextdata type ) {
  XPointer data;

  if ( XFindContext( XWindow::getDisplay(), h, XWindow::getContext(), &data ) )
    return 0;

  ASSERTPR( ((long *)data)[GWL_HWND] == h, "Not a Wasabi window!" );

  return ((long *)data)[type];
}

int GetClassName( OSWINDOWHANDLE hWnd, char *lpClassName, int nMaxCount ) {
  const char *cn = (const char *)GetWindowLong( hWnd, GWL_CLASSNAME );
  STRNCPY(lpClassName, cn, nMaxCount);
  return STRLEN(lpClassName)+1;
}

#if 0 // CUT
void MoveWindowRect( OSWINDOWHANDLE h, int x, int y ) {
  XPointer xp;
  int *data;

  if ( XFindContext( XWindow::getDisplay(), h, XWindow::getContext(), &xp ) )
    return;

  data = (int *)xp;

  ASSERT( data[GWL_HWND] == h );

  data[GWL_RECT_RIGHT]  -= data[GWL_RECT_LEFT] - x;
  data[GWL_RECT_BOTTOM] -= data[GWL_RECT_TOP] - y;
  data[GWL_RECT_LEFT]   = x;
  data[GWL_RECT_TOP]    = y;
}
#endif

#if 0 // CUT
void SetWindowRect( OSWINDOWHANDLE h, RECT *r ) {
  int *data;
  XPointer xp;

  if ( XFindContext( XWindow::getDisplay(), h, XWindow::getContext(), &xp ) )
    return;

  data = (int *)xp;

  ASSERT( data[GWL_HWND] == h );

  data[GWL_RECT_LEFT]   = r->left;
  data[GWL_RECT_TOP]    = r->top;
  data[GWL_RECT_RIGHT]  = r->right;
  data[GWL_RECT_BOTTOM] = r->bottom;
}
#endif

#if 0 // CUT: Use StdWnd::getWindowRect
//WaI> Keith's old code, here to fall back on in case I really messed up.
int GetWindowRect( OSWINDOWHANDLE h, RECT *r ) {
  int *data;
  XPointer xp;

  if ( XFindContext( XWindow::getDisplay(), h, XWindow::getContext(), &xp ) )
    return 0;

  data = (int *)xp;

  ASSERT( data[GWL_HWND] == h );

  r->left   = data[GWL_RECT_LEFT];
  r->top    = data[GWL_RECT_TOP];
  r->right  = data[GWL_RECT_RIGHT];
  r->bottom = data[GWL_RECT_BOTTOM];

  POINT offset = { 0, 0};
  while( (h = data[GWL_PARENT]) != XWindow::RootWin() ) {
    if ( XFindContext( XWindow::getDisplay(), h, XWindow::getContext(), &xp ) )
      return 0;

    data = (int *)xp;

    ASSERT( data[GWL_HWND] == h );

    offset.x += data[GWL_RECT_LEFT];
    offset.y += data[GWL_RECT_TOP];
  }
  r->left += offset.x;
  r->top += offset.y;
  r->right += offset.x;
  r->bottom += offset.y;

  return 1;
}
#endif

void InvalidateRect( OSWINDOWHANDLE h, const RECT *r, BOOL ) {
  OSREGIONHANDLE invalid = (OSREGIONHANDLE)GetWindowLong( h, GWL_INVALIDREGION );
  if ( ! invalid ) {
    invalid = XCreateRegion();
    SetWindowLong( h, GWL_INVALIDREGION, (LONG)invalid );
  }

  XRectangle xr;
  if ( r == NULL ) {
    RECT rct;
    StdWnd::getClientRect( h, &rct );
  } else {
    xr.x = r->left;
    xr.y = r->top;
    xr.width = r->right - r->left;
    xr.height = r->bottom - r->top;
  }

  XUnionRectWithRegion( &xr, invalid, invalid );

  PostMessage( h, WM_PAINT, 0, 0 );
}

void InvalidateRgn( OSWINDOWHANDLE h, OSREGIONHANDLE r, BOOL ) {
  OSREGIONHANDLE invalid = (OSREGIONHANDLE)GetWindowLong( h, GWL_INVALIDREGION );

  if ( ! invalid ) {
    invalid = XCreateRegion();
    SetWindowLong( h, GWL_INVALIDREGION, (LONG)invalid );
  }

  ASSERT( r != invalid );
  XUnionRegion( invalid, r, invalid );

  PostMessage( h, WM_PAINT, 0, 0 );
}

void ValidateRect( OSWINDOWHANDLE h, const RECT *r ) {
  OSREGIONHANDLE invalid = (OSREGIONHANDLE)GetWindowLong( h, GWL_INVALIDREGION );
  if ( ! invalid ) return;

  XRectangle xr;
  if ( r == NULL ) {
    XDestroyRegion( invalid );
    SetWindowLong( h, GWL_INVALIDREGION, 0 );
    return;
  }

  xr.x = r->left;
  xr.y = r->top;
  xr.width = r->right - r->left;
  xr.height = r->bottom - r->top;

  OSREGIONHANDLE tmp = XCreateRegion();
  XUnionRectWithRegion( &xr, tmp, tmp );
  XSubtractRegion( invalid, tmp, invalid );
  XDestroyRegion( tmp );
}

void ValidateRgn( OSWINDOWHANDLE h, OSREGIONHANDLE r ) {
  OSREGIONHANDLE invalid = (OSREGIONHANDLE)GetWindowLong( h, GWL_INVALIDREGION );
  if ( ! invalid ) return;

  ASSERT( r != invalid );
  XSubtractRegion( invalid, r, invalid );
}


void TranslateMessage( MSG *m ) {
  if ( m->message != WM_CHAR && m->message != WM_KEYDOWN &&
       m->message != WM_KEYUP )
    return;

  int index = !!( Std::keyDown( VK_SHIFT ));

  m->wParam = XKeycodeToKeysym( XWindow::getDisplay(), m->wParam, index );
}

void PostMessage( OSWINDOWHANDLE win, UINT msg, WPARAM wParam, LPARAM lParam ) {
  XEvent e;

  LoadAtoms();

  e.type = ClientMessage;
  e.xclient.window = win;
  e.xclient.message_type = winamp_msg;
  e.xclient.format = 32;
  e.xclient.data.l[0] = msg;
  e.xclient.data.l[1] = wParam;
  e.xclient.data.l[2] = lParam;

  XSendEvent( XWindow::getDisplay(), win, FALSE, NoEventMask, &e );
}

void PostQuitMessage( int i ) {
  PostMessage( None, WM_QUIT, i, 0 );
}

