#ifndef _WNDCB_H
#define _WNDCB_H

#include <api/syscb/callbacks/syscbi.h>
#include <bfc/common.h>

class Container;
class RootWnd;

class WndInfo {
  public:
  GUID guid;
  const char *groupid;
  const char *wndtype;
  Container *c;
};

namespace WndCallback {
  enum {
    SHOWWINDOW=10,
    HIDEWINDOW=20,
    GROUPCHANGE=30,
    TYPECHANGE=40,
  };
};

#define WNDCALLBACKI_PARENT SysCallbackI
class COMEXP WndCallbackI : public WNDCALLBACKI_PARENT {
public:
  virtual FOURCC syscb_getEventType() { return SysCallback::WINDOW; }

protected:
  virtual int onShowWindow(Container *c, GUID guid, const char *groupid) { return 0; }
  virtual int onHideWindow(Container *c, GUID guid, const char *groupid) { return 0; }
  virtual int onGroupChange(const char *groupid) { return 0; }
  virtual int onTypeChange(const char *type) { return 0; }

private:
  virtual int syscb_notify(int msg, int param1=0, int param2=0);

};

#endif
