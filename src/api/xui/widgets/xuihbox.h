#ifndef XUIHBOX_H
#define XUIHBOX_H

#include "xuiflexbox.h"

#define XUIHBox_PARENT XUIObject

class XUIHBox : public XUIHBox_PARENT {
public:
  XUIHBox() {
    m_flex_orientation = FLEXBOX_ORIENTATION_HORIZONTAL;
  }
};

MAKE_XUISERVICE_CLASS(XUIHBoxSvc, VirtualWnd, XUIHBox, "hbox");


#endif // XUIHBOX_H
