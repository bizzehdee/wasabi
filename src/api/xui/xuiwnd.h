#ifndef XUIWND_H
#define XUIWND_H

#define FORWARDEVENT0(event) virtual int event() { WNDCLASS::event(); m_xuiobject->event(); return 0; }
#define FORWARDEVENT1(event, a) virtual int event(a _a) { WNDCLASS::event(_a); m_xuiobject->event(_a); return 0; }
#define FORWARDEVENT2(event, a, b) virtual int event(a _a, b _b) { WNDCLASS::event(_a, _b); m_xuiobject->event(_a, _b); return 0; }
#define FORWARDEVENT3(event, a, b, c) virtual int event(a _a, b _b, c _c) { WNDCLASS::event(_a, _b, _c); m_xuiobject->event(_a, _b, _c); return 0; }
#define FORWARDEVENT0X(event, newevent) virtual int event() { WNDCLASS::event(); m_xuiobject->newevent(); return 0; }
#define FORWARDEVENT1X(event, newevent, a) virtual int event(a _a) { WNDCLASS::event(_a); m_xuiobject->newevent(); return 0; }
#define FORWARDEVENT2X(event, newevent, a, b) virtual int event(a _a, b _b) { WNDCLASS::event(_a, _b); m_xuiobject->newevent(_a, _b); return 0; }
#define _FORWARDEVENT0(event) virtual void event() { WNDCLASS::event(); m_xuiobject->event(); }
#define _FORWARDEVENT1(event, a) virtual void event(a _a) { WNDCLASS::event(_a); m_xuiobject->event(_a); }
#define _FORWARDEVENT2(event, a, b) virtual void event(a _a, b _b) { WNDCLASS::event(_a, _b); m_xuiobject->event(_a, _b); }
#define _FORWARDEVENT0X(event, newevent) virtual void event() { WNDCLASS::event(); m_xuiobject->newevent(); }
#define _FORWARDEVENT1X(event, newevent, a) virtual void event(a _a) { WNDCLASS::event(_a); m_xuiobject->newevent(); }
#define _FORWARDEVENT2X(event, newevent, a, b) virtual void event(a _a, b _b) { WNDCLASS::event(_a, _b); m_xuiobject->newevent(_a, _b); }

template <class WNDCLASS, class XUICLASS>
class XUIWnd : public WNDCLASS {
public:
  
  XUIWnd() { 
    m_xuiobject = new XUICLASS();
    m_xuiobject->setBaseWnd(this);
  }
  
  virtual ~XUIWnd() {
    m_xuiobject->onDestroy();
    safedelete(m_xuiobject, XUICLASS*);
  }

  WNDCLASS *getWindow() { return m_wnd; }
  XUIObject *getXUIObject() { return m_xuiobject; }

  FORWARDEVENT0(onInit);
  FORWARDEVENT0(onResize);
  FORWARDEVENT2(onLeftButtonDown, int, int);
  FORWARDEVENT2(onLeftButtonUp, int, int);
  FORWARDEVENT2(onRightButtonDown, int, int);
  FORWARDEVENT2(onRightButtonUp, int, int);
  FORWARDEVENT2(onMiddleButtonDown, int, int);
  FORWARDEVENT2(onMiddleButtonUp, int, int);
  FORWARDEVENT2(onMouseWheelUp, int, int);
  FORWARDEVENT2(onMouseWheelDown, int, int);
  FORWARDEVENT2(onMouseMove, int, int);
  FORWARDEVENT0(onGetFocus);
  FORWARDEVENT0X(onKillFocus, onLoseFocus);
  FORWARDEVENT0(onActivate);
  FORWARDEVENT0(onDeactivate);
  FORWARDEVENT1(onChar, unsigned int);
  FORWARDEVENT1(onKeyDown, unsigned int);
  FORWARDEVENT1(onKeyUp, unsigned int);
  _FORWARDEVENT0(onCancelCapture);
  _FORWARDEVENT0(onMinimize);
  _FORWARDEVENT0(onRestore);
  _FORWARDEVENT0(onMaximize);
  virtual void onSetVisible(int show) {
    WNDCLASS::onSetVisible(show);
    if (show) m_xuiobject->onShow();
    else m_xuiobject->onHide();
  }
  virtual int onEnable(int en) {
    WNDCLASS::onEnable(en);
    if (en) m_xuiobject->onEnable();
    else m_xuiobject->onDisable();
    return 0;
  }
  FORWARDEVENT1(onPaint, Canvas *);
  virtual int wantFocus() { return m_xuiobject->wantFocus(); }
  virtual int isClickThrough() { return m_xuiobject->isMouseThrough(); }
  virtual int getPreferences(int what) {
    int pref = m_xuiobject->getPreferences(what);
    if (pref != AUTOWH) return pref;
    return WNDCLASS::getPreferences(what);
  }
  virtual int dragEnter(RootWnd *source) {
    m_xuiobject->dragEnter(source);
    return 0;
  }
  virtual int dragOver(int x, int y, RootWnd *w) {
    WNDCLASS::dragOver(x, y, w);
    return m_xuiobject->dragOver(w, x, y);
  }
  FORWARDEVENT1(dragLeave, RootWnd *);
  FORWARDEVENT3(dragDrop, RootWnd *, int, int);
  FORWARDEVENT1(dragComplete, int);
  virtual int isDndThrough() { return m_xuiobject->isDndThrough(); }

private:

  XUICLASS *m_xuiobject;
  WNDCLASS *m_wnd;

};

#define ACCESS_SOURCE_WND(identifier, classname, parentclass) \
classname *identifier; \
virtual void onSetWindow() { \
  parentclass::onSetWindow(); \
  identifier = static_cast<classname*>(m_wnd); \
}

#endif // XUIWND_H
