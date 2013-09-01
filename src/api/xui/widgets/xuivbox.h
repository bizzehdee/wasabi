#ifndef XUIVBOX_H
#define XUIVBOX_H

#include "xuiflexbox.h"

#define XUIVBox_PARENT XUIObject

class XUIVBox : public XUIVBox_PARENT {
public:
  XUIVBox() {
    m_flex_orientation = FLEXBOX_ORIENTATION_VERTICAL;
  }
};


MAKE_XUISERVICE_CLASS(XUIVBoxSvc, VirtualWnd, XUIVBox, "vbox");


#endif // XUIVBOX_H
