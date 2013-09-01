#ifndef _WASABI_PLATFORM_H
#define _WASABI_PLATFORM_H

/* --------------------------------------------------------------- */
// Standard int types

typedef unsigned int UINT;
typedef signed int SINT;

typedef unsigned char UCHAR;
typedef signed char SCHAR;

typedef unsigned long ARGB32;
typedef unsigned long RGB32;

typedef unsigned long ARGB24;
typedef unsigned long RGB24;

typedef unsigned short ARGB16;
typedef unsigned short RGB16;

typedef unsigned long FOURCC;

#include <bfc/std_mkncc.h>  // for MKnCC

#ifdef NULL
#undef NULL
#endif
#ifndef NULL
#define NULL 0
#endif

/* --------------------------------------------------------------- */
// Top-level platforms:

#if defined(WIN32)
#define WASABI_PLATFORM_WIN32
# include <bfc/platform/win32/win32.h>

#elif defined(_WIN32_WCE)
#define WASABI_PLATFORM_WINCE
# include <bfc/platform/wince/wince.h>

#elif defined(LINUX)
#define WASABI_PLATFORM_LINUX
# include <bfc/platform/linux/linux.h>

#elif defined(DARWIN)
#define WASABI_PLATFORM_DARWIN
# include <bfc/platform/darwin/darwin.h>

#else
#error No recognized platform defined!
#endif

/* --------------------------------------------------------------- */

// define GUID
#include <bfc/platform/guid.h>

// C includes

#include <stdio.h>
#include <stdlib.h>
#ifdef __cplusplus
#include <new>
#else
//WaI> new.h is just a backwards compatibility for older C++ implementations.
//     it doesn't make sense to include it for non-C++
//CUT #include <new.h>
#endif
#include <limits.h>

// Some default values
#ifndef INVALIDOSMODULEHANDLE
#define INVALIDOSMODULEHANDLE  ((OSMODULEHANDLE)0)
#endif
#ifndef INVALIDOSWINDOWHANDLE
#define INVALIDOSWINDOWHANDLE  ((OSWINDOWHANDLE)0)
#endif
#ifndef INVALIDOSICONHANDLE
#define INVALIDOSICONHANDLE  ((OSICONHANDLE)0)
#endif
#ifndef INVALIDOSCURSORHANDLE
#define INVALIDOSCURSORHANDLE  ((OSCURSORHANDLE)0)
#endif
#ifndef INVALIDOSTHREADHANDLE
#define INVALIDOSTHREADHANDLE ((OSTHREADHANDLE)0)
#endif
#ifndef INVALIDOSREGIONHANDLE
#define INVALIDOSREGIONHANDLE  ((OSREGIONHANDLE)0)
#endif

#ifndef RGB
#define RGB( r, g, b ) ((((r)&0xFF)<<16)|(((g)&0xFF)<<8)|((b)&0xFF))
#endif



// Ode macro keyworkds
#define DISPATCH_   // makes a method dispatchable, automatically assigns a free ID (requires Interface)
#define DISPATCH(x) // makes a method dispatchable and specifies its ID (not duplicate check, requires Interface)
#define NODISPATCH  // prevents a method from being dispatched if the class is marked for dispatching by default
#define EVENT       // marks a method as being an event to which other classes can connect to receive notification (used by Script and DependentItem helpers)
#define SCRIPT      // exposes a method to script interfaces (requires Script helper)
#define IN          // Input parameter
#define OUT         // Output parameter
#define INOUT       // Input/Output parameter

#endif//_WASABI_PLATFORM_H
