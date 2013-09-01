#include "precomp.h"

#include <bfc/std_wnd.h>

#ifdef WASABI_COMPILE_WND
//#include <bfc/api/api_wnd.h>
#endif

#include "clickwnd.h"

#include <wnd/notifmsg.h>

ClickWnd::ClickWnd() {
  handleRight = TRUE;
  handleMiddle = TRUE;
  button = -1;
  mousedown = 0;
  mcaptured = 0;
  hilite = 0;
  down = 0;
  areacheck = 0;
}

ClickWnd::~ClickWnd() {
  BaseWnd::hintDestroying(); // so basewnd doesn't call onCancelCapture
  if (getCapture()) endCapture();
}

void ClickWnd::setHandleRightClick(int tf) {
  handleRight=tf;
}

int ClickWnd::getHandleRightClick() {
  return handleRight;
}

int ClickWnd::onLeftButtonDown(int x, int y) {
  notifyParent(ChildNotify::CLICKWND_LEFTDOWN, x, y);
  CLICKWND_PARENT::onLeftButtonDown(x, y);
  abortTip();
  RootWnd *dp = getDesktopParent();
  if (dp != NULL) {
    if (dp->wantActivation()) {
      StdWnd::setActiveWindow(getRootParent()->gethWnd()); 
      StdWnd::setFocus(getRootParent()->gethWnd());
    } else {
      OSWINDOWHANDLE w = dp->gethWnd();
      OSWINDOWHANDLE owner = StdWnd::getParent(w);
      if (owner != INVALIDOSWINDOWHANDLE) {
        StdWnd::setActiveWindow(owner);
        StdWnd::setFocus(owner);
      }
    }
  } else {
    StdWnd::setActiveWindow(getRootParent()->gethWnd()); 
  }
  if (ptInRegion(x, y))
    return onButtonDown(WM_LBUTTONDOWN, x, y);
  else
    return 1;
}

int ClickWnd::onMiddleButtonDown(int x, int y) {
  notifyParent(ChildNotify::CLICKWND_MIDDLEDOWN, x, y);
  CLICKWND_PARENT::onMiddleButtonDown(x, y);
  abortTip();
  if (!handleMiddle) return 1;
  if (ptInRegion(x, y))
    return onButtonDown(WM_MBUTTONDOWN, x, y);
  else
    return 1;
}

int ClickWnd::onRightButtonDown(int x, int y) {
  notifyParent(ChildNotify::CLICKWND_RIGHTDOWN, x, y);
  CLICKWND_PARENT::onRightButtonDown(x, y);
  abortTip();
  if (!handleRight) return 1;
  if (ptInRegion(x, y))
    return onButtonDown(WM_RBUTTONDOWN, x, y);
  else
    return 1;
}

int ClickWnd::onLeftButtonUp(int x, int y) {
  notifyParent(ChildNotify::CLICKWND_LEFTUP, x, y);
  CLICKWND_PARENT::onLeftButtonUp(x, y);
//jf
//  if (ptInRegion())
    return onButtonUp(WM_LBUTTONUP, x, y);
//  else
//    return 1;
}

int ClickWnd::onMiddleButtonUp(int x, int y) {
  notifyParent(ChildNotify::CLICKWND_MIDDLEUP, x, y);
  CLICKWND_PARENT::onMiddleButtonUp(x, y);
  //jf
  //if (ptInRegion()) 
  if (!handleMiddle) {
    onMiddlePush(x, y);
    return 1;
  }
    return onButtonUp(WM_MBUTTONUP, x, y);
//  else
//    return 1;
}

int ClickWnd::onRightButtonUp(int x, int y) {
  notifyParent(ChildNotify::CLICKWND_RIGHTUP, x, y);
  CLICKWND_PARENT::onRightButtonUp(x, y);
  //jf
  //if (ptInRegion()) 
  if (!handleRight) {
    onRightPush(x, y);
    return 1;
  }
    return onButtonUp(WM_RBUTTONUP, x, y);
//  else
//    return 1;
}

int ClickWnd::onMouseMove(int x, int y) {
  POINT pos, rpos={x,y};

  CLICKWND_PARENT::onMouseMove(x, y);

  pos=rpos;
  clientToScreen(&pos);

#ifdef WASABI_COMPILE_WND
  int mouseover = (WASABI_API_WND->rootWndFromPoint(&pos) == static_cast<RootWnd *>(this) && ptInRegion(x, y));
#else
  int mouseover = (BaseWnd::rootWndFromScreenPoint(&pos) == static_cast<RootWnd *>(this) && ptInRegion(x, y));
#endif

  if (!mouseover && (!mousedown || !Std::keyDown(button?MK_RBUTTON:MK_LBUTTON))) {
    if (mcaptured || getCapture()) {
      _onLeaveArea();
      endCapture();
      mcaptured = 0;
      hilite = 0;
      down = 0;
      mousedown = 0;
      invalidate();
    }
    return 1;
  }

  if (!getCapture() && mouseover) {	// capture to see when leave
    _enterCapture();
  }

  int lastdown = down;
  int lasthilite = hilite;
  hilite = mouseover;
#ifdef WASABI_COMPILE_WNDMGR
  int m = getGuiObject() ? getGuiObject()->guiobject_getMover() : 0;
#else
  int m = 0;
#endif
  if (!m) {
    down = userDown() || (mouseover && mousedown);
  } else 
    down = userDown() || mousedown;
  
  // FG> note to self now that i finally fixed this... :
  // there is a potential bottleneck here, if for some reason this test is always true when moving the windows around like crazy.
  if (down != lastdown || (hilite != lasthilite && !m)) {
    invalidate();
  }

  return 1;
}

void ClickWnd::_enterCapture() {
  if (!mcaptured) _onEnterArea();
  if (!StdWnd::getCapture()) beginCapture();  //FUCKO: unsure about this for XWindow, could result in stealing the Grab.
  mcaptured = 1;
}

int ClickWnd::onButtonDown(int which, int x, int y) {
  
  if (!wantClicks()) return 1;

  if (!getCapture()) {
    _enterCapture();
  }
  mousedown = 1;
  down = 1;
  button = -1;
  if (which == WM_LBUTTONDOWN) button = 0;
  else if (which == WM_RBUTTONDOWN) button = 1;
  invalidate();

  return 1;
}

int ClickWnd::onButtonUp(int which, int x, int y) {

    // make sure same button
  if (button == 0 && which == WM_RBUTTONUP) return 1;
  if (button == 1 && which == WM_LBUTTONUP) return 1;

  if (!down) {
    if (mcaptured) _onLeaveArea();
    endCapture();
    mcaptured = 0;
    hilite = 0;
    mousedown = 0;
    return 1;
  }

    POINT pos={x,y};
    clientToScreen(&pos);

#ifdef WASABI_COMPILE_WND
    int mouseover = (WASABI_API_WND->rootWndFromPoint(&pos) == static_cast<RootWnd *>(this) && ptInRegion(x, y));
#else
  int mouseover = (BaseWnd::rootWndFromScreenPoint(&pos) == static_cast<RootWnd *>(this) && ptInRegion(x, y));
#endif
    if (!mouseover && mcaptured) {
      _onLeaveArea();
      endCapture();
      mcaptured = 0;
      hilite = 0;
    }

    // it was down, process the event
    int a = down;
    down = 0;
    mousedown = 0;
    invalidate();

    if (a) {
      if (button == 0) onLeftPush(x, y);
      else if (button == 1) onRightPush(x, y);
      else if (button == 2) onMiddlePush(x, y);
    } else {		// was off button
      hilite = 0;
    }

    return 1;
}

void ClickWnd::onSetVisible(int show) {
  CLICKWND_PARENT::onSetVisible(show);
  if (!show && getCapture()) {
    mcaptured=0;
    hilite = 0;
    down = 0;
    mousedown = 0;
    endCapture();
    _onLeaveArea();
  }
}

void ClickWnd::_onEnterArea() {
  if (areacheck == 0) {
    onEnterArea();
    areacheck++;
  } else 
    DebugString("onEnterArea check failed\n");
}

void ClickWnd::_onLeaveArea() {
  if (areacheck == 1) {
    onLeaveArea();
    areacheck--;
  } else 
    DebugString("onLeaveArea check failed\n");
}

void ClickWnd::onEnterArea() {
}

void ClickWnd::onLeaveArea() {
}

void ClickWnd::onCancelCapture() {
  CLICKWND_PARENT::onCancelCapture();
  if (areacheck) _onLeaveArea();
  mcaptured=0;
  hilite = 0;
  down = 0;
  mousedown = 0;
}

