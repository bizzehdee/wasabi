#ifndef XUIELEMENT_H
#define XUIELEMENT_H

class XUIElement {
public:
  enum {
    XUIELEMENT_XUIOBJECT=0,
    XUIELEMENT_XUISCRIPT=1,
    XUIELEMENT_TEXT     =2,
  };

  virtual int getElementType()=0;
  virtual void setAttribute(const char *attrname, const char *attrvalue)=0;
};

#endif
