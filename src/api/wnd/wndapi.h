#ifndef _WASABI_WNDAPI_H_
#define _WASABI_WNDAPI_H_

#include <bfc/std.h>
#include <bfc/ptrlist.h>
#include <bfc/stack.h>
#include <wnd/basewnd.h>
#include "api_wnd.h"

class wndapi : public wnd_apiI {
public:
  wndapi();
  ~wndapi();

  static const char *getServiceName() { return "Windowing API"; }
  static GUID getServiceType() { return WaSvc::UNIQUE; }

  virtual void main_setRootWnd(RootWnd *w);
  virtual RootWnd *main_getRootWnd();
  virtual RootWnd *getModalWnd();
  virtual void pushModalWnd(RootWnd *w=MODALWND_NOWND);
  virtual void popModalWnd(RootWnd *w=MODALWND_NOWND);
  virtual RootWnd *rootWndFromPoint(POINT *pt) {return NULL;}//stub
  virtual RootWnd *rootWndFromOSHandle(OSWINDOWHANDLE wnd) {return NULL;}//stub
  virtual void registerRootWnd(RootWnd *wnd);
  virtual void unregisterRootWnd(RootWnd *wnd);
  virtual int rootwndIsValid(RootWnd *wnd);
  virtual void hookKeyboard(RootWnd *hooker) {return;}//stub
  virtual void unhookKeyboard(RootWnd *hooker) {return;}//stub
  virtual void kbdReset() {return;}//stub
  //check a class to see if the system wants to
  //process these keys before the window handles them
  virtual int interceptOnChar(unsigned int c) {return 0;}//stub
  virtual int interceptOnKeyDown(int k) {return 0;}//stub
  virtual int interceptOnKeyUp(int k) {return 0;}//stub
  virtual int interceptOnSysKeyDown(int k, int kd) {return 0;}//stub
  virtual int interceptOnSysKeyUp(int k, int kd) {return 0;}//stub
  //if the system wants to process these keys, then
  //pass the command to a class that will fire an action
  virtual int forwardOnChar(RootWnd *from, unsigned int c, int kd) {return 0;}//stub
  virtual int forwardOnKeyDown(RootWnd *from, int k, int kd) {return 0;}//stub
  virtual int forwardOnKeyUp(RootWnd *from, int k, int kd) {return 0;}//stub
  virtual int forwardOnSysKeyDown(RootWnd *from, int k, int kd) {return 0;}//stub
  virtual int forwardOnSysKeyUp(RootWnd *from, int k, int kd) {return 0;}//stub
  virtual int forwardOnKillFocus() {return 0;}//stub
  //popupexit?
  virtual void popupexit_deregister(PopupExitCallback *cb) {return;}//stub
  virtual void popupexit_register(PopupExitCallback *cb, RootWnd *watched) {return;}//stub
  virtual int popupexit_check(RootWnd *w) {return 0;}//stub
  virtual void popupexit_signal() {return;}//stub
  //
  virtual void skin_renderBaseTexture(RootWnd *base, CanvasBase *c, const RECT *r, RootWnd *destWnd, int alpha=255) {return;}//stub
  virtual void skin_registerBaseTextureWindow(RootWnd *window, const char *bmp=NULL) {return;}//stub
  virtual void skin_unregisterBaseTextureWindow(RootWnd *window) {return;}//stub
  virtual void appdeactivation_push_disallow(RootWnd *w) {return;}//stub
  virtual void appdeactivation_pop_disallow(RootWnd *w) {return;}//stub
  virtual int appdeactivation_isallowed(RootWnd *w) {return 0;}//stub
  virtual void appdeactivation_setbypass(int i) {return;}//stub
#ifdef WASABI_COMPILE_PAINTSETS
  virtual int paintset_present(int set) {return 0;}//stub
#ifdef WASABI_COMPILE_IMGLDR
  virtual void paintset_render(int set, CanvasBase *c, const RECT *r, int alpha=255) {return;}//stub
#ifdef WASABI_COMPILE_FONTS
  virtual void paintset_renderTitle(char *t, CanvasBase *c, const RECT *r, int alpha=255) {return;}//stub
#endif // fonts
#endif // imgldr
#endif // paintsets
  virtual int forwardOnMouseWheel(int l, int a) {return 0;}//stub
  // fg> this may need to go away eventually but i need it _right now_
  virtual void setDefaultDropTarget(void *dt) {return;}//stub
  virtual void *getDefaultDropTarget() {return NULL;}//stub
  virtual int pushKeyboardLock() {return 0;}//stub
  virtual int popKeyboardLock() {return 0;}//stub
  virtual int isKeyboardLocked() {return 0;}//stub
private:
  RootWnd *m_root_wnd;
  Stack<RootWnd*> m_root_stack;
  PtrList<RootWnd> m_root_list;
};

#endif //!_WASABI_WNDAPI_H_
