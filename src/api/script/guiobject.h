#ifndef __GUIOBJECT_H
#define __GUIOBJECT_H

#include "bfc/dispatch.h"
#include "bfc/nsguid.h"

class RootWnd; // rootwnd.h
class ScriptObject; // scriptobj.h
class GuiObject; // this file
class RootObject; // rootobj.h
class svc_xuiObject; // studio/services/svc_xuiobject.h
class GuiStatusCallback; 
class AppCmds;
class CfgItem;
class FindObjectCallback;

class Group;
class Layout;

class GuiObject : public Dispatchable {
public:

  RootWnd *guiobject_getRootWnd();
  void guiobject_setRootWnd(RootWnd *w);

  void guiobject_getGuiPosition(int *x, int *y, int *w, int *h, int *rx, int *ry, int *rw, int *rh);
  void guiobject_setGuiPosition(int *x, int *y, int *w, int *h, int *rx, int *ry, int *rw, int *rh);
  void guiobject_setParentGroup(Group *l);
  ScriptObject *guiobject_getScriptObject();
  RootObject *guiobject_getRootObject();
  Group *guiobject_getParentGroup();
  GuiObject *guiobject_getParent();
  Layout *guiobject_getParentLayout();

	int guiobject_wantAutoContextMenu();
 
  void guiobject_setId(const char *id); 
  const char *guiobject_getId(); 

	void guiobject_setTargetX(int tx);
	void guiobject_setTargetY(int ty);
	void guiobject_setTargetW(int tw);
	void guiobject_setTargetH(int th);
	void guiobject_setTargetA(int th);
	void guiobject_setTargetSpeed(float s);
	void guiobject_gotoTarget(void);
  void guiobject_cancelTarget();

	int guiobject_getAutoWidth();
	int guiobject_getAutoHeight();
  int guiobject_movingToTarget();

  void guiobject_bringToFront();
  void guiobject_bringToBack();
  void guiobject_bringAbove(GuiObject *o);
  void guiobject_bringBelow(GuiObject *o);

  void guiobject_setClickThrough(int ct);
  int guiobject_isClickThrough();

  void guiobject_setAutoSysMetricsX(int a);
  void guiobject_setAutoSysMetricsY(int a);
  void guiobject_setAutoSysMetricsW(int a);
  void guiobject_setAutoSysMetricsH(int a);
  int guiobject_getAutoSysMetricsX();
  int guiobject_getAutoSysMetricsY();
  int guiobject_getAutoSysMetricsW();
  int guiobject_getAutoSysMetricsH();

  void guiobject_setRegionOp(int op);
  int guiobject_getRegionOp();
  void guiobject_setRectRgn(int rrgn);
  int guiobject_isRectRgn();

  void guiobject_setMover(int mover);
  int guiobject_getMover();

  FOURCC guiobject_getDropTarget();
  void guiobject_setDropTarget(const char *target);

	void guiobject_onLeftButtonDown(int x, int y);
	void guiobject_onLeftButtonUp(int x, int y);
	void guiobject_onRightButtonDown(int x, int y);
	void guiobject_onRightButtonUp(int x, int y);
	void guiobject_onLeftButtonDblClk(int x, int y);
	void guiobject_onRightButtonDblClk(int x, int y);
	void guiobject_onMouseMove(int x, int y);
	void guiobject_onEnterArea();
	void guiobject_onLeaveArea();
  void guiobject_onCancelCapture();
	void guiobject_onEnable(int en);
	void guiobject_onResize(int x, int y, int w, int h);
  void guiobject_onSetVisible(int v);
  void guiobject_onTargetReached();
	void guiobject_setAlpha(int a);
	int guiobject_getAlpha();
	void guiobject_setActiveAlpha(int a);
	int guiobject_getActiveAlpha();
	void guiobject_setInactiveAlpha(int a);
	int guiobject_getInactiveAlpha();
  void guiobject_onStartup();
  const char *guiobject_getXmlParam(const char *param);
  int guiobject_setXmlParam(const char *param, const char *value);
  int guiobject_setXmlParamById(int id, const char *value);
  int guiobject_runModal();
  void guiobject_endModal(int retcode);
  void guiobject_popParentLayout();
  void guiobject_registerStatusCB(GuiStatusCallback *cb);
  void guiobject_setStatusText(const char *text, int overlay=FALSE);
  void guiobject_addAppCmds(AppCmds *commands);
  void guiobject_removeAppCmds(AppCmds *commands);
  void guiobject_pushCompleted(int max=100);
  void guiobject_incCompleted(int add=1);
  void guiobject_setCompleted(int pos);
  void guiobject_popCompleted();
  void guiobject_onChar(unsigned int c);
  void guiobject_onKeyDown(int vkcode);
  void guiobject_onKeyUp(int vkcode);

  svc_xuiObject *guiobject_getXuiService();
  void guiobject_setXuiService(svc_xuiObject *svc);
  GuiObject *guiobject_findObject(const char *id);
  GuiObject *guiobject_findObjectByInterface(GUID interface_guid);
  GuiObject *guiobject_findObjectByCallback(FindObjectCallback *cb);
  GuiObject *guiobject_findObjectChain(FindObjectCallback *cb, GuiObject *caller=NULL);
  const char *guiobject_getName();

  void guiobject_setCfgAttrib(CfgItem *i, const char *attrib);
  CfgItem *guiobject_getCfgItem();
  const char *guiobject_getCfgAttrib();

  int guiobject_getCfgInt();
  void guiobject_setCfgInt(int i);
  float guiobject_getCfgFloat();
  void guiobject_setCfgFloat(float v);
  const char *guiobject_getCfgString();
  void guiobject_setCfgString(const char *s);
  int guiobject_hasCfgAttrib();

  void guiobject_onGetFocus();
  void guiobject_onKillFocus();
  void guiobject_onAccelerator(const char *accel);

  enum {
    GUI_GETGUIPOSITION=10,
    GUI_SETGUIPOSITION=20,
    GUI_SETPARENTGROUP=30,
    GUI_GETPARENTGROUP=40,
    GUI_GETPARENT=45,
    GUI_GETPARENTLAYOUT=50,
    GUI_GETROOTWND=60,
    GUI_WANTAUTOCTXMENU=70,
    GUI_SETID=80,
    GUI_GETID=90,
    GUI_SETTARGETX=100,
    GUI_SETTARGETY=110,
    GUI_SETTARGETW=120,
    GUI_SETTARGETH=130,
    GUI_SETTARGETA=140,
    GUI_SETTARGETSPEED=150,
    GUI_GOTOTARGET=160,
    GUI_CANCELTARGET=161,
    GUI_MOVINGTOTARGET=170,
    GUI_GETAUTOWIDTH=180,
    GUI_GETAUTOHEIGHT=190,
    GUI_BRINGTOFRONT=200,
    GUI_BRINGTOBACK=210,
    GUI_BRINGBELOW=220,
    GUI_BRINGABOVE=230,
    GUI_SETCLICKTHROUGH=240,
    GUI_ISCLICKTHROUGH=250,
    GUI_SETAUTOSMX=260,
    GUI_SETAUTOSMY=270,
    GUI_SETAUTOSMW=280,
    GUI_SETAUTOSMH=290,
    GUI_GETAUTOSMX=300,
    GUI_GETAUTOSMY=310,
    GUI_GETAUTOSMW=320,
    GUI_GETAUTOSMH=330,
    GUI_ONLEFTBUTTONDOWN=340,
    GUI_ONLEFTBUTTONUP=350,
    GUI_ONRIGHTBUTTONDOWN=360,
    GUI_ONRIGHTBUTTONUP=370,
    GUI_ONLEFTBUTTONDBLCLK=380,
    GUI_ONRIGHTBUTTONDBLCLK=390,
    GUI_ONMOUSEMOVE=400,
    GUI_ONENTERAREA=410,
    GUI_ONLEAVEAREA=420,
    GUI_ONENABLE=430,
    GUI_ONRESIZE=440,
    GUI_ONSETVISIBLE=450,
    GUI_ONTARGETREACHED=460,
    GUI_SETALPHA=470,
    GUI_GETALPHA=480,
    GUI_SETACTIVEALPHA=490,
    GUI_GETACTIVEALPHA=500,
    GUI_SETINACTIVEALPHA=510,
    GUI_GETINACTIVEALPHA=520,
    GUI_ONSTARTUP=530,
    GUI_SETXMLPARAM=540,
    GUI_SETXMLPARAMBYID=541,
    GUI_GETXMLPARAM=542,
//    GUI_ONUNKNOWNXMLPARAM=545, retired
    GUI_SETROOTWND=550,
    GUI_GETSCRIPTOBJECT=560,
    GUI_GETROOTOBJECT=570,
    GUI_RUNMODAL=580,
    GUI_ENDMODAL=590,
    GUI_SETXUISVC=600,
    GUI_GETXUISVC=610,
    GUI_POPPARENTLAYOUT=620,
    GUI_SETREGIONOP=630,
    GUI_GETREGIONOP=640,
    GUI_SETRECTRGN=650,
    GUI_ISRECTRGN=660,
    GUI_REGISTERSTATUSCB=670,
    GUI_SETSTATUSTXT=680,
    GUI_ADDCTXTCMDS=690,
    GUI_REMCTXTCMDS=700,
    GUI_FINDOBJECT=800,
    GUI_FINDOBJECTBYGUID=801,
    GUI_FINDOBJECTBYCB=802,
    GUI_FINDOBJECTCHAIN=803,
    GUI_GETNAME=805,
    GUI_GETMOVER=810,
    GUI_SETMOVER=820,
    GUI_GETDROPTARGET=830,
    GUI_SETDROPTARGET=840,
    GUI_ONCANCELCAPTURE=850,
    GUI_SETCFGATTRIB=860,
    GUI_GETCFGITEM=870,
    GUI_GETCFGATTRIB=880,
    GUI_SETCFGINT=890,
    GUI_GETCFGINT=900,
    GUI_GETCFGFLOAT=910,
    GUI_SETCFGFLOAT=920,
    GUI_SETCFGSTRING=930,
    GUI_GETCFGSTRING=940,
    GUI_HASATTRIB=950,
    GUI_PUSHCOMPLETED=1000,
    GUI_INCCOMPLETED=1010,
    GUI_SETCOMPLETED=1020,
    GUI_POPCOMPLETED=1030,
    GUI_ONCHAR=1100,
    GUI_ONKEYDOWN=1110,
    GUI_ONKEYUP=1120,
    GUI_ONGETFOCUS=1200,
    GUI_ONKILLFOCUS=1210,
    GUI_ONACCELERATOR=1300,
  };
    
  operator ScriptObject *() { return guiobject_getScriptObject(); }
  operator RootWnd *() { return guiobject_getRootWnd(); }
};

inline void GuiObject::guiobject_getGuiPosition(int *x, int *y, int *w, int *h, int *rx, int *ry, int *rw, int *rh) {
  _voidcall(GUI_GETGUIPOSITION, x, y, w, h, rx, ry, rw, rh);
}

inline void GuiObject::guiobject_setGuiPosition(int *x, int *y, int *w, int *h, int *rx, int *ry, int *rw, int *rh) {
  _voidcall(GUI_SETGUIPOSITION, x, y, w, h, rx, ry, rw, rh);
}

inline void GuiObject::guiobject_setParentGroup(Group *g) {
  _voidcall(GUI_SETPARENTGROUP, g);
}

inline Group *GuiObject::guiobject_getParentGroup() {
  return _call(GUI_GETPARENTGROUP, (Group *)NULL);
}

inline GuiObject *GuiObject::guiobject_getParent() {
  return _call(GUI_GETPARENT, (GuiObject *)NULL);
}

inline Layout *GuiObject::guiobject_getParentLayout() {
  return _call(GUI_GETPARENTLAYOUT, (Layout*)NULL);
}

inline RootWnd *GuiObject::guiobject_getRootWnd() {
  return _call(GUI_GETROOTWND, (RootWnd *)NULL);
}

inline void GuiObject::guiobject_setRootWnd(RootWnd *r) {
  _voidcall(GUI_SETROOTWND, r);
}

inline int GuiObject::guiobject_wantAutoContextMenu() {
  return _call(GUI_WANTAUTOCTXMENU, 0);
}

inline void GuiObject::guiobject_setId(const char *id) {
  _voidcall(GUI_SETID, id);
}
 
inline const char *GuiObject::guiobject_getId() {
  return _call(GUI_GETID, (const char *)NULL);
}

inline void GuiObject::guiobject_setTargetX(int tx) {
  _voidcall(GUI_SETTARGETX, tx);
}

inline void GuiObject::guiobject_setTargetY(int ty) {
  _voidcall(GUI_SETTARGETY, ty);
}

inline void GuiObject::guiobject_setTargetW(int tw) {
  _voidcall(GUI_SETTARGETW, tw);
}

inline void GuiObject::guiobject_setTargetH(int th) {
  _voidcall(GUI_SETTARGETH, th);
}

inline void GuiObject::guiobject_setTargetA(int ta) {
  _voidcall(GUI_SETTARGETA, ta);
}

inline void GuiObject::guiobject_setTargetSpeed(float s) {
  _voidcall(GUI_SETTARGETSPEED, s);
}

inline void GuiObject::guiobject_gotoTarget(void) {
  _voidcall(GUI_GOTOTARGET);
}

inline void GuiObject::guiobject_cancelTarget() {
  _voidcall(GUI_CANCELTARGET);
}

inline int GuiObject::guiobject_getAutoWidth() {
  return _call(GUI_GETAUTOWIDTH, 0);
}

inline int GuiObject::guiobject_getAutoHeight() {
  return _call(GUI_GETAUTOHEIGHT, 0);
}

inline int GuiObject::guiobject_movingToTarget() {
  return _call(GUI_MOVINGTOTARGET, 0);
}

inline void GuiObject::guiobject_bringToFront() {
  _voidcall(GUI_BRINGTOFRONT);
}

inline void GuiObject::guiobject_bringToBack() {
  _voidcall(GUI_BRINGTOBACK);
}

inline void GuiObject::guiobject_bringAbove(GuiObject *o) {
  _voidcall(GUI_BRINGABOVE, o);
}

inline void GuiObject::guiobject_bringBelow(GuiObject *o) {
  _voidcall(GUI_BRINGBELOW, o);
}

inline void GuiObject::guiobject_setClickThrough(int ct) {
  _voidcall(GUI_SETCLICKTHROUGH, ct);
}

inline void GuiObject::guiobject_setRegionOp(int op) {
  _voidcall(GUI_SETREGIONOP, op);
}

inline int GuiObject::guiobject_getRegionOp() {
  return _call(GUI_GETREGIONOP, 0);
}

inline void GuiObject::guiobject_setRectRgn(int rrgn) {
  _voidcall(GUI_SETRECTRGN, rrgn);
}

inline int GuiObject::guiobject_isRectRgn() {
  return _call(GUI_ISRECTRGN, 0);
}

inline int GuiObject::guiobject_isClickThrough() {
  return _call(GUI_ISCLICKTHROUGH, 0);
}

inline void GuiObject::guiobject_setAutoSysMetricsX(int a) {
  _voidcall(GUI_SETAUTOSMX, a);
}

inline void GuiObject::guiobject_setAutoSysMetricsY(int a) {
  _voidcall(GUI_SETAUTOSMY, a);
}

inline void GuiObject::guiobject_setAutoSysMetricsW(int a) {
  _voidcall(GUI_SETAUTOSMW, a);
}

inline void GuiObject::guiobject_setAutoSysMetricsH(int a) {
  _voidcall(GUI_SETAUTOSMH, a);
}

inline int GuiObject::guiobject_getAutoSysMetricsX() {
  return _call(GUI_GETAUTOSMX, 0);
}

inline int GuiObject::guiobject_getAutoSysMetricsY() {
  return _call(GUI_GETAUTOSMY, 0);
}

inline int GuiObject::guiobject_getAutoSysMetricsW() {
  return _call(GUI_GETAUTOSMW, 0);
}

inline int GuiObject::guiobject_getAutoSysMetricsH() {
  return _call(GUI_GETAUTOSMH, 0);
}

inline void GuiObject::guiobject_onLeftButtonDown(int x, int y) {
  _voidcall(GUI_ONLEFTBUTTONDOWN, x, y);
}

inline void GuiObject::guiobject_onLeftButtonUp(int x, int y) {
  _voidcall(GUI_ONLEFTBUTTONUP, x, y);
}

inline void GuiObject::guiobject_onRightButtonDown(int x, int y) {
  _voidcall(GUI_ONRIGHTBUTTONDOWN, x, y);
}

inline void GuiObject::guiobject_onRightButtonUp(int x, int y) {
  _voidcall(GUI_ONRIGHTBUTTONUP, x, y);
}

inline void GuiObject::guiobject_onLeftButtonDblClk(int x, int y) {
  _voidcall(GUI_ONLEFTBUTTONDBLCLK, x, y);
}

inline void GuiObject::guiobject_onRightButtonDblClk(int x, int y) {
  _voidcall(GUI_ONRIGHTBUTTONDBLCLK, x, y);
}

inline void GuiObject::guiobject_onMouseMove(int x, int y) {
  _voidcall(GUI_ONMOUSEMOVE, x, y);
}

inline void GuiObject::guiobject_onEnterArea() {
  _voidcall(GUI_ONENTERAREA);
}

inline void GuiObject::guiobject_onLeaveArea() {
  _voidcall(GUI_ONLEAVEAREA);
}

inline void GuiObject::guiobject_onEnable(int en) {
  _voidcall(GUI_ONENABLE, en);
}

inline void GuiObject::guiobject_onResize(int x, int y, int w, int h) {
  _voidcall(GUI_ONRESIZE, x, y, w, h);
}

inline void GuiObject::guiobject_onSetVisible(int v) {
  _voidcall(GUI_ONSETVISIBLE, v);
}

inline void GuiObject::guiobject_onTargetReached() {
  _voidcall(GUI_ONTARGETREACHED);
}

inline void GuiObject::guiobject_setAlpha(int a) {
  _voidcall(GUI_SETALPHA, a);
}

inline int GuiObject::guiobject_getAlpha() {
  return _call(GUI_GETALPHA, 255);
}

inline void GuiObject::guiobject_setActiveAlpha(int a) {
  _voidcall(GUI_SETACTIVEALPHA, a);
}

inline int GuiObject::guiobject_getActiveAlpha() {
  return _call(GUI_GETACTIVEALPHA, 255);
}

inline void GuiObject::guiobject_setInactiveAlpha(int a) {
  _voidcall(GUI_SETINACTIVEALPHA, a);
}

inline int GuiObject::guiobject_getInactiveAlpha() {
  return _call(GUI_GETINACTIVEALPHA, 255);
}

inline void GuiObject::guiobject_onStartup() {
  _voidcall(GUI_ONSTARTUP);
}

inline int GuiObject::guiobject_setXmlParam(const char *param, const char *value) {
  return _call(GUI_SETXMLPARAM, 0, param, value);  
}

inline int GuiObject::guiobject_setXmlParamById(int id, const char *value) {
  return _call(GUI_SETXMLPARAMBYID, 0, id, value);  
}

inline const char *GuiObject::guiobject_getXmlParam(const char *param) {
  return _call(GUI_GETXMLPARAM, (const char *)NULL, param);
}

inline ScriptObject *GuiObject::guiobject_getScriptObject() {
  return _call(GUI_GETSCRIPTOBJECT, (ScriptObject *)NULL);  
}

inline RootObject *GuiObject::guiobject_getRootObject() {
  return _call(GUI_GETROOTOBJECT, (RootObject *)NULL);  
}

inline int GuiObject::guiobject_runModal() {
  return _call(GUI_RUNMODAL, 0);  
}

inline void GuiObject::guiobject_endModal(int retcode) {
  _voidcall(GUI_ENDMODAL, retcode);  
}

inline svc_xuiObject *GuiObject::guiobject_getXuiService() {
  return _call(GUI_GETXUISVC, (svc_xuiObject *)NULL);  
}

inline void GuiObject::guiobject_setXuiService(svc_xuiObject *svc) {
  _voidcall(GUI_SETXUISVC, svc);  
}

inline void GuiObject::guiobject_popParentLayout() {
  _voidcall(GUI_POPPARENTLAYOUT);
}

inline void GuiObject::guiobject_setStatusText(const char *txt, int overlay) {
  _voidcall(GUI_SETSTATUSTXT, txt, overlay);
}

inline void GuiObject::guiobject_addAppCmds(AppCmds *commands) {
  _voidcall(GUI_ADDCTXTCMDS, commands);
}

inline void GuiObject::guiobject_removeAppCmds(AppCmds *commands) {
  _voidcall(GUI_REMCTXTCMDS, commands);
}

inline
void GuiObject::guiobject_pushCompleted(int max) {
  _voidcall(GUI_PUSHCOMPLETED, max);
}
inline
void GuiObject::guiobject_incCompleted(int add) {
  _voidcall(GUI_INCCOMPLETED, add);
}
inline
void GuiObject::guiobject_setCompleted(int pos) {
  _voidcall(GUI_SETCOMPLETED, pos);
}

inline
void GuiObject::guiobject_popCompleted() {
  _voidcall(GUI_POPCOMPLETED);
}

inline void GuiObject::guiobject_registerStatusCB(GuiStatusCallback *callback) {
  _voidcall(GUI_REGISTERSTATUSCB, callback);
}

inline GuiObject *GuiObject::guiobject_findObject(const char *id) {
  return _call(GUI_FINDOBJECT, (GuiObject *)NULL, id);
}

inline GuiObject *GuiObject::guiobject_findObjectByInterface(GUID interface_guid) {
  return _call(GUI_FINDOBJECTBYGUID, (GuiObject *)NULL, interface_guid);
}

inline GuiObject *GuiObject::guiobject_findObjectByCallback(FindObjectCallback *cb) {
  return _call(GUI_FINDOBJECTBYCB, (GuiObject *)NULL, cb);
}

inline GuiObject *GuiObject::guiobject_findObjectChain(FindObjectCallback *cb, GuiObject *caller) {
  return _call(GUI_FINDOBJECTCHAIN, (GuiObject *)NULL, cb, caller);
}

inline const char *GuiObject::guiobject_getName() {
  return _call(GUI_GETNAME, (const char *)NULL);
}

inline int GuiObject::guiobject_getMover() {
  return _call(GUI_GETMOVER, 0);
}

inline void GuiObject::guiobject_setMover(int s) {
  _voidcall(GUI_SETMOVER, s);
}

inline FOURCC GuiObject::guiobject_getDropTarget() {
  return _call(GUI_GETDROPTARGET, (FOURCC)0);
}

inline void GuiObject::guiobject_setDropTarget(const char *target) {
  _voidcall(GUI_SETDROPTARGET, target);
}

inline void GuiObject::guiobject_onCancelCapture() {
  _voidcall(GUI_ONCANCELCAPTURE);
}

inline void GuiObject::guiobject_setCfgAttrib(CfgItem *item, const char *attr) {
  _voidcall(GUI_SETCFGATTRIB, item, attr);
}

inline CfgItem *GuiObject::guiobject_getCfgItem() {
  return _call(GUI_GETCFGITEM, (CfgItem *)NULL);
}

inline const char *GuiObject::guiobject_getCfgAttrib() {
  return _call(GUI_GETCFGATTRIB, (const char *)NULL);
}

inline int GuiObject::guiobject_getCfgInt() {
  return _call(GUI_GETCFGINT, 0);
}

inline void GuiObject::guiobject_setCfgInt(int i) {
  _voidcall(GUI_SETCFGINT, i);
}

inline float GuiObject::guiobject_getCfgFloat() {
  return _call(GUI_GETCFGFLOAT, (float)0);
}

inline void GuiObject::guiobject_setCfgFloat(float v) {
  _voidcall(GUI_SETCFGFLOAT, v);
}

inline const char *GuiObject::guiobject_getCfgString() {
  return _call(GUI_GETCFGSTRING, (const char *)NULL);
}

inline void GuiObject::guiobject_setCfgString(const char *s) {
  _voidcall(GUI_SETCFGSTRING, s);
}

inline int GuiObject::guiobject_hasCfgAttrib() {
  return _call(GUI_HASATTRIB, 0);
}

inline void GuiObject::guiobject_onChar(unsigned int c) {
  _voidcall(GUI_ONCHAR, c);
}

inline void GuiObject::guiobject_onKeyDown(int k) {
  _voidcall(GUI_ONKEYDOWN, k);
}

inline void GuiObject::guiobject_onKeyUp(int k) {
  _voidcall(GUI_ONKEYUP, k);
}

inline void GuiObject::guiobject_onGetFocus() {
  _voidcall(GUI_ONGETFOCUS);
}

inline void GuiObject::guiobject_onKillFocus() {
  _voidcall(GUI_ONKILLFOCUS);
}

inline void GuiObject::guiobject_onAccelerator(const char *accel) {
  _voidcall(GUI_ONACCELERATOR, accel);
}

#endif
