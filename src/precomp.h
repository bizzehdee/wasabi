#ifndef _PRECOMP_H
#define _PRECOMP_H

#ifdef LINUX
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#endif

#ifdef WIN32

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN    // Exclude rarely-used stuff from Windows headers

#ifdef AFXMEM
#define _AFX_NOFORCE_LIBS
#define ATLASSERT(x) ASSERT(x)
#include <afx.h>
#endif

#ifdef _AFX // Are we linking with MFC ?

#define VC_EXTRALEAN    // Exclude rarely-used stuff from Windows headers

#ifndef STRICT
#define STRICT
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif


#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#include <afxpriv.h>        // WM_KICKIDLE
#endif  // _AFX_NO_OLE_SUPPORT

#endif  // _AFX

#endif  // WIN32

//stuff for doxygen
#define IN
#define OUT

// Most used wasabi headers
#include <wasabicfg.h>
#include <bfc/platform/platform.h>

#include <bfc/assert.h>
#include <bfc/common.h>
#include <bfc/std.h>
#include <bfc/paramobj.h>
#include <bfc/ptrlist.h>
#include <bfc/foreach.h>
#include <bfc/stack.h>
#include <bfc/tlist.h>
#include <bfc/string/string.h>
#include <bfc/dispatch.h>
#include <bfc/guid.h>
//CUT #include <bfc/nsguid.h>
#include <bfc/rect.h>

#ifdef WASABI_COMPILE_SVC
#include <api/service/servicei.h>
#endif

//CUT? #ifdef WASABI_COMPILE_WND

#if defined(WASABI_PLATFORM_WIN32) || defined(WASABI_PLATFORM_XWINDOW)
#include <wnd/rootwnd.h>
#endif

#ifdef WASABI_COMPILE_SKIN
#include <wnd/wnds/guiobjwnd.h>
#include <api/script/guiobject.h>
#endif

//CUT? #endif

#ifdef WASABI_COMPILE_SCRIPT
#include <api/script/scriptobj.h>
#include <api/script/objcontroller.h>
#include <api/script/scriptvar.h>
#endif


using namespace wasabi;

#endif // _PRECOMP_H
