#ifndef __OSWND_H
#define __OSWND_H

//CUT #include "../common/guiobjwnd.h"
#include <wnd/labelwnd.h>

//CUT #define OSWND_PARENT GuiObjectWnd
#define OSWND_PARENT LabelWnd

class OSWnd : public OSWND_PARENT {
public:
  virtual int onInit();
  virtual void onSetVisible(int show);
  virtual int onResize();
  virtual int handleRatio() { return 0; }

  virtual OSWINDOWHANDLE getOSHandle()=0;
};

#endif
