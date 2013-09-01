#ifndef __SYSTRAY_H
#define __SYSTRAY_H

#include <bfc/string/string.h>

class SystrayIcon {
public: 
  SystrayIcon(OSWINDOWHANDLE wnd, int uid, int msg, OSICONHANDLE smallicon);
  ~SystrayIcon();
  void setTip(const char *tip);
  void setIcon(OSICONHANDLE hicon);

private:
#ifdef WASABI_PLATFORM_WIN32
  BOOL addIcon();
  BOOL deleteIcon();
  BOOL setTip();
  int id, message;
  OSWINDOWHANDLE hwnd;
  OSICONHANDLE icon;
  String tip;
#else
#warning port me: SystrayIcon
#endif
};

#endif
