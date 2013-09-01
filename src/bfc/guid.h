#ifndef __WASABI_GUID_H_
#define __WASABI_GUID_H_

#include <bfc/platform/guid.h>
#include <bfc/std_math.h>
#include <bfc/compare.h>
#include <bfc/string/string.h>
#include <bfc/named.h>

#ifdef WASABI_PLATFORM_WIN32
#include <objbase.h>
#endif

int compareGUID(GUID *a, GUID *b);

class Guid {
public:
  Guid(const Guid &_g) : g(_g.g) {}
  Guid(const GUID &_g) : g(_g) {}
  Guid(const char *str) { fromChar(str); }
  Guid(int create=TRUE) : g(INVALID_GUID) { if (create) createGuid(); }
  
  virtual ~Guid() {}
  
  operator GUID() const { return g; }
  GUID operator =(GUID _g) { g = _g; return g; }

  GUID getGuid() const { return g; }
  void setGuid(GUID _g) { g = _g; }
  
  // To/from the "Human Readable" character format.
  // "{1B3CA60C-DA98-4826-B4A9-D79748A5FD73}"
  virtual String toChar() const;
  virtual void fromChar(const char *source);
  // To/from the "C Structure" character format.
  // "{ 0x1b3ca60c, 0xda98, 0x4826, { 0xb4, 0xa9, 0xd7, 0x97, 0x48, 0xa5, 0xfd, 0x73 } };"
  String toCode() const;
  void fromCode(const char *source);
  
  // strlen("{xx xxx xxx-xxxx-xxxx-xxxx-xxx xxx xxx xxx}"
  enum { GUID_STRLEN = 38 };
  
  virtual void createGuid();
  virtual void setToInvalid();
  virtual void setToGeneric();
  
  static int compare(const Guid &_a, const Guid &_b);
  static int compare(const GUID &_a, const GUID &_b);
  int compareTo(const Guid &_g);

protected:
  GUID g;
};

class GUIDSort {
public:
  static int compareItem(void *p1, void *p2) {
    return Guid::compare(*(GUID *)p1, *(GUID *)p2);
  }
  static int compareAttrib(const char *attrib, void *item) {
    return Guid::compare(*(GUID *)attrib, *(GUID *)item);
  }
};

class GuidSort {
public:
  static int compareItem(void *p1, void *p2) {
    return Guid::compare(*(Guid *)p1, *(Guid *)p2);
  }
  static int compareAttrib(const char *attrib, void *item) {
    return Guid::compare(*(Guid *)attrib, *(Guid *)item);
  }
};

class NamedGuid : public Guid, public Named {
public:
  NamedGuid(const GUID &g, const char *name = "(null)") : Guid(g), Named(name) {}
  NamedGuid(const Guid &g, const char *name = "(null)") : Guid(g), Named(name) {}
  NamedGuid(const char *named_guid) { fromChar(named_guid); }
  NamedGuid() { createGuid(); }
  
  virtual ~NamedGuid() {}
  
  // To/from the Named Guid character format.
  // "{1B3CA60C-DA98-4826-B4A9-D79748A5FD73};Name"
  virtual String toChar() const;
  virtual void fromChar(const char *source);
  
  virtual void createGuid();
  virtual void setToInvalid();
  virtual void setToGeneric();
};

DECL_COMPARE_OPERATORS(Guid, Guid::compare);

#endif//__WASABI_GUID_H_
