#ifndef _CURSOR_H
#define _CURSOR_H

#include <bfc/dispatch.h>
//#include "../studio/skincb.h"
#include <bfc/string/string.h>

class Cursor : public Dispatchable {
public:
  OSCURSORHANDLE getOSHandle();

  enum {
    CURSOR_GETOSHANDLE = 0,
  };
};

inline OSCURSORHANDLE Cursor::getOSHandle() {
  return _call(CURSOR_GETOSHANDLE, (OSCURSORHANDLE)NULL);
}

class CursorI : public Cursor {
public:
  CursorI() {}
  virtual ~CursorI() {}

  virtual OSCURSORHANDLE getOSHandle()=0;
  
protected:
  RECVS_DISPATCH;
};

#ifdef WASABI_COMPILE_SKIN
#include <api/syscb/callbacks/skincb.h>
class SkinCursor : public CursorI, public SkinCallbackI {
public:
  SkinCursor();
  SkinCursor(const char *elementid);
  virtual ~SkinCursor();
  
  virtual void setCursorElementId(const char *id);
  virtual int skincb_onReset();
  virtual OSCURSORHANDLE getOSHandle();
  virtual void reset();

private:
  String name;
  OSCURSORHANDLE cursor;
};

#endif

#endif
