#ifndef XUIEDIT_H
#define XUIEDIT_H

#include <api/service/svcs/svc_xuiobject.h>
#include "xuiobject.h"
#include <api/xui/xuiwnd.h>
#include <wnd/wnds/editwnd.h>
#include "sourcesink/XUIEditEventSource.h"
#include "js/JSXUIEdit.h"

#define XUIEdit_PARENT XUIObject

class XUIEdit : public XUIEdit_PARENT, 
                 public XUIEditEventSource,
                 public JSXUIEdit {
public:
  XUIEdit();
  virtual ~XUIEdit();
  virtual void onAttributeChanged(const char *attributeName, const char *attributeValue);
  SCRIPT void selectAll();
  virtual void onSetWindow();

  SCRIPT EVENT void onEdit(const char *value=NULL);
  SCRIPT EVENT void onIdleEdit(const char *value=NULL);
  virtual int wantFocus() { return 1; }

private:
  int m_myvalueupdate;
  EditWnd *m_edit;
  MemBlock<char> m_buffer;
};

class XUIEditSvc: public XUIWnd<EditWnd, XUIEdit> {
public:
  virtual void onEditUpdate() {
    static_cast<XUIEdit *>(getXUIObject())->onEdit();
  }
  virtual void onIdleEditUpdate() {
    static_cast<XUIEdit *>(getXUIObject())->onIdleEdit();
  }
  
  static const char *getXmlTag() { return "edit"; }
};

#endif
