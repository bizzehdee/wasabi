#include "precomp.h"

#include "stringtable.h"

class StringRef : public String {
protected:
  friend class StringTable;
  friend class Freelist<StringRef>;

  StringRef(const char *str);
  ~StringRef();

  int refcount;
};

StringRef::StringRef(const char *str)
  : String(str) {
  refcount = 1;
}

class PLS_StringRef  {
public:
  // comparator for sorting
  static int compareItem(StringRef *p1, StringRef *p2) {
    return STRCMP(p1->getValue(), p2->getValue());
  }
  // comparator for searching
  static int compareAttrib(const char *attrib, StringRef *item) {
    return STRCMP(attrib, item->getValue());
  }
};

StringRef::~StringRef() {
  //ASSERT(refcount == 0);
  if (refcount != 0) DebugString("StringRef: refcount != 0: '%s'\n", vs());
}

StringTable::~StringTable() {
//  ASSERTPR(refs.getNumItems() == 0, "didn't free all strings!");
  refs_freelist.deletePtrList(&refs);
}

StringTable::StringTable(int autocull) : want_autocull(autocull) {}

const char *StringTable::allocStr(const char *val) {
  if (val == NULL) return NULL;
  StringRef *ref=refs.findItem(val);
  if(ref) {
    ref->refcount++;
    return ref->getValue();
  }

  StringRef *s = refs_freelist.newRecord(val);
  refs.addItem(s);
  return s->getValue();
}

void StringTable::deallocStr(const char *val) {
  if (val == NULL) return;

  int i=0;
  StringRef *ref=refs.findItem(val,&i);
  if(ref) {
    if (--ref->refcount <= 0) {
      if (want_autocull) {
        refs.delByPos(i);
        refs_freelist.deleteRecord(ref);
      }
    }
    return;
  }
}

int StringTable::getNumStrings() {
  return refs.n();
}

const char *StringTable::enumString(int pos) {
  StringRef *ref = refs[pos];
  if (ref != NULL) return ref->getValueSafe();
  return NULL;
}


void StringTable::cull() {
  for (int i = 0; i < refs.getNumItems(); i++) {
    StringRef *ref = refs[i];
    if (ref->refcount == 0) {
      refs.delByPos(i);
      i--;
      refs_freelist.deleteRecord(ref);
    }
  }
}

int StringTable::gccb_onGarbageCollect() {
  cull();
  return 1;
}
