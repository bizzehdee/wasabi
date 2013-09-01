#include "precomp.h"

#include "subclass.h"

#include <bfc/map.h>

static Map<HWND, SubClasser*> map;

static
LRESULT CALLBACK myproc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
  SubClasser *t;
  if (map.getItem(hwnd, &t)) {
    LRESULT ret=0;
    if (t->onSubclassWndProc(hwnd, iMsg, wParam, lParam, ret)) return ret;
  }
  return CallWindowProc(t->prev_wndproc, hwnd, iMsg, wParam, lParam);
}

SubClasser::SubClasser() {
  subclass_hwnd = NULL;
}

SubClasser::~SubClasser() {
  if (subclass_hwnd != 0) map.delItem(subclass_hwnd);
}

void SubClasser::subclass(HWND hwnd) {
  if (hwnd == NULL) return;
  subclass_hwnd = hwnd;
  map.addItem(hwnd, this);
  prev_wndproc = (WNDPROC)GetWindowLong(hwnd, GWL_WNDPROC);
  SetWindowLong(hwnd, GWL_WNDPROC, (LONG)myproc);
}

void SubClasser::unsubclass() {
  if (subclass_hwnd != NULL) {
    map.delItem(subclass_hwnd);
    ASSERT(prev_wndproc != 0);
    SetWindowLong(subclass_hwnd, GWL_WNDPROC, (LONG)prev_wndproc);
    prev_wndproc = 0;
    subclass_hwnd = NULL;
  }
}
