#ifndef _WASABI_XWINDOW_TYPES_H
#define _WASABI_XWINDOW_TYPES_H

//
// This file contains the types that need to be defined for POSIX with XWINDOW.
//
//

// Fucking Region and Font and Cursor namespace conflicts
#define _XRegion _XRegion
#define Region OSREGIONHANDLE
#define Font OSFONTHANDLE
#define Cursor OSCURSORHANDLE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/Xinerama.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/xpm.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#undef _XRegion
#undef Region
#undef Font
#undef Cursor

#ifdef __INCLUDE_GTK
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#endif // gtk

typedef Window OSWINDOWHANDLE;
//typedef Region OSREGIONHANDLE;
//typedef Font OSFONTHANDLE;
//typedef Cursor OSCURSORHANDLE;
typedef Pixmap OSICONHANDLE;

//CUT? typedef void *HPEN;

#define IVALIDOSWINDOWHANDLE None

#ifdef WASABI_COMPILE_FONTS

#ifdef __INCLUDE_FREETYPE
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#endif // freetype

//typedef FT_Face HFONT;
#endif // fonts

#if 1
//WaI> this will change. use Std:: or class Rect.
//     Do not use member selection ('.' or '->') on a RECT outside of a platform-specfic code.

// I know about XRectangle, but it's easier to recreate this than it is
// to merge it with the rest of the code
#define WASABI_WIN32RECT
typedef struct { int left, top, right, bottom; } RECT;
#else
typedef XRectangle RECT;
#endif

typedef struct { int x, y; } POINT;


// Fix this for canvas
//CUT typedef void * PAINTSTRUCT;
// The HDC knows what it's drawing on

typedef struct hdc_typ {
  GC gc;
  Drawable d;
  OSREGIONHANDLE clip;
} *HDC;

// Pixmaps don't have associated width and height, at least not that I know of
typedef struct {
  Pixmap p;
  int bmWidth;
  int bmHeight;
  XShmSegmentInfo *shmseginfo;

} BITMAP, HBITMAP;

#define NULLREGION    1
#define SIMPLEREGION  2
#define COMPLEXREGION 3

#define ALL_EVENTS       ExposureMask | StructureNotifyMask | \
                         KeyPressMask | KeyReleaseMask | \
                         ButtonPressMask | ButtonReleaseMask | \
                         PointerMotionMask | FocusChangeMask | \
                         EnterWindowMask | LeaveWindowMask


#define NO_INPUT_EVENTS  ExposureMask | StructureNotifyMask | \
                         PointerMotionMask | FocusChangeMask | \
                         EnterWindowMask | LeaveWindowMask


// Fix this with editwnd!
typedef void * WNDPROC;
typedef void * DRAWITEMSTRUCT;

typedef void (*TIMERPROC)(OSWINDOWHANDLE, UINT, UINT, DWORD);

typedef struct {
  OSWINDOWHANDLE hwnd;
  UINT message;
  WPARAM wParam;
  LPARAM lParam;
  DWORD time;
  POINT pt;
} MSG;

#define VK_MENU    (XK_Alt_L | (XK_Alt_R << 16))
#define VK_MBUTTON (XK_Meta_L | (XK_Meta_R << 16))
#define VK_SHIFT   (XK_Shift_L | (XK_Shift_R << 16))
#define MK_RBUTTON ((XK_VoidSymbol << 16) | 1)
#define MK_LBUTTON ((XK_VoidSymbol << 16) | 2)
#define MK_MBUTTON ((XK_VoidSymbol << 16) | 3)
#define VK_CONTROL (XK_Control_L | (XK_Control_R << 16))
#define VK_DELETE  (XK_Delete | (XK_KP_Delete << 16))
#define VK_RETURN  (XK_Return)
#define VK_ESCAPE  (XK_Escape)
#define VK_DOWN    (XK_Down)
#define VK_UP      (XK_Up)
#define VK_LEFT    (XK_Left)
#define VK_RIGHT   (XK_Right)
#define VK_HOME    (XK_Home)
#define VK_END     (XK_End)
#define VK_PRIOR   (XK_Prior)
#define VK_NEXT    (XK_Next)
#define VK_BACK    (XK_BackSpace)
#define VK_F1      (XK_F1)
#define VK_SPACE   (XK_space)

#define INVALID_HANDLE_VALUE NULL

enum {
  WA_ACTIVE, WA_CLICKACTIVE,
};

enum {
  SC_CLOSE,
};

enum {
  DT_LEFT,
  DT_CENTER,
  DT_RIGHT,
  DT_VCENTER,
  DT_WORDBREAK,
  DT_SINGLELINE,
  DT_NOPREFIX,
  DT_PATH_ELLIPSIS,
  DT_END_ELLIPSIS,
  DT_MODIFYSTRING,
  DT_CALCRECT,

};


#endif // _WASABI_XWINDOW_TYPES_H
