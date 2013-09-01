#ifndef _WASABI_LABELWND_H
#define _WASABI_LABELWND_H

//#include "guiobjwnd.h"
//CUT #include <wnd/basewnd.h>
#include "clickwnd.h"

//#define LABELWND_PARENT GuiObjectWnd
#define LABELWND_PARENT ClickWnd
class LabelWnd : public LABELWND_PARENT {
protected:
  LabelWnd();
public:

  virtual void getClientRect(RECT *);
  virtual int onResize();
  virtual int onPaint(Canvas *canvas);
  virtual int onGetFocus();
  virtual int onKillFocus();
  virtual void invalidateLabel();
  virtual int wantFocus();
  virtual int wantRenderBaseTexture() { return 1; }

  // override & return 1 to force painting label with focus all the time
  virtual int forceFocus() { return 0; }

  virtual void onSetName();
  virtual void setMargin(int newmargin);

  virtual int setFontSize(int size);

//CUT  virtual int childNotify(RootWnd *child, int msg, int param1=0, int param2=0);

  int showLabel(int show);
  int getLabelHeight();

  void reloadResources();

private:
  int show_label, labelsize;
  int labelHeight;
  int margin;
};

// use this if you want a generic labelwnd (but try not to)
class LabelWndI : public LabelWnd { };

#endif
