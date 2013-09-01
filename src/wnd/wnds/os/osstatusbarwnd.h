#ifndef OSSTATUSBARWND_H
#define OSSTATUSBARWND_H

#include <wnd/basewnd.h>
#include <commctrl.h>

#define OSSTATUSBARWND_PARENT BaseWnd 

class OSStatusBarWnd;
class OSToolTip;
class CommandCallback;

// styles for win32 :
// SBT_NOBORDERS  0x100
// SBT_OWNERDRAW  0x1000
// SBT_POPOUT     0x200
// SBT_RTLREADING 0x400
#define SBP_STRETCH    0x08000000

class StatusBarPart {
public:
  StatusBarPart(OSStatusBarWnd *parent);
  void copyFrom(StatusBarPart * part);
  void setWidth(int width);
  int getWidth() { return m_width; }
  void setText(const char *text);
  const char *getText() { return m_text; }
  void setStyle(int style);
  int getStyle() { return m_style; }
  void setTooltip(const char *text);
  const char *getTooltip() { return m_tooltip; }
  void setIcon(HICON icon);
  HICON getIcon() { return m_icon; }
  void setCommand(int type, int id);
  int getCommand(int type);
  void clearCommand(int type) { setCommand(type,-1); }
  void render(HDC hdc,RECT* r,DWORD data);
  void setData(DWORD value);
  DWORD getData();
  void clean() { m_dirty = 0; }
  int isDirty() { return m_dirty; }
private:
  int m_width;
  int m_commands[6];
  OSStatusBarWnd *m_parent;
  DWORD m_data;
  String m_text;
  String m_tooltip;
  HICON m_icon;
  int m_style;
  int m_dirty;
};

class OSStatusBarWnd : public OSSTATUSBARWND_PARENT {
public:
  OSStatusBarWnd();
  virtual ~OSStatusBarWnd();
  virtual int onInit();
  virtual const char *getWindowClass() { return STATUSCLASSNAME; }
  virtual LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual int onResize();  
  virtual int onLeftButtonDown(int x, int y);
  virtual int onMiddleButtonDown(int x, int y);
  virtual int onRightButtonDown(int x, int y);
  virtual int wantDoubleClicks() { return 0; }
  virtual int onLeftButtonDblClk(int x, int y) ;
  virtual int onMiddleButtonDblClk(int x, int y);
  virtual int onRightButtonDblClk(int x, int y);
  virtual int handleDblClicks(UINT uMsg, int x, int y);

  void renderPart(int part,HDC hdc,RECT* r,DWORD data);
  int hitTest(int x,int y);
  int handleCommand(int type, int x, int y);

  void setCommandCallback(CommandCallback *cc) { m_cc = cc; }
  CommandCallback *getCommandCallback() { return m_cc; }

  virtual void timerCallback(int id);
  StatusBarPart *addNewPart();
  void removePart(StatusBarPart *part);
  StatusBarPart *enumPart(int n);
  int getNumParts();

  void updateStatusBar();
  void resizeParts();
  void setBulkUpdate(int b);

private:
  OSToolTip * m_tooltipwnd;
  PtrList<StatusBarPart> m_parts;
  bool m_inBulkUpdate;
  CommandCallback *m_cc;
  POINT lastClickP[3];
};



#endif

