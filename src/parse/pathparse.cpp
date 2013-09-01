#include <precomp.h>

#include "pathparse.h"

PathParser::PathParser(const char *_str, const char *sep, int uniquestrs) :
  processed(FALSE), str(_str ? _str : ""), separators(sep), uniques(uniquestrs)
{
  ASSERT(sep != NULL);
}

void PathParser::setString(const char *string, const char *sep) {
  ASSERT(sep != NULL);
  if (string == NULL) string = "";
  str = string;
  separators = sep;
  strings.removeAll();
  processed = 0;
}

int PathParser::getNumStrings() {
  process();
  return strings.getNumItems();
}

char *PathParser::enumString(int i) {
  process();
  return strings[i];
}

char *PathParser::enumStringSafe(int i, char *def_val) {
  char *ret = enumString(i);
  if (ret == NULL) ret = def_val;
  return ret;
}

PtrList<String> PathParser::makeStringList(int trimmed) {
  PtrList<String> ret;
  for (int i = 0; i < getNumStrings(); i++) {
    String *s = new String(enumString(i));
    if (trimmed) s->trim();
    ret.addItem(s);
  }
  return ret;
}

void PathParser::process() {
  if (processed) return;
  processed = 1;
  preProcess(str);
  char *nonconst = str.getNonConstVal();
  
  char *pt = strtok(nonconst, separators);
  if (pt == NULL) return;
  postProcess(pt);
  strings.addItem(pt);
  for (;;) {
    char *pt = strtok(NULL, separators);
    if (pt == NULL) break;
    postProcess(pt);
    if (uniques) {
      int exists = 0;
      foreach(strings)
        if (STRCASEEQL(strings.getfor(), pt)) {
          exists=1;
          break;
        }
      endfor;
      if (exists) continue;
    }
    strings.addItem(pt);
  }
}

