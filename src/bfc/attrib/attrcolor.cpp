#include "precomp.h"

#include "attrcolor.h"

#include <draw/argb32.h>

ARGB32 _color::setValueFromString(const char *newval) {
  setData(newval); return *this;
}

ARGB32 _color::getValueAsColor() {
  char buf[1024]="";
  getData(buf, sizeof buf);
  unsigned int a=0, r=0, g=0, b=0;
  if (*buf == '#') {
    SSCANF(buf+1, "%02x%02x%02x", &r, &g, &b);
  }
  return MKARGB32(a, r, g, b);
}
