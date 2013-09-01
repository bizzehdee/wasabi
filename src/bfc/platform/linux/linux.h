#ifndef _WASABI_LINUX_H
#define _WASABI_LINUX_H

#ifndef WASABI_PLATFORM_LINUX
#error this file is only for the Linux platform
#endif

#ifndef _WASABI_PLATFORM_H
#ifndef _WASABI_LINUX_H_ALLOW_INCLUDE // You must know what you are doing to include this file.
#error only include from bfc/platform/platform.h
#endif
#endif

//Subplatforms:

#define WASABI_PLATFORM_POSIX
#define POSIX /*DEPRECATED*/

#ifdef X11
# define WASABI_PLATFORM_XWINDOW
# define XWINDOW /*DEPRECATED*/
#endif

#include <bfc/platform/posix/posix.h>
#ifdef WASABI_PLATFORM_XWINDOW
#include <bfc/platform/xwindow/xwindow.h>
#endif

#endif//_WASABI_LINUX_H
