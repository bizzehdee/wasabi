#ifndef _WINCE_H
#define _WINCE_H

#ifndef _WIN32_WCE
#error this file is only for WindowsCE
#endif

#ifndef _WASABI_PLATFORM_H
#error this file should only be included from platform.h
#endif

// this should be the *only* place windows.h gets included!
#include <windows.h>
#include <stdio.h>

#if defined(_MSC_VER)	// msvc
# define WASABIDLLEXPORT __declspec(dllexport)
# if _MSC_VER >= 1100
#  define NOVTABLE __declspec(novtable)
# endif
#endif

#define THREADSTORAGE __declspec(thread)

#define _TRY __try
#define _EXCEPT(x) __except(x)

#define OSPIPE HANDLE
#define OSPROCESSID int
#define OSMODULEHANDLE  HINSTANCE
#define INVALIDOSMODULEHANDLE  ((OSMODULEHANDLE)0)
#define OSWINDOWHANDLE  HWND
#define INVALIDOSWINDOWHANDLE  ((OSWINDOWHANDLE)0)
#define OSICONHANDLE  HICON
#define INVALIDOSICONHANDLE  ((OSICONHANDLE)0)
#define OSCURSORHANDLE  HICON
#define INVALIDOSCURSORHANDLE  ((OSCURSORHANDLE)0)
#define OSTHREADHANDLE  HANDLE
#define INVALIDOSTHREADHANDLE ((OSTHREADHANDLE)0)
#define OSREGIONHANDLE  HRGN
#define INVALIDOSREGIONHANDLE  ((OSREGIONHANDLE)0)

#define _asm __asm

#endif
