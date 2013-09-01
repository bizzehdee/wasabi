#include <precomp.h>
#include "wndapi.h"

wnd_api *wndApi = NULL;

wndapi::wndapi() {
  m_root_wnd = NULL;
}

wndapi::~wndapi() {

}

void wndapi::main_setRootWnd(RootWnd *w) {
  if(!w) return;
  if(w == MODALWND_NOWND) return;

  m_root_wnd = w;
}

RootWnd *wndapi::main_getRootWnd() {
  return m_root_wnd;
}

RootWnd *wndapi::getModalWnd() {
  if (!m_root_stack.peek()) return NULL;
  return m_root_stack.top();
}

void wndapi::pushModalWnd(RootWnd *w) {
  if(!w) return;
  m_root_stack.push(w);
}

void wndapi::popModalWnd(RootWnd *w) {
  if(!w) return;
  m_root_stack.pop(&w);
}

void wndapi::registerRootWnd(RootWnd *wnd) {
  if(!wnd) return;

  m_root_list.addItem(wnd);
}

void wndapi::unregisterRootWnd(RootWnd *wnd) {
  if(!wnd) return;

  m_root_list.removeItem(wnd);
}

int wndapi::rootwndIsValid(RootWnd *wnd) {
  if(!wnd) return -1;

  if(m_root_list.haveItem(wnd)) return 1;
  return 0;
}
