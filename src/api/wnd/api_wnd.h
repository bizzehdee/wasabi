#ifndef __API_WND_H
#define __API_WND_H

#include <wasabicfg.h>
#include <bfc/dispatch.h>

class RootWnd;		// see ../bfc/rootwnd.h
class PopupExitCallback; // see ../bfc/popexitcb.h
class CanvasBase;	// see ../common/canvas.h
class CfgItem;

#ifndef MODALWND_DEF
#define MODALWND_DEF
RootWnd *const MODALWND_NOWND = reinterpret_cast<RootWnd*>(-1);
#endif

class wnd_api : public Dispatchable {
  public:

  // the master window
  /**
    Get the main root window.
    
    @ret Main root window.
  */
  RootWnd *main_getRootWnd();
  void main_setRootWnd(RootWnd *w);
  /**
    Get the modal window on top of the
    modal window stack.
    
    @ret Window that's currently modal.
  */
  RootWnd *getModalWnd();
  /**
    Push a window onto the modal stack. If
    the window is on top of the stack, it's
    currently modal.
    
    @param w Window to push onto the modal stack.
  */
  void pushModalWnd(RootWnd *w=MODALWND_NOWND);
  
  /**
    Pop a window from the modal stack. If
    the window is on top of the stack, it
    will no longer be modal after this call.
    
    @param w Window to pop from the modal stack.
  */
  void popModalWnd(RootWnd *w=MODALWND_NOWND);
  RootWnd *rootWndFromPoint(POINT *pt);
  RootWnd *rootWndFromOSHandle(OSWINDOWHANDLE wnd);
  void registerRootWnd(RootWnd *wnd);
  void unregisterRootWnd(RootWnd *wnd);
  int rootwndIsValid(RootWnd *wnd);
  void hookKeyboard(RootWnd *hooker);
  void unhookKeyboard(RootWnd *hooker);
  void kbdReset();
  int interceptOnChar(unsigned int c);
  int interceptOnKeyDown(int k);
  int interceptOnKeyUp(int k);
  int interceptOnSysKeyDown(int k, int kd);
  int interceptOnSysKeyUp(int k, int kd);
  int forwardOnChar(RootWnd *from, unsigned int c, int kd);
  int forwardOnKeyDown(RootWnd *from, int k, int kd);
  int forwardOnKeyUp(RootWnd *from, int k, int kd);
  int forwardOnSysKeyDown(RootWnd *from, int k, int kd);
  int forwardOnSysKeyUp(RootWnd *from, int k, int kd);
  int forwardOnKillFocus();
  int pushKeyboardLock();
  int popKeyboardLock();
  int isKeyboardLocked();
  void popupexit_deregister(PopupExitCallback *cb);
  void popupexit_register(PopupExitCallback *cb, RootWnd *watched);
  int popupexit_check(RootWnd *w);
  void popupexit_signal();
#define RenderBaseTexture renderBaseTexture //CUT
  void skin_renderBaseTexture(RootWnd *base, CanvasBase *c, const RECT &r, RootWnd *destWnd, int alpha=255);
  void skin_registerBaseTextureWindow(RootWnd *window, const char *bmp=NULL);
  void skin_unregisterBaseTextureWindow(RootWnd *window);
  void appdeactivation_push_disallow(RootWnd *w);
  void appdeactivation_pop_disallow(RootWnd *w);
  int appdeactivation_isallowed(RootWnd *w);
  void appdeactivation_setbypass(int i);
#ifdef WASABI_COMPILE_PAINTSETS
  int paintset_present(int set);
#ifdef WASABI_COMPILE_IMGLDR
  void paintset_render(int set, CanvasBase *c, const RECT *r, int alpha=255);
#ifdef WASABI_COMPILE_FONTS
  void paintset_renderTitle(char *t, CanvasBase *c, const RECT *r, int alpha=255);
#endif // fonts
#endif // imgldr
#endif // paintsets
  int forwardOnMouseWheel(int l, int a);
  void setDefaultDropTarget(void *dt);
  void *getDefaultDropTarget();

  enum {
    API_WND_GETROOTWND                     =  10,
    API_WND_SETROOTWND                     =  20,
    API_WND_GETMODALWND                    =  30,
    API_WND_PUSHMODALWND                   =  40,
    API_WND_POPMODALWND                    =  50,
    API_WND_ROOTWNDFROMPOINT               =  60,
    API_WND_ROOTWNDFROMOSHANDLE            =  65,
    API_WND_REGISTERROOTWND                =  70,
    API_WND_UNREGISTERROOTWND              =  80,
    API_WND_ROOTWNDISVALID                 =  90,
    API_WND_INTERCEPTONCHAR                = 100,
    API_WND_INTERCEPTONKEYDOWN             = 110,
    API_WND_INTERCEPTONKEYUP               = 120,
    API_WND_INTERCEPTONSYSKEYDOWN          = 130,
    API_WND_INTERCEPTONSYSKEYUP            = 140,
    API_WND_HOOKKEYBOARD                   = 150,
    API_WND_UNHOOKKEYBOARD                 = 160,
    API_WND_KBDRESET                       = 165,
    API_WND_FORWARDONCHAR                  = 170,
    API_WND_FORWARDONKEYDOWN               = 180,
    API_WND_FORWARDONKEYUP                 = 190,
    API_WND_FORWARDONSYSKEYDOWN            = 200,
    API_WND_FORWARDONSYSKEYUP              = 210,
    API_WND_FORWARDONKILLFOCUS             = 220,
    API_WND_POPUPEXIT_CHECK                = 230,
    API_WND_POPUPEXIT_SIGNAL               = 240,
    API_WND_POPUPEXIT_REGISTER             = 250,
    API_WND_POPUPEXIT_DEREGISTER           = 260,
    API_WND_RENDERBASETEXTURE              = 270,
    API_WND_REGISTERBASETEXTUREWINDOW      = 280,
    API_WND_UNREGISTERBASETEXTUREWINDOW    = 290,
    API_WND_APPDEACTIVATION_PUSH_DISALLOW  = 300,
    API_WND_APPDEACTIVATION_POP_DISALLOW   = 310,
    API_WND_APPDEACTIVATION_ISALLOWED      = 320,
    API_WND_APPDEACTIVATION_SETBYPASS      = 330,
#ifdef WASABI_COMPILE_PAINTSETS
    API_WND_PAINTSET_PRESENT               = 335,
#ifdef WASABI_COMPILE_IMGLDR
    API_WND_PAINTSET_RENDER                = 340,
#ifdef WASABI_COMPILE_FONTS
    API_WND_PAINTSET_RENDERTITLE           = 350,
#endif // fonts
#endif // imgldr
#endif // paintsets
    API_WND_FORWARDONMOUSEWHEEL            = 360,
    // fg> this may need to go away eventually but i need it _right now_
    API_WND_SETDEFAULTDROPTARGET           = 370,
    API_WND_GETDEFAULTDROPTARGET           = 380,
    API_WND_PUSHKBDLOCK                    = 390,
    API_WND_POPKBDLOCK                     = 400,
    API_WND_ISKBDLOCKED                    = 410,
  };
};


inline RootWnd *wnd_api::main_getRootWnd() {
  return _call(API_WND_GETROOTWND, (RootWnd *)0 );
}

inline void wnd_api::main_setRootWnd(RootWnd *w) {
  _voidcall(API_WND_SETROOTWND, w );
}

inline RootWnd *wnd_api::getModalWnd() {
  return _call(API_WND_GETMODALWND, (RootWnd *)0  );
}

inline void wnd_api::pushModalWnd(RootWnd *w) {
  _voidcall(API_WND_PUSHMODALWND, w);
}

inline void wnd_api::popModalWnd(RootWnd *w) {
  _voidcall(API_WND_POPMODALWND, w);
}

inline RootWnd *wnd_api::rootWndFromPoint(POINT *pt) {
  return _call(API_WND_ROOTWNDFROMPOINT, (RootWnd *)0, pt );
}

inline RootWnd *wnd_api::rootWndFromOSHandle(OSWINDOWHANDLE wnd) {
  return _call(API_WND_ROOTWNDFROMOSHANDLE, (RootWnd *)0, wnd);
}

inline void wnd_api::registerRootWnd(RootWnd *wnd) {
  _voidcall(API_WND_REGISTERROOTWND, wnd );
}

inline void wnd_api::unregisterRootWnd(RootWnd *wnd) {
  _voidcall(API_WND_UNREGISTERROOTWND, wnd );
}

inline int wnd_api::rootwndIsValid(RootWnd *wnd) {
  return _call(API_WND_ROOTWNDISVALID, (int)0, wnd );
}

inline int wnd_api::interceptOnChar(unsigned int c) {
  return _call(API_WND_INTERCEPTONCHAR, (int)0, c );
}

inline int wnd_api::interceptOnKeyDown(int k) {
  return _call(API_WND_INTERCEPTONKEYDOWN, (int)0, k );
}

inline int wnd_api::interceptOnKeyUp(int k) {
  return _call(API_WND_INTERCEPTONKEYUP, (int)0, k );
}

inline int wnd_api::interceptOnSysKeyDown(int k, int kd) {
  return _call(API_WND_INTERCEPTONSYSKEYDOWN, (int)0, k , kd );
}

inline int wnd_api::interceptOnSysKeyUp(int k, int kd) {
  return _call(API_WND_INTERCEPTONSYSKEYUP, (int)0, k , kd );
}

inline void wnd_api::hookKeyboard(RootWnd *hooker) {
  _voidcall(API_WND_HOOKKEYBOARD, hooker);
}

inline void wnd_api::unhookKeyboard(RootWnd *hooker) {
  _voidcall(API_WND_UNHOOKKEYBOARD, hooker);
}

inline void wnd_api::kbdReset() {
  _voidcall(API_WND_KBDRESET);
}

inline int wnd_api::forwardOnChar(RootWnd *from, unsigned int c, int kd) {
  return _call(API_WND_FORWARDONCHAR, (int)0, from, c, kd );
}

inline int wnd_api::forwardOnKeyDown(RootWnd *from, int k, int kd) {
  return _call(API_WND_FORWARDONKEYDOWN, (int)0, from, k, kd);
}

inline int wnd_api::forwardOnKeyUp(RootWnd *from, int k, int kd) {
  return _call(API_WND_FORWARDONKEYUP, (int)0, from, k, kd );
}

inline int wnd_api::forwardOnSysKeyDown(RootWnd *from, int k, int kd) {
  return _call(API_WND_FORWARDONSYSKEYDOWN, (int)0, from, k , kd );
}

inline int wnd_api::forwardOnSysKeyUp(RootWnd *from, int k, int kd) {
  return _call(API_WND_FORWARDONSYSKEYUP, (int)0, from, k , kd );
}

inline int wnd_api::forwardOnKillFocus() {
  return _call(API_WND_FORWARDONKILLFOCUS, (int)0  );
}

inline int wnd_api::popupexit_check(RootWnd *w) {
  return _call(API_WND_POPUPEXIT_CHECK, 0, w);
}

inline void wnd_api::popupexit_signal() {
  _voidcall(API_WND_POPUPEXIT_SIGNAL);
}

inline void wnd_api::popupexit_register(PopupExitCallback *cb, RootWnd *watched) {
  _voidcall(API_WND_POPUPEXIT_REGISTER, cb, watched);
}

inline void wnd_api::popupexit_deregister(PopupExitCallback *cb) {
  _voidcall(API_WND_POPUPEXIT_DEREGISTER, cb);
}

#define RenderBaseTexture renderBaseTexture //CUT

inline void wnd_api::skin_renderBaseTexture(RootWnd *base, CanvasBase *c, const RECT &r, RootWnd *destWnd, int alpha) {
  _voidcall(API_WND_RENDERBASETEXTURE, base, c, &r, destWnd, alpha );
}

inline void wnd_api::skin_registerBaseTextureWindow(RootWnd *window, const char *bmp) {
  _voidcall(API_WND_REGISTERBASETEXTUREWINDOW, window, bmp);
}

inline void wnd_api::skin_unregisterBaseTextureWindow(RootWnd *window) {
  _voidcall(API_WND_UNREGISTERBASETEXTUREWINDOW, window );
}

inline void wnd_api::appdeactivation_push_disallow(RootWnd *w) {
  _voidcall(API_WND_APPDEACTIVATION_PUSH_DISALLOW, w);
}

inline void wnd_api::appdeactivation_pop_disallow(RootWnd *w) {
  _voidcall(API_WND_APPDEACTIVATION_POP_DISALLOW, w);
}

inline int wnd_api::appdeactivation_isallowed(RootWnd *w) {
  return _call(API_WND_APPDEACTIVATION_ISALLOWED, 0, w);
}

inline void wnd_api::appdeactivation_setbypass(int i) {
  _voidcall(API_WND_APPDEACTIVATION_SETBYPASS, i);
}

#ifdef WASABI_COMPILE_PAINTSETS
inline int wnd_api::paintset_present(int set) {
  return _call(API_WND_PAINTSET_PRESENT, 0, set);
}

#ifdef WASABI_COMPILE_IMGLDR

inline void wnd_api::paintset_render(int set, CanvasBase *c, const RECT *r, int alpha) {
  _voidcall(API_WND_PAINTSET_RENDER, set, c, r, alpha);
}

#ifdef WASABI_COMPILE_FONTS
inline void wnd_api::paintset_renderTitle(char *t, CanvasBase *c, const RECT *r, int alpha) {
  _voidcall(API_WND_PAINTSET_RENDERTITLE, t, c, r, alpha);
}
#endif // fonts
#endif // imgldr
#endif // paintsets

inline int wnd_api::forwardOnMouseWheel(int l, int a) {
  return _call(API_WND_FORWARDONMOUSEWHEEL, 0, l, a);
}

// fg> this may need to go away eventually but i need it _right now_

inline void wnd_api::setDefaultDropTarget(void *dt) {
  _voidcall(API_WND_SETDEFAULTDROPTARGET, dt);
}

inline void *wnd_api::getDefaultDropTarget() {
  return _call(API_WND_GETDEFAULTDROPTARGET, (void*)NULL);
}

inline int wnd_api::pushKeyboardLock() {
  return _call(API_WND_PUSHKBDLOCK, 0);
}

inline int wnd_api::popKeyboardLock() {
  return _call(API_WND_POPKBDLOCK, 0);
}

inline int wnd_api::isKeyboardLocked() {
  return _call(API_WND_ISKBDLOCKED, 0);
}

class wnd_apiI : public wnd_api {
  public:
    virtual void main_setRootWnd(RootWnd *w)=0;
    virtual RootWnd *main_getRootWnd()=0;
    virtual RootWnd *getModalWnd()=0;
    virtual void pushModalWnd(RootWnd *w=MODALWND_NOWND)=0;
    virtual void popModalWnd(RootWnd *w=MODALWND_NOWND)=0;
    virtual RootWnd *rootWndFromPoint(POINT *pt)=0;
    virtual RootWnd *rootWndFromOSHandle(OSWINDOWHANDLE wnd)=0;
    virtual void registerRootWnd(RootWnd *wnd)=0;
    virtual void unregisterRootWnd(RootWnd *wnd)=0;
    virtual int rootwndIsValid(RootWnd *wnd)=0;
    virtual void hookKeyboard(RootWnd *hooker)=0;
    virtual void unhookKeyboard(RootWnd *hooker)=0;
    virtual void kbdReset()=0;
    virtual int interceptOnChar(unsigned int c)=0;
    virtual int interceptOnKeyDown(int k)=0;
    virtual int interceptOnKeyUp(int k)=0;
    virtual int interceptOnSysKeyDown(int k, int kd)=0;
    virtual int interceptOnSysKeyUp(int k, int kd)=0;
    virtual int forwardOnChar(RootWnd *from, unsigned int c, int kd)=0;
    virtual int forwardOnKeyDown(RootWnd *from, int k, int kd)=0;
    virtual int forwardOnKeyUp(RootWnd *from, int k, int kd)=0;
    virtual int forwardOnSysKeyDown(RootWnd *from, int k, int kd)=0;
    virtual int forwardOnSysKeyUp(RootWnd *from, int k, int kd)=0;
    virtual int forwardOnKillFocus()=0;
    virtual void popupexit_deregister(PopupExitCallback *cb)=0;
    virtual void popupexit_register(PopupExitCallback *cb, RootWnd *watched)=0;
    virtual int popupexit_check(RootWnd *w)=0;
    virtual void popupexit_signal()=0;
    virtual void skin_renderBaseTexture(RootWnd *base, CanvasBase *c, const RECT *r, RootWnd *destWnd, int alpha=255)=0;
    virtual void skin_registerBaseTextureWindow(RootWnd *window, const char *bmp=NULL)=0;
    virtual void skin_unregisterBaseTextureWindow(RootWnd *window)=0;
    virtual void appdeactivation_push_disallow(RootWnd *w)=0;
    virtual void appdeactivation_pop_disallow(RootWnd *w)=0;
    virtual int appdeactivation_isallowed(RootWnd *w)=0;
    virtual void appdeactivation_setbypass(int i)=0;
#ifdef WASABI_COMPILE_PAINTSETS
    virtual int paintset_present(int set)=0;
#ifdef WASABI_COMPILE_IMGLDR
    virtual void paintset_render(int set, CanvasBase *c, const RECT *r, int alpha=255)=0;
#ifdef WASABI_COMPILE_FONTS
    virtual void paintset_renderTitle(char *t, CanvasBase *c, const RECT *r, int alpha=255)=0;
#endif // fonts
#endif // imgldr
#endif // paintsets
    virtual int forwardOnMouseWheel(int l, int a)=0;
    // fg> this may need to go away eventually but i need it _right now_
    virtual void setDefaultDropTarget(void *dt)=0;
    virtual void *getDefaultDropTarget()=0;
    virtual int pushKeyboardLock()=0;
    virtual int popKeyboardLock()=0;
    virtual int isKeyboardLocked()=0;
  protected:
    RECVS_DISPATCH;
};

// {ABB8FBC7-6255-4d41-ACAB-D3D61DDD74EE}
static const GUID wndApiServiceGuid = 
{ 0xabb8fbc7, 0x6255, 0x4d41, { 0xac, 0xab, 0xd3, 0xd6, 0x1d, 0xdd, 0x74, 0xee } };

extern wnd_api *wndApi;

#endif
