#ifndef _COMMON_H
#define _COMMON_H

#ifdef WIN32
// disable "dll-interface to be used by clients of class" warning message.
#pragma warning(disable: 4251)
#endif /* WIN32 */

#ifndef COMEXP

#if defined(COMMON_EXPORT)
#define COMEXP __declspec (dllexport) 
#elif defined(COMMON_IMPORT)
#define COMEXP __declspec (dllimport)
#else
#define COMEXP
#endif

#endif

#ifndef __cplusplus 
#define EXTC extern 
#else
#define EXTC extern "C" 
#endif

#include <bfc/std.h>

#endif /* _COMMON_H */
