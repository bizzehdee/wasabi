#include <precomp.h>
#include "nsguid.h"

#ifdef LINUX
#ifndef WASABI_PLATFORM_DARWIN
#include <uuid/uuid.h>
#else
#include <CoreFoundation/CFUUID.h>
#endif
#endif

char *nsGUID::toChar(const GUID &guid, char *target) {
  // {1B3CA60C-DA98-4826-B4A9-D79748A5FD73}
  SPRINTF( target, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
    (int)guid.Data1, (int)guid.Data2, (int)guid.Data3,
    (int)guid.Data4[0], (int)guid.Data4[1],
    (int)guid.Data4[2], (int)guid.Data4[3],
    (int)guid.Data4[4], (int)guid.Data4[5],
    (int)guid.Data4[6], (int)guid.Data4[7] );

  return target;
}

GUID nsGUID::fromChar(const char *source) {

  if (source == NULL) return INVALID_GUID;

  if (STRCASEEQL(source, "@all@")) {
    static GUID g={ 0xFFFFFFFF, 0xFFFF, 0xFFFF, { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } };
    return g;
  }
  if (STRNINCMP(source,"guid:{")==0) {
    source+=5;
  }
  if(STRICMP(source,"guid:avs")==0) {
    static GUID g={ 10, 12, 16, { 255, 123, 1, 1, 66, 99, 69, 12 } };
    return g;
  }
  if (STRICMP(source,"guid:pl")==0 || STRICMP(source,"guid:playlist")==0) {
    static GUID g={ 0x45f3f7c1, 0xa6f3, 0x4ee6, { 0xa1, 0x5e, 0x12, 0x5e, 0x92, 0xfc, 0x3f, 0x8d } };
    return g;
  }
  if (STRICMP(source,"guid:ml")==0 || STRICMP(source,"guid:musiclibrary")==0 || STRICMP(source,"guid:library")==0) {
    static GUID g={ 0x6b0edf80, 0xc9a5, 0x11d3, { 0x9f, 0x26, 0x00, 0xc0, 0x4f, 0x39, 0xff, 0xc6 } };
    return g;
  }
  if (STRICMP(source,"guid:default")==0) {
    static GUID g={ 0xFFFFFFFF, 0xFFFF, 0xFFFF, { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } };
    return g;
  }

  if (!STRCHR(source, '{') || !STRCHR(source, '}')) return INVALID_GUID;

  GUID guid;
  int Data1, Data2, Data3;
  int Data4[8];

  // {1B3CA60C-DA98-4826-B4A9-D79748A5FD73}
  int n = SSCANF( source, " { %08x - %04x - %04x - %02x%02x - %02x%02x%02x%02x%02x%02x } ",
    &Data1, &Data2, &Data3, Data4 + 0, Data4 + 1,
    Data4 + 2, Data4 + 3, Data4 + 4, Data4 + 5, Data4 + 6, Data4 + 7 );

  if (n != 11) return INVALID_GUID;

  // Cross assign all the values
  guid.Data1 = Data1;
  guid.Data2 = Data2;
  guid.Data3 = Data3;
  guid.Data4[0] = Data4[0];
  guid.Data4[1] = Data4[1];
  guid.Data4[2] = Data4[2];
  guid.Data4[3] = Data4[3];
  guid.Data4[4] = Data4[4];
  guid.Data4[5] = Data4[5];
  guid.Data4[6] = Data4[6];
  guid.Data4[7] = Data4[7];

  return guid;
}

char *nsGUID::toCode(const GUID &guid, char *target) {
  //{ 0x1b3ca60c, 0xda98, 0x4826, { 0xb4, 0xa9, 0xd7, 0x97, 0x48, 0xa5, 0xfd, 0x73 } };
  SPRINTF( target, "{ 0x%08x, 0x%04x, 0x%04x, { 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x } };",
    (int)guid.Data1, (int)guid.Data2, (int)guid.Data3,
    (int)guid.Data4[0], (int)guid.Data4[1],
    (int)guid.Data4[2], (int)guid.Data4[3],
    (int)guid.Data4[4], (int)guid.Data4[5],
    (int)guid.Data4[6], (int)guid.Data4[7] );

  return target;
}

GUID nsGUID::fromCode(const char *source) {

  GUID guid;
  int Data1, Data2, Data3;
  int Data4[8];

  //{ 0x1b3ca60c, 0xda98, 0x4826, { 0xb4, 0xa9, 0xd7, 0x97, 0x48, 0xa5, 0xfd, 0x73 } };
  SSCANF( source, " { 0x%08x, 0x%04x, 0x%04x, { 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x } } ; ",
    &Data1, &Data2, &Data3, Data4 + 0, Data4 + 1,
    Data4 + 2, Data4 + 3, Data4 + 4, Data4 + 5, Data4 + 6, Data4 + 7 );

  // Cross assign all the values
  guid.Data1 = Data1;
  guid.Data2 = Data2;
  guid.Data3 = Data3;
  guid.Data4[0] = Data4[0];
  guid.Data4[1] = Data4[1];
  guid.Data4[2] = Data4[2];
  guid.Data4[3] = Data4[3];
  guid.Data4[4] = Data4[4];
  guid.Data4[5] = Data4[5];
  guid.Data4[6] = Data4[6];
  guid.Data4[7] = Data4[7];

  return guid;
}

int nsGUID::compare(const GUID &a, const GUID &b) {
  int delta1 = a.Data1 - b.Data1;
  if (delta1 == 0) {
    int delta2 = a.Data2 - b.Data2;
    if (delta2 == 0) {
      int delta3 = a.Data3 - b.Data3;
      if (delta3 == 0) {
        int i;
        for (i = 0; i < 8; i++ ) {
          int delta4 = a.Data4[i] - b.Data4[i];
          if (delta4 == 0) {
            continue; // :)
          } else {
            return delta4;
          }
        }
      } else {
        return delta3;
      }
    } else {
      return delta2;
    }
  } else {
    return delta1;
  }
  return 0;
}

#ifdef WASABI_COMPILE_CREATEGUID
void nsGUID::createGuid(GUID *g) {
#ifdef WIN32
  CoCreateGuid(g);
#endif
#ifdef LINUX
 #ifndef WASABI_PLATFORM_DARWIN
  uuid_t uid;
  uuid_generate(uid);
  MEMCPY(g, &uid, MIN((int)sizeof(GUID), (int)sizeof(uuid_t)));
 #else
  CFUUIDRef uid;
  uid = CFUUIDCreate(NULL);
  CFUUIDBytes bytes = CFUUIDGetUUIDBytes(uid);
  MEMCPY(g, &bytes, MIN((int)sizeof(GUID), (int)sizeof(CFUUIDRef)));
 #endif
#endif
}
#endif





