#ifndef WASABI_EDITWND_H
#define WASABI_EDITWND_H

#include <bfc/attrib/attrcolor.h>
#include <wnd/virtualwnd.h>
#include <wnd/usermsg.h>
#include <wnd/notifmsg.h>

#define EDITWND_PARENT VirtualWnd

class EditWnd : public EDITWND_PARENT {
public:
  EditWnd(char *buffer=NULL, int buflen=0, ARGB32 bgcolour=RGB(255,255,255), ARGB32 textcolour=RGB(0,0,0));
  virtual ~EditWnd();

  virtual int onInit();
  virtual int onPaint(Canvas *canvas);
  virtual int onResize();
#ifdef WIN32
  virtual LRESULT wndProc(OSWINDOWHANDLE hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

  // mig: Made these virtual to allow to be accessed by 
  // EditWndString object in editwndstring.h
  virtual void setBuffer(char *buffer, int len);
  virtual void getBuffer(char *outbuf, int len);

  virtual const char *getBufferPtr() { return outbuf; }
  virtual int getBufferLength() { return maxlen; }
  virtual void setBackgroundColor(COLORREF c);
  virtual void setTextColor(COLORREF c);
  virtual void setDisabledBackgroundColor(COLORREF c);

  void setModal(int modal);	//if modal, deletes self on enter
  void setAutoEnter(int a);	//fake an onEnter event when lose focus
  int getAutoEnter() { return autoenter; }
  void setAutoSelect(int a);	//true==grab the focus on init
  void setIdleTimerLen(int ms);	// how many ms keys are idle before send msg
  virtual void onSetVisible(int show);
  virtual int onGetFocus();
  virtual int wantFocus();
  virtual void setWantFocus(int w) { wantfocus = w; }
  virtual void selectAll();
  virtual void enter();
  virtual void setIdleEnabled(int i) { idleenabled = i; }
  virtual int getIdleEnabled() { return idleenabled; }

  //void setBorder(int border);
  int getTextLength();
  virtual void setEnabled(int en);
  
  OSWINDOWHANDLE getEditWnd();
  virtual int handleRatio() { return 0; }
  virtual int getAutoSelect() { return autoselect; }

  void setMultiline(int ml);
  void setReadOnly(int ro);
  void setPassword(int pw);
  void setAutoHScroll(int hs);
  void setAutoVScroll(int vs);
  void setVScroll(int vs);
  int isEditorKey(int vk);
  virtual void invalidate();

  virtual int gotFocus();

  // the wndproc for the edit box
  virtual LRESULT editWndProc(OSWINDOWHANDLE hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  virtual int onCustomEditPopup(int x, int y) { return 0; }

protected:
  virtual void timerCallback(int id);

  // call down on these if you override them
  virtual void onEditUpdate();
  virtual void onIdleEditUpdate();
  virtual int onEnter();	// user hit enter.. return 1 to close window
  virtual int onAbort();	// user hit escape.. return 1 to close window
  virtual int onLoseFocus();	// different from onKillFocus() from BaseWnd!

  void setStyle(LONG style, int set);

#ifdef LINUX
  virtual int onLeftButtonDown( int x, int y );
  virtual int onLeftButtonUp( int x, int y );
  virtual int onMouseMove( int x, int y );
  virtual int onKeyDown(int key);
#endif

private:
#ifdef LINUX
  int textposFromCoord( int x, int y );
#endif

  OSWINDOWHANDLE editWnd;
  WNDPROC prevWndProc;
  int maxlen;
  int retcode;
  int idletimelen;
  int modal;
  //int bordered;
  int autoenter;
  int beforefirstresize;
  int autoselect;
  int multiline;
  int readonly;
  int password;
  int idleenabled;
  int autohscroll,autovscroll,vscroll;
  int nextenterfaked;
  int enabled;
  ARGB32 backgroundcolor, textcolor, selectioncolor;
  ARGB32 dbackgroundcolor, dtextcolor;
#ifdef LINUX
  int selstart, selend;
  int cursorpos;
  int selectmode;
  int viewstart;
#endif
#ifdef WIN32
  HBRUSH oldbrush;
  HBRUSH doldbrush;
#endif

  // Basically, we're redoing the functionality of EditWndString 
  // (the bigger version), so we'll probably erase EditWndString 
  // completely as an object.
  MemBlock<char> buffer8;
  char *outbuf;
  int wantfocus;
#ifdef LINUX
  String inbuf;
#endif
};

#define EDITWND_RETURN_NOTHING	0	// user didn't do nothing
#define EDITWND_RETURN_OK	1	// user hit return
#define EDITWND_RETURN_CANCEL	2	// user hit escape or something

#endif
