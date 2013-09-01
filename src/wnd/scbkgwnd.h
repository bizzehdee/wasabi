#ifndef _WASABI_SCRLBKGWND_H
#define _WASABI_SCRLBKGWND_H

#include <wnd/canvas.h>
#include "labelwnd.h"
//CUT #include "scrollbar.h"
//CUT #include "../../common/sepwnd.h"
//?#include "../autobitmap.h"
#include <draw/bitmap.h>

class ScrollBar;
//fixme class SepWnd;

#define SCRLBKGWND_PARENT LabelWnd
class ScrlBkgWnd : public SCRLBKGWND_PARENT {
protected:
  
  ScrlBkgWnd();
public:
  
  virtual ~ScrlBkgWnd();
  
  
  virtual int onInit();
  
  virtual int onPaint(Canvas *c);
  
  virtual void drawBackground(Canvas *canvas);
  
  virtual int onEraseBkgnd(HDC dc);
  
  virtual int childNotify(RootWnd *child, int msg, int param1, int param2);
  
  virtual int onResize();
  
  virtual void getClientRect(RECT *r);
//  virtual void getNonClientRect(RECT *r);
  
  virtual int getHeaderHeight();
  virtual void timerCallback (int id);

  
  virtual void onHScrollToggle(int set);
  
  virtual void onVScrollToggle(int set);
  
  virtual void onSetVisible(int show);
  
  virtual int wantHScroll() { return 1; }
  
  virtual int wantVScroll() { return 1; }
  
  void makeWindowOverlayMask(Region *r);

  
  SkinBitmap *getBgBitmap(void);
  
  void setBgBitmap(const char *b);
  
  void setBgColor(COLORREF rgb);
  
  virtual COLORREF getBgColor(void);

  
  virtual int getContentsWidth();	// not safe to call getclientrect!
  
  virtual int getContentsHeight();	// not safe to call getclientrect!

  
  void setLineHeight(int h);
  
  int getLinesPerPage();

  
  int getScrollX();
  
  int getScrollY();
  
  int getScrollbarWidth();	//deprecated

  int getHScrollbarWidth();
  int getVScrollbarWidth();

  int getCurHScrollbarWidth();	// 0 if not showing
  int getCurVScrollbarWidth();	// 0 if not showing
  
  virtual void scrollToY(int y, int signal=TRUE);
  
  virtual void scrollToX(int x, int signal=TRUE);

protected:

  virtual void onScrollY(int y) { }
  
  void setSlidersPosition();
  
  int needDoubleBuffer();
  
  virtual BOOL needHScroll();
  
  virtual BOOL needVScroll();
  
  int getMaxScrollY();
  
  int getMaxScrollX();
  
  void updateScrollY(BOOL smooth=FALSE);
  
  void updateScrollX(BOOL smooth=FALSE);
  
  void smoothScrollToY(int y);
  
  void smoothScrollToX(int x);
  
  void updateVScroll(int y);
  
  void updateHScroll(int x);

//fixme  AutoSkinBitmap bmp;
  
  int dbbuffer;
  BOOL inDestroy;

  ScrollBar *hScroll;
  ScrollBar *vScroll;
//fixme   SepWnd *hSep;
//fixme   SepWnd *vSep;

  COLORREF bgColor;

  int scrollX;
  int scrollY;

  BOOL needSetSliders;
  BOOL wantsep;

  int lineHeight;

  float smoothScrollYInc, smoothScrollXInc;
  float smoothScrollYCur, smoothScrollXCur;
  int smoothScrollYTimerCount, smoothScrollXTimerCount;
  int smoothYTimer, smoothXTimer;
  
  void killSmoothYTimer();
  
  void killSmoothXTimer();
  double lastratio;
  RECT smsqr;
  
  void _setSlidersPosition();
  int in_set_slider_position;
};

#endif
