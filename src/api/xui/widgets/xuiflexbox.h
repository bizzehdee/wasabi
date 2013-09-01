#ifndef XUIFLEXBOX_H
#define XUIFLEXBOX_H

#include <wnd/virtualwnd.h>
#include <api/service/svcs/svc_xuiobject.h>
#include <api/xui/xuiwnd.h>
#include "xuiobject.h"

#define XUIFlexBox_PARENT XUIObject

class XUIFlexBox : public XUIFlexBox_PARENT {
public:
  XUIFlexBox();
  virtual ~XUIFlexBox();

  ACCESS_SOURCE_WND(m_box, VirtualWnd, XUIFlexBox_PARENT);

};

#endif // XUIFLEXBOX_H
