#ifndef _WASABI_GUID_H
#define _WASABI_GUID_H

#include <bfc/platform/platform.h>


#ifndef GUID_DEFINED
#define GUID_DEFINED

typedef struct _GUID {
  unsigned int Data1;
  unsigned short Data2;
  unsigned short Data3;
  unsigned char Data4[8];
} GUID;

#ifndef _REFCLSID_DEFINED
#define REFGUID const GUID &
#define _REFCLSID_DEFINED
#endif

#endif // !GUID_DEFINED
/*
#ifdef DARWIN
#include <string.h>
#endif
#ifdef WIN32
// this is for GUID == and !=
#include <objbase.h>
#ifndef GUID_EQUALS_DEFINED
#define GUID_EQUALS_DEFINED
#endif
#else

#ifdef __cplusplus
#ifndef GUID_EQUALS_DEFINED
#define GUID_EQUALS_DEFINED
static __inline int operator ==(const GUID &a, const GUID &b) {
  return !memcmp(&a, &b, sizeof(GUID));
}
static __inline int operator !=(const GUID &a, const GUID &b) {
  return !!memcmp(&a, &b, sizeof(GUID));
}
#endif	//GUID_EQUALS_DEFINED
#endif	//__cplusplus
#endif	//WIN32
*/


static const GUID INVALID_GUID = { 0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0} };
static const GUID GENERIC_GUID = { 0xFFFFFFFF, 0xFFFF, 0xFFFF, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} };

#endif
