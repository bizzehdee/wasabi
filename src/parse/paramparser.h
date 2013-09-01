#ifndef _WASABI_PARAMPARSE_H
#define _WASABI_PARAMPARSE_H

#include <parse/pathparse.h>

class ParamParser : private PathParser {
public:
  ParamParser(const char *str, const char *sep=";", int unique=0) : PathParser(str, sep, unique) {}
  using PathParser::setString;

  int findGuid(GUID g);
  int findString(const char *str);

  int hasGuid(GUID g) { return findGuid(g) >= 0; }
  int hasString(const char *str) { return findString(str) >= 0; }

  const char *enumItem(int element) { return enumString(element); }
  int getNumItems() { return getNumStrings(); }
  const char *getLastItem() { return getLastString(); }
};

#endif
