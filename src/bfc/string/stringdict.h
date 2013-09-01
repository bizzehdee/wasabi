#ifndef __STRINGDICT_H
#define __STRINGDICT_H

#include <bfc/string/string.h>
#include <bfc/ptrlist.h>

// a convenient class to statically declare strings and their IDs with binary search lookups
//
// example of use :
//
// BEGIN_STRINGDICTIONARY(_identifier)
// SDI("somestring",              0);
// SDI("someotherstring",         1);
// SDI("andyetanotherstring",     2);
// END_STRINGDICTIONARY(_identifier, identifier)

// foo (const char *str) {
//  int a = identifier.getId(str);
//  if (a < 0) { 
//    // not found!
//  }
// return a;
// }

class StringDictionaryItem {
  public:
    StringDictionaryItem(const char *string, int stringid) : str(string), id(stringid) {}
    virtual ~StringDictionaryItem() {}
    const char *getString() { return str; }
    int getId() { return id; }
  private:
    String str;
    int id;
};

class StringDictionaryItemCompare {
public:
  static int compareItem(void *p1, void *p2) {
    return STRICMP(((StringDictionaryItem *)p1)->getString(), ((StringDictionaryItem *)p2)->getString());
  }
  static int compareAttrib(const char *attrib, void *item) {
    return STRICMP(attrib, ((StringDictionaryItem *)item)->getString());
  }
};

class StringDictionary {
  public:
    StringDictionary() {}
    virtual ~StringDictionary() { items.deleteAll(); }
    virtual void addItem(const char *str, int id) { ASSERT(id != -1); items.addItem(new StringDictionaryItem(str, id)); }
    virtual int getId(const char *str, int *notfound=NULL) { StringDictionaryItem *i = items.findItem(str); if (i == NULL) { if (notfound) { *notfound = 1; } return -1; } return i->getId(); }
  private:
    PtrListQuickSorted<StringDictionaryItem, StringDictionaryItemCompare> items;
};

#define BEGIN_STRINGDICTIONARY(class_ident) \
class class_ident : public StringDictionary { \
  public: \
    class_ident() { 

#define SDI(str, id) \
      addItem(str, id);

#define END_STRINGDICTIONARY(class_ident, instance_ident) \
    }; \
    virtual ~class_ident() { } \
}; \
\
class_ident instance_ident;


#endif
