#ifndef __WILDCHARSENUM_H
#define __WILDCHARSENUM_H

#include <bfc/ptrlist.h>
#include <bfc/string/string.h>

using namespace wasabi;

class find_entry {
public:
  find_entry(const char *_path, const char *_filename) : path(_path), filename(_filename) {}
  ~find_entry() {}
  String path;
  String filename;
};

class COMEXP WildcharsEnumerator {
  
public:
  
  WildcharsEnumerator(const char *_selection);
  virtual ~WildcharsEnumerator();

  int getNumFiles();
  const char *enumFile(int n);
  void rescan();

  static int isWildchars(const char *filename);

private:

  int utf16;
  String selection;
  PtrList < find_entry > finddatalist;
  String singfiledup;
  String enumFileString;
};

#endif
