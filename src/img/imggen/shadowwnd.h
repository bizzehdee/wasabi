#ifndef _SHADOW_H
#define _SHADOW_H

#include "../common/guiobjwnd.h"
#include "../bfc/paintcb.h"

#define XUISHADOWWND_PARENT GuiObjectWnd
class XuiShadowWnd : public XUISHADOWWND_PARENT, public PaintCallback {
public:
  static const char *xuiobject_getXmlTag() { return "Shadow"; }

  XuiShadowWnd();
  virtual ~XuiShadowWnd();

  virtual int onInit();

  virtual int onPaint(Canvas *canvas);

  virtual int setXuiParam(int xuihandle, int xmlattributeid, const char *xmlattributename, const char *value);

  virtual void onAfterPaint(PaintCallbackInfo *info);
  virtual void onInvalidation(PaintCallbackInfo *info);
protected:
  virtual void onWindowDeleted(RootWnd *w);
  void attachToGroup();
  virtual void timerclient_timerCallback(int id);

private:
  int myxuihandle;
  String targetname;
  RootWnd *group;
  BltCanvas *bltcanvas;
  int c_w, c_h;
  int in_paint;
};

class XuiShadowWndSvc : public XuiObjectSvc2<XuiShadowWnd> {};

#endif
