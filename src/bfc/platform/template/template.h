#error Template is a fake platform. Do not use.
// Please read the README

#ifndef _WASABI_TEMPLATE_H
#define _WASABI_TEMPLATE_H

/* --------------------------------------------------------------- */
// Always use protection!

#ifndef TEMPLATE
#error this file is only for the Template platform
#endif

#ifndef _WASABI_PLATFORM_H
#ifndef _WASABI_TEMPLATE_H_ALLOW_INCLUDE // You must know what you are doing to include this file.
#error only include from bfc/platform/platform.h
#endif
#endif


//
// The following sections are not neccessarily in any particular order.
//

/* --------------------------------------------------------------- */
// include your subplatforms

// define all first, in case of interdependant parts of some Subplatforms.
#define SUBPLATFORM1
#define SUBPLATFORM2
#define SUBPLATFORM3
// then include.
#include <bfc/platform/subplatform1/subplatform1.h>
#include <bfc/platform/subplatform2/subplatform2.h>
#include <bfc/platform/subplatform3/subplatform3.h>

/* --------------------------------------------------------------- */
// include your platform files, define your types, etc...

#include <tmpltsys.h>
#include <tmpltstuff.h>


// For Top-level platforms, you must make sure these symbols are defined
// either here or in a Subplatform. Default values are given here for some.
// Though #define'd here, typedef's are also encouraged.
//
// WaI> Feel free to add symbols I missed.


#define _TRY
#define _EXCEPT(x)

// blank if no symbol exists on your platform.
#define NOVTABLE

#define THREADSTORAGE

// if your platform has a compatible (16-byte) GUID, define it and _GUID_DEFINED
//    see guid.h
#define GUID
#define _GUID_DEFINED

// if your platform has GUID (in)equality operators.
#ifdef __cplusplus
#define GUID_EQUALS_DEFINED
static __inline int operator ==(const GUID &a, const GUID &b);
static __inline int operator !=(const GUID &a, const GUID &b);
#endif	//__cplusplus

#define RECT

#define OSPIPE
#define OSPROCESSID

#define MAX_PATH 8192

#define OSMODULEHANDLE
#define INVALIDOSMODULEHANDLE  ((OSMODULEHANDLE)0)

#define OSWINDOWHANDLE
#define INVALIDOSWINDOWHANDLE  ((OSWINDOWHANDLE)0)

#define OSICONHANDLE
#define INVALIDOSICONHANDLE  ((OSICONHANDLE)0)

#define OSCURSORHANDLE
#define INVALIDOSCURSORHANDLE  ((OSCURSORHANDLE)0)

#define OSTHREADHANDLE
#define INVALIDOSTHREADHANDLE ((OSTHREADHANDLE)0)

#define OSREGIONHANDLE
#define INVALIDOSREGIONHANDLE  ((OSREGIONHANDLE)0)

/* --------------------------------------------------------------- */
#endif//_WASABI_TEMPLATE_H
