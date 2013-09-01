#ifndef _LOADLIB_H
#define _LOADLIB_H

#include <bfc/std.h>
#include <bfc/named.h>
#if UTF8
#ifdef WANT_UTF8_WARNINGS
#pragma CHAT("mig", "all", "UTF8 is enabled in loadlib.cpp")
#endif
#include <bfc/string/encodedstr.h>
#endif

class COMEXP Library : public Named {
public:
  Library(const char *filename=NULL);
  Library(const Library &l) {
    lib = NULL; // FG> overrides default constructor, so we need to init this too...
    load(l.getName());
  }
  ~Library();

  Library& operator =(const Library &l) {
    if (this != &l) {
      unload();
      load(l.getName());
    }
    return *this;
  }

  int load(const char *filename=NULL);
  void unload();

  void *getProcAddress(const char *procname);
  OSMODULEHANDLE getHandle() const { return lib; }

private:
  OSMODULEHANDLE lib;
};

#endif
