#ifndef _CLICKWND_H
#define _CLICKWND_H

// this class defines clicking behavior, i.e. detecting mouse downs and ups
// and doing captures to determine clicks

#include "backbufferwnd.h"

#define CLICKWND_PARENT BackBufferWnd
class NOVTABLE ClickWnd : public CLICKWND_PARENT {
protected:
  ClickWnd();
public:
  virtual ~ClickWnd();

  void setHandleRightClick(int tf);
  int getHandleRightClick();

  // override these to get clicks!
  virtual void onLeftPush(int x, int y) {}
  virtual void onMiddlePush(int x, int y) {}
  virtual void onRightPush(int x, int y) {}
  virtual void onLeftDoubleClick(int x, int y) {}  
  virtual void onRightDoubleClick(int x, int y) {} 

  virtual void onEnterArea();
  virtual void onLeaveArea();

  virtual void onSetVisible(int show);
  virtual void onCancelCapture();
  virtual int isInClick() { return mousedown; }

protected:
  virtual int onLeftButtonDown(int x, int y);
  virtual int onMiddleButtonDown(int x, int y);
  virtual int onRightButtonDown(int x, int y);
  virtual int onLeftButtonUp(int x, int y);
  virtual int onMiddleButtonUp(int x, int y);
  virtual int onRightButtonUp(int x, int y);
  virtual int onMouseMove(int x, int y);

  // override this and return 0 to ignore clicks
  virtual int wantClicks() { return 1; }
  // override this and return 1 to force down-ness
  virtual int userDown() { return 0; }

  int getHilite() { return hilite; }	// mouse is over, period
  int getDown() { return down; }	// mouse is over and pushing down

  int onButtonDown(int which, int x, int y);
  int onButtonUp(int which, int x, int y);
  void _enterCapture();

private:
  void _onEnterArea();
  void _onLeaveArea();

  int button;	// 0 == left, 1 == right, 2 == middle, which button was pushed
  int handleRight:1, handleMiddle:1;
  int mousedown:1;
  int mcaptured:1;	// we are capturing the mouse
  int hilite:1;	// mouse is over but not down
  int down:1;
  int areacheck;
};

#endif
