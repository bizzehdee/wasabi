#ifndef OSTOOLTIP_H
#define OSTOOLTIP_H

#include <commctrl.h>
#include <wnd/basewnd.h>

class OSToolTip : public BaseWnd {
public:
  OSToolTip(const char *text=NULL);
  virtual int init(RootWnd *parent);
  virtual LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int wantActivation() { return 0; }
  virtual void resize(int x, int y, int w, int h) {}
  virtual const char *getWindowClass() { return TOOLTIPS_CLASSA; }
  virtual int setTool(int id, RECT *r, const char *text);
  virtual int setToolRect(int id, RECT *r);

private:
  String m_text;
};

#endif
