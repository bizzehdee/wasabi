#ifndef _WASABI_STRINGTABLE_H
#define _WASABI_STRINGTABLE_H

// maintains a refcounted cache of strings

#include <bfc/ptrlist.h>
#include <bfc/string/string.h>
#include <bfc/freelist.h>

#include <api/syscb/callbacks/gccb.h>	// for auto-garbage collection

#include "stringtable.h"

class StringRef;
class PLS_StringRef;

// the garbage collection is totally optional
class StringTable /*: public GarbageCollectCallbackI */{
public:
  StringTable(int autocull=TRUE);
  ~StringTable();

  const char *allocStr(const char *val);
  void deallocStr(const char *val);

  int getNumStrings();
  const char *enumString(int pos);

  // delete strings with refcount == 0 (needed if autocull==FALSE)
  void cull();

protected:
  // if you want, you can register this as a syscallback and it will cull
  // on system garbage collection events (instead of on deletion)
  virtual int gccb_onGarbageCollect();

private:
  int want_autocull;
  PtrListInsertSorted<StringRef,PLS_StringRef> refs;
  Freelist<StringRef> refs_freelist;
};

#endif
