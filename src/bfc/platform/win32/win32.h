#ifndef _WIN32_H
#define _WIN32_H

#ifndef WIN32
#error this file is only for win32
#endif

#ifndef _WASABI_PLATFORM_H
#error this file should only be included from platform.h
#endif

// this should be the *only* place windows.h gets included!
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>

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

//MS is not up-to-date with standards:
//(this was added in C99)
#define va_copy(dest, src) (dest=src)

#ifndef _DEBUG
// release optimizations
#pragma optimize("gsy",on)
#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma comment(linker,"/RELEASE")
// set the 512-byte alignment
#pragma comment(linker,"/opt:nowin98")
#endif
#endif	// _DEBUG

#if defined(UNICODE) || defined(_UNICODE)
#define UTF8 1
#endif

#endif
