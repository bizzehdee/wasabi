#ifndef _SUBCLASS_H
#define _SUBCLASS_H

class SubClasser {
public:
  SubClasser();
  virtual ~SubClasser();

  void subclass(HWND hwnd);
  void unsubclass();

  virtual int onSubclassWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &retval) {
//    retval = 0;
//    switch (uMsg) {
//      default:
        return 0;
//    }
//    return 1;
  }

  HWND subclass_hwnd;
  WNDPROC prev_wndproc;
};

#endif
