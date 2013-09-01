#ifndef XUISCRIPT_H
#define XUISCRIPT_H

#include "xuielement.h"
#include <api/js/jssandbox.h>

class XUIDocument;
class XUIObject;

class JSObjectDef {
public:
  JSObjectDef(XUIObject *object, const char *name) : m_object(object),
                                                     m_name(name) {}
  XUIObject *getObject() { return m_object; }
  const char *getName() { return m_name; }

private:
  XUIObject *m_object;
  String m_name;
};

class XUIScript : public XUIElement {
public:
  XUIScript();
  virtual ~XUIScript();

  virtual int getElementType() { return XUIELEMENT_XUISCRIPT; }
  virtual void setAttribute(const char *attrname, const char *attrvalue)=0;
  virtual void appendCode(const char *text, const char *file=NULL, int line=0)=0;
  virtual void onReady(XUIObject *object, const char *name)=0;
  virtual void onReady(PtrList<JSObjectDef> &defs)=0;
};

#endif
