#ifndef XUIJSSCRIPT_H
#define XUIJSSCRIPT_H

#include <api/js/jssandbox.h>
#include "xuiscript.h"

class XUIJSScript : public XUIScript,
                    public JSSandbox {
public:
  XUIJSScript() {}
  virtual ~XUIJSScript() {}

  virtual void setAttribute(const char *attrname, const char *attrvalue);
  virtual void appendCode(const char *text, const char *file=NULL, int line=0);
  virtual void onReady(XUIObject *object, const char *name);
  virtual void onReady(PtrList<JSObjectDef> &defs);

private:
  String m_code;
  String m_file;
  int m_line;
};

#endif // XUIJSSCRIPT_H