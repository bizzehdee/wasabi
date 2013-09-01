#include "precomp.h"

#include "guid.h"

#ifdef LINUX
#ifndef WASABI_PLATFORM_DARWIN
#include <uuid/uuid.h>
#else
#include <CoreFoundation/CFUUID.h>
#endif
#endif

String Guid::toChar() const {
  // {1B3CA60C-DA98-4826-B4A9-D79748A5FD73}
  StringPrintf ret("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
    (int)g.Data1, (int)g.Data2, (int)g.Data3,
    (int)g.Data4[0], (int)g.Data4[1],
    (int)g.Data4[2], (int)g.Data4[3],
    (int)g.Data4[4], (int)g.Data4[5],
    (int)g.Data4[6], (int)g.Data4[7] );

  return ret;
}

void Guid::fromChar(const char *source) {

  if (source == NULL) {
    g = INVALID_GUID;
    return;
  }

  if (STRCASEEQL(source, "@all@")) {
    g = GENERIC_GUID;
    return;
  }
  if (STRNINCMP(source,"guid:{")==0) {
    source+=5;
  }
  if (STRICMP(source,"guid:default")==0) {
    g = GENERIC_GUID;
    return;
  }

  if (!STRCHR(source, '{') || !STRCHR(source, '}')) {
    g = INVALID_GUID;
    return;
  }

  int Data1, Data2, Data3;
  int Data4[8];

  // {1B3CA60C-DA98-4826-B4A9-D79748A5FD73}
  int n = SSCANF( source, " { %08x - %04x - %04x - %02x%02x - %02x%02x%02x%02x%02x%02x } ",
    &Data1, &Data2, &Data3, Data4 + 0, Data4 + 1,
    Data4 + 2, Data4 + 3, Data4 + 4, Data4 + 5, Data4 + 6, Data4 + 7 );

  if (n != 11) {
    g = INVALID_GUID;
    return;
  }
  
  // Cross assign all the values
  g.Data1 = Data1;
  g.Data2 = Data2;
  g.Data3 = Data3;
  g.Data4[0] = Data4[0];
  g.Data4[1] = Data4[1];
  g.Data4[2] = Data4[2];
  g.Data4[3] = Data4[3];
  g.Data4[4] = Data4[4];
  g.Data4[5] = Data4[5];
  g.Data4[6] = Data4[6];
  g.Data4[7] = Data4[7];

  return;
}

String Guid::toCode() const {
  //{ 0x1b3ca60c, 0xda98, 0x4826, { 0xb4, 0xa9, 0xd7, 0x97, 0x48, 0xa5, 0xfd, 0x73 } };
  StringPrintf ret("{ 0x%08x, 0x%04x, 0x%04x, { 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x } };",
    (int)g.Data1, (int)g.Data2, (int)g.Data3,
    (int)g.Data4[0], (int)g.Data4[1],
    (int)g.Data4[2], (int)g.Data4[3],
    (int)g.Data4[4], (int)g.Data4[5],
    (int)g.Data4[6], (int)g.Data4[7] );

  return ret;
}

void Guid::fromCode(const char *source) {
  //SSCANF assumes 4-byte ints.
  int Data1, Data2, Data3;
  int Data4[8];
  
  //{ 0x1b3ca60c, 0xda98, 0x4826, { 0xb4, 0xa9, 0xd7, 0x97, 0x48, 0xa5, 0xfd, 0x73 } };
  SSCANF( source, " { 0x%08x, 0x%04x, 0x%04x, { 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x } } ; ",
    &Data1, &Data2, &Data3, Data4 + 0, Data4 + 1,
    Data4 + 2, Data4 + 3, Data4 + 4, Data4 + 5, Data4 + 6, Data4 + 7 );
  
  // Cross assign all the values
  g.Data1 = Data1;
  g.Data2 = Data2;
  g.Data3 = Data3;
  g.Data4[0] = Data4[0];
  g.Data4[1] = Data4[1];
  g.Data4[2] = Data4[2];
  g.Data4[3] = Data4[3];
  g.Data4[4] = Data4[4];
  g.Data4[5] = Data4[5];
  g.Data4[6] = Data4[6];
  g.Data4[7] = Data4[7];
  
  return;
}


void Guid::createGuid() {
#ifdef WIN32
  CoCreateGuid(&g);
#endif
#ifdef LINUX
 #ifndef WASABI_PLATFORM_DARWIN
  uuid_t uid;
  uuid_generate(uid);
  MEMCPY(&g, &uid, MIN((int)sizeof(GUID), (int)sizeof(uuid_t)));
 #else
  CFUUIDRef uid;
  uid = CFUUIDCreate(NULL);
  CFUUIDBytes bytes = CFUUIDGetUUIDBytes(uid);
  MEMCPY(&g, &bytes, MIN((int)sizeof(GUID), (int)sizeof(CFUUIDRef)));
 #endif
#endif
}

void Guid::setToInvalid() { g = INVALID_GUID; }
void Guid::setToGeneric() { g = GENERIC_GUID; }

int Guid::compareTo(const Guid &_g) {
  Guid g(_g);
  return compare(*this, g);
}

int Guid::compare(const Guid &_a, const Guid &_b) {
  return compare(_a.getGuid(), _b.getGuid());
}

int Guid::compare(const GUID &a, const GUID &b) {
  if(a.Data1 < b.Data1) return -1;
  if(a.Data1 > b.Data1) return 1;
  if(a.Data2 < b.Data2) return -1;
  if(a.Data2 > b.Data2) return 1;
  if(a.Data3 < b.Data3) return -1;
  if(a.Data3 > b.Data3) return 1;
  for(int i=0; i<8; i++) {
    if(a.Data4[i] < b.Data4[i]) return -1;
    if(a.Data4[i] > b.Data4[i]) return 1;
  }
  return 0;
}

/* ------------------------------------------- */

String NamedGuid::toChar() const {
  StringPrintf ret("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X};%s",
    (int)g.Data1, (int)g.Data2, (int)g.Data3,
    (int)g.Data4[0], (int)g.Data4[1],
    (int)g.Data4[2], (int)g.Data4[3],
    (int)g.Data4[4], (int)g.Data4[5],
    (int)g.Data4[6], (int)g.Data4[7],
    getName() );

  return ret;
}  

void NamedGuid::fromChar(const char *source) {
  String s(source);
  String g = s.lSpliceChar(';');
  Guid::fromChar(g.vs());
  setName(s.vs());
}  

void NamedGuid::createGuid() {
  Guid::createGuid();
  setName("(null)");
}
  
  
void NamedGuid::setToInvalid() {
  Guid::setToInvalid();
  setName("(Invalid)");
}

void NamedGuid::setToGeneric() {
  Guid::setToGeneric();
  setName("@all@");
}


