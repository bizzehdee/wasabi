#ifndef _WASABI_DARWIN_H
#define _WASABI_DARWIN_H

#ifndef WASABI_PLATFORM_DARWIN
#error this file is only for the Darwin platform
#endif
#define LINUX
#ifndef _WASABI_PLATFORM_H
#ifndef _WASABI_LINUX_H_ALLOW_INCLUDE // You must know what you are doing to include this file.
#error only include from bfc/platform/platform.h
#endif
#endif

//Subplatforms:
#define WASABI_PLATFORM_POSIX
#define POSIX /*DEPRECATED*/
#ifdef WASABI_COMPILE_WND
# define XWINDOW
#endif

#ifdef XWINDOW
# include <bfc/platform/xwindow/xwindow.h>
#else // wnd
typedef void * OSWINDOWHANDLE;
typedef void * OSREGIONHANDLE;
typedef void * OSFONTHANDLE;
typedef void * OSCURSORHANDLE;
typedef void * OSICONHANDLE;
#endif
#include <bfc/platform/posix/posix.h>


#ifdef WASABI_COMPILE_WND

#ifdef WASABI_COMPILE_FONTS

#ifdef __INCLUDE_FREETYPE
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#endif // freetype

//typedef FT_Face HFONT;
#endif // fonts

#endif // wnd

#ifdef WASABI_COMPILE_WND
// Fix this for canvas
typedef void * PAINTSTRUCT;
#endif

#ifndef WASABI_COMPILE_WND
#ifndef None
#define None (OSWINDOWHANDLE)0
#endif
#endif

#ifdef WASABI_COMPILE_WND
// Fix this with editwnd!
typedef void * WNDPROC;
typedef void * DRAWITEMSTRUCT;

#define VK_MENU    (XK_Alt_L | (XK_Alt_R << 16))
#define VK_MBUTTON (XK_Meta_L | (XK_Meta_R << 16))
#define VK_SHIFT   (XK_Shift_L | (XK_Shift_R << 16))
#define MK_RBUTTON ((XK_VoidSymbol << 16) | 1)
#define MK_LBUTTON ((XK_VoidSymbol << 16) | 2)
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
#endif



#endif//_WASABI_DARWIN_H

