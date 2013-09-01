#include <precomp.h>
#include <windows.h>
#include <shellapi.h>
#include "systray.h"
#include <bfc/std.h>
#include <bfc/string/encodedstr.h>

// sorry, this isn't portable... but the interface should be

SystrayIcon::SystrayIcon(OSWINDOWHANDLE wnd, int uid, int msg, OSICONHANDLE smallicon) {
  ASSERT(wnd != NULL);
  id = uid;
  hwnd = wnd;
  message = msg;
  icon = smallicon;
  int r = addIcon();
}

SystrayIcon::~SystrayIcon() {
  int r = deleteIcon();
}

void SystrayIcon::setTip(const char *_tip) {
  tip = _tip;
  tip.trunc(64);
  setTip();
}

BOOL SystrayIcon::addIcon() { 
  BOOL res; 
  NOTIFYICONDATAW tnid; 
 
  tnid.cbSize = sizeof(NOTIFYICONDATAW); 
  tnid.uID = id; 
  tnid.hWnd = hwnd; 
  tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
  tnid.uCallbackMessage = message; 
  tnid.hIcon = icon;
  if (tip.numCharacters() > 64) {
    tip.trunc(61);
    tip += "...";
  }
  if (tip) {
    EncodedStr enc;
    enc.convertFromUTF8(SvcStrCnv::UTF16, tip);
    MEMCPY(tnid.szTip, enc.getEncodedBuffer(), enc.getBufferSize()); 
  } else {
    tnid.szTip[0] = '\0'; 
  }
 
  res = Shell_NotifyIconW(NIM_ADD, &tnid); 
 
  return res; 
} 

BOOL SystrayIcon::setTip() {
  BOOL res;
  NOTIFYICONDATAW tnid;
 
  tnid.cbSize = sizeof(NOTIFYICONDATAW);
  tnid.uFlags = NIF_TIP;
  tnid.uID = id;
  tnid.hWnd = hwnd;
  if (tip.numCharacters() > 64) {
    tip.trunc(61);
    tip += "...";
  }
  if (tip && *tip) {
    EncodedStr enc;
    enc.convertFromUTF8(SvcStrCnv::UTF16, tip);
    ASSERTPR(enc.getEncodedBuffer()!=NULL, "utf8->utf16 failed, probably need to link in the UTF8 service");
    MEMCPY(tnid.szTip, enc.getEncodedBuffer(), MIN(126, enc.getBufferSize())); 
    tnid.szTip[63] = 0;
  } else {
    tnid.szTip[0] = '\0'; 
  }
 
  res = Shell_NotifyIconW(NIM_MODIFY, &tnid); 
 
  return res; 
}

void SystrayIcon::setIcon(OSICONHANDLE hicon) {
  NOTIFYICONDATAW tnid; 

  icon = hicon;

  tnid.cbSize = sizeof(NOTIFYICONDATAW); 
  tnid.uFlags = NIF_ICON; 
  tnid.uID = id;
  tnid.hWnd = hwnd;
  tnid.hIcon = icon;
  Shell_NotifyIconW(NIM_MODIFY, &tnid); 
}
 
BOOL SystrayIcon::deleteIcon() { 
  BOOL res; 
  NOTIFYICONDATAW tnid; 
 
  tnid.cbSize = sizeof(NOTIFYICONDATAW); 
  tnid.hWnd = hwnd; 
  tnid.uID = id; 
		 
  res = Shell_NotifyIconW(NIM_DELETE, &tnid); 
  return res; 
} 

