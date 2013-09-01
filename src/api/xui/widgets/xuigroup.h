#ifndef XUIGROUP_H
#define XUIGROUP_H

#include <api/service/svcs/svc_xuiobject.h>
#include "xuiobject.h"
#include <api/xui/xuiwnd.h>
#include <wnd/virtualwnd.h>
#include "sourcesink/XUIGroupEventSource.h"
#include "js/JSXUIGroup.h"

#define XUIGroup_PARENT XUIObject

class XUIGroup : public XUIGroup_PARENT, 
                 public XUIGroupEventSource,
                 public JSXUIGroup {
public:
  XUIGroup();
  virtual ~XUIGroup();
  virtual void onAttributeChanged(const char *attributeName, const char *attributeValue);
  virtual void onDestroy();
  virtual void onInit();
  SCRIPT XUIObject *getContent() { return m_content; }

private:
  void setContent(const char *id);
  void destroyContent();

  XUIObject *m_content;
  int m_inOnInit;
};

MAKE_XUISERVICE_CLASS(XUIGroupSvc, VirtualWnd, XUIGroup, "group");


#endif // XUIVBOX_H
