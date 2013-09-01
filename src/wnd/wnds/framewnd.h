#ifndef _WASABI_FRAMEWND_H
#define _WASABI_FRAMEWND_H

#include <wnd/labelwnd.h>
#include <draw/bitmap.h>

#define MAXCHILD 2	// this had better never not be 2

typedef enum {
  DIVIDER_HORIZONTAL, DIVIDER_VERTICAL, DIVIDER_UNDEFINED = -1
} FrameWndDividerType;
enum { SDP_FROMLEFT, SDP_FROMRIGHT };
#define SDP_FROMTOP SDP_FROMLEFT
#define SDP_FROMBOTTOM SDP_FROMRIGHT

typedef enum {
  FRAMEWND_SQUISH,
  FRAMEWND_COVER
} FrameWndSlideMode;

#define SIZERWIDTH 8

// this window holds other basewnd derived classes
#define FRAMEWND_PARENT LabelWnd
class FrameWnd : public FRAMEWND_PARENT {
public:
  FrameWnd();
  virtual ~FrameWnd();

  virtual int onInit();

  virtual int getCursorType(int x, int y);

  virtual int onPaint(Canvas *canvas);
  virtual int onResize();

  virtual int onLeftButtonDown(int x, int y);
  virtual int onMouseMove(int x, int y);	// only called when mouse captured
  virtual int onLeftButtonUp(int x, int y);

  virtual int childNotify(RootWnd *which, int msg, int param1, int param2);

//  virtual int forceFocus();

  // unique to this class
  int setChildren(BaseWnd *child1, BaseWnd *child2=NULL);
  int setChildrenRootWnd(RootWnd *child1, RootWnd *child2=NULL);
  RootWnd *enumChild(int which);
  // horizontal or vertical?
  void setDividerType(FrameWndDividerType type);
  FrameWndDividerType getDividerType();
  // where is the divider?
  int setDividerPos(int from, int pos);
  // this version doesn't check the cfg file for last position
  int setDividerPosNoCfg(int from, int pos);
  void getDividerPos(int *from, int *pos);

  int setResizeable(int is);
  void setMinWidth(int min);

  virtual int onGetFocus();
  virtual int onDeferredCallback(int p1, int p2);

  // cover or squish
  void setSlideMode(FrameWndSlideMode mode);
  FrameWndSlideMode getSlideMode();

  virtual void onResizeChildren(RECT leftr, RECT rightr);

  // drag and drops are forwarded into windowshaded windows
  virtual int dragEnter(RootWnd *sourceWnd);
  virtual int dragOver(int x, int y, RootWnd *sourceWnd);
  virtual int dragLeave(RootWnd *sourceWnd);
  virtual int dragDrop(RootWnd *sourceWnd, int x, int y);

protected:
  int convertPropToPix(int prop);
  int ConvertPixToProp();

  void windowshade(int which, int shaded);
  RootWnd *getWindowShadedChild();

private:
  int _setChildren(RootWnd *child1, RootWnd *child2, BaseWnd *child1b, BaseWnd *child2b);
  int nchild;
  BaseWnd *children[MAXCHILD];
  RootWnd *rwchildren[MAXCHILD];
  int hidey[MAXCHILD];
  int windowshaded[MAXCHILD];

  FrameWndDividerType vert;

  int resizeable;
  FrameWndSlideMode slidemode;

  int divideside;
  int pullbarpos;	// 0..65536
  int prevpullbarpos;
  int minwidth, maxwidth;

  int fake_focus;

  int down;

  RECT sizerRect;

//fixme
#define AutoSkinBitmap Bitmap
  AutoSkinBitmap h_bitmap, v_bitmap, h_grabber, v_grabber, ws_bitmap;
  int resizing;
};

#define PULLBAR_FULL 65536L
#define PULLBAR_HALF (PULLBAR_FULL/2)
#define PULLBAR_QUARTER (PULLBAR_FULL/4)
#define PULLBAR_THREEQUARTER (PULLBAR_FULL-PULLBAR_QUARTER)
#define PULLBAR_EIGHTH (PULLBAR_FULL/8)

#endif
