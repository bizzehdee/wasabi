#ifndef _PAINTCB_H
#define _PAINTCB_H

#include <bfc/depview.h>
#include <bfc/dispatch.h>

#include "rootwnd.h"

class Canvas;
class Region;

class NOVTABLE PaintCallbackInfo : public Dispatchable {
protected:
  PaintCallbackInfo() { }

public:
  Canvas *getCanvas();
  Region *getRegion();

  enum {
    PAINTCBINFO_GETCANVAS = 10,
    PAINTCBINFO_GETREGION = 20,
  };
};

inline Canvas *PaintCallbackInfo::getCanvas() {
  return _call(PAINTCBINFO_GETCANVAS, (Canvas *)NULL);
}

inline Region *PaintCallbackInfo::getRegion() {
  return _call(PAINTCBINFO_GETREGION, (Region *)NULL);
}

class NOVTABLE PaintCallbackInfoI : public PaintCallbackInfo {
public:
  PaintCallbackInfoI(Canvas *_canvas, Region *_region) : canvas(_canvas), region(_region) { }
  virtual ~PaintCallbackInfoI() { }

  virtual Canvas *getCanvas() { return canvas; }
  virtual Region *getRegion() { return region; }

private:
  Canvas *canvas;
  Region *region;

protected:
  RECVS_DISPATCH;
};

class PaintCallback : DependentViewerTPtr<RootWnd> {
public:
  PaintCallback() { wnd = NULL; };
  PaintCallback(RootWnd *w);
  virtual ~PaintCallback();

  virtual void monitorWindow(RootWnd *w);
  virtual int viewer_onEvent(RootWnd *item, int event, int param, void *ptr, int ptrlen);
  virtual int viewer_onItemDeleted(RootWnd *item);

  // override those
  virtual void onBeforePaint(PaintCallbackInfo *info) { }
  virtual void onAfterPaint(PaintCallbackInfo *info) { }
  virtual void onWindowDeleted(RootWnd *w)=0; // warning, pointer invalid
  virtual void onInvalidation(PaintCallbackInfo *info) { }

  enum {
    BEFOREPAINT = 10,
    AFTERPAINT = 20,
  };

private:
  RootWnd *wnd;
};

#endif
