#ifndef _WASABI_ATTRCOLOR_H
#define _WASABI_ATTRCOLOR_H

#include "attribute.h"

class _color : public Attribute {
public:
  _color(const char *name=NULL, const char *default_val="#ff00ff") : Attribute(name) {
    setData(default_val, TRUE);
  }

  virtual int getAttributeType() { return AttributeType::COLOR; }

  ARGB32 setValueFromString(const char *newval);
  ARGB32 operator =(const char *newval) { return setValueFromString(newval); }
//  ARGB32 operator =(const char newval[]) { return setValueFromString(newval); }
  operator ARGB32() { return getValueAsColor(); }

  ARGB32 getValueAsColor();
};

//british friendly ;-p
#define _colour _color

#endif
