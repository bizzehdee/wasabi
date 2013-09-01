#include <precomp.h>
#include "paramparser.h"
#include <bfc/nsguid.h>

int ParamParser::findGuid(GUID g) {
  for (int i=0;i<getNumItems();i++) {
    const char *e = enumItem(i);
    GUID eguid = nsGUID::fromChar(e);
    if (g == eguid) return i;
  }
  return -1;
}

int ParamParser::findString(const char *str) {
  for (int i=0;i<getNumItems();i++) {
    if (STRCASEEQL(str, enumItem(i))) return i;
  }
  return -1;
}