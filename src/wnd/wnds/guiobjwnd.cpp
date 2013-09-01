#include "guiobjwnd.h"
#include "api/script/scriptobj.h"
//#include "api/script/api_script.h"
#include "api/script/scriptguid.h"
#include "api/service/svcs/svc_droptarget.h"
#include "wnd/notifmsg.h"

GuiObjectWnd::GuiObjectWnd() {
  my_gui_object = NULL;//static_cast<GuiObject *>(WASABI_API_SCRIPT->maki_encapsulate(guiObjectGuid, getScriptObject()));
  getScriptObject()->vcpu_setInterface(xmlObjectGuid, static_cast<XmlObject *>(this));
  getScriptObject()->vcpu_setInterface(guiObjectWndGuid, static_cast<GuiObjectWnd *>(this));
  //NULL;//my_gui_object->guiobject_setRootWnd(this);
  getScriptObject()->vcpu_setClassName("GuiObject");
  //getScriptObject()->vcpu_setController(WASABI_API_SCRIPT->maki_getController(guiObjectGuid));
  cfg_reentry = 0;
  xuihandle = newXuiHandle();
  addParam(xuihandle, "id", GUIOBJECT_ID, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "alpha", GUIOBJECT_ALPHA, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "activealpha", GUIOBJECT_ACTIVEALPHA, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "inactivealpha", GUIOBJECT_INACTIVEALPHA, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "sysregion", GUIOBJECT_SYSREGION, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "regionop", GUIOBJECT_SYSREGION, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "rectrgn", GUIOBJECT_RECTRGN, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "tooltip", GUIOBJECT_TOOLTIP, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "sysmetricsx", GUIOBJECT_SYSMETRICSX, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "sysmetricsy", GUIOBJECT_SYSMETRICSY, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "sysmetricsw", GUIOBJECT_SYSMETRICSW, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "sysmetricsh", GUIOBJECT_SYSMETRICSH, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "move", GUIOBJECT_MOVE, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "renderbasetexture", GUIOBJECT_RENDERBASETEXTURE, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "cfgattrib", GUIOBJECT_CFGATTR, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "x", GUIOBJECT_X, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "y", GUIOBJECT_Y, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "w", GUIOBJECT_W, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "h", GUIOBJECT_H, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "visible", GUIOBJECT_VISIBLE, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "relatx", GUIOBJECT_RELATX, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "relaty", GUIOBJECT_RELATY, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "relatw", GUIOBJECT_RELATW, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "relath", GUIOBJECT_RELATH, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "droptarget", GUIOBJECT_DROPTARGET, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "ghost", GUIOBJECT_GHOST, XUI_ATTRIBUTE_IMPLIED);
  addParam(xuihandle, "notify", GUIOBJECT_NOTIFY, XUI_ATTRIBUTE_IMPLIED);
  for (int i=0;i<10;i++)
    addParam(xuihandle, StringPrintf("notify%d", i), GUIOBJECT_NOTIFY, XUI_ATTRIBUTE_IMPLIED);
}

GuiObjectWnd::~GuiObjectWnd() {
  //WASABI_API_SCRIPT->maki_deencapsulate(guiObjectGuid, my_gui_object);
}

int GuiObjectWnd::onRightButtonDown(int x, int y) {
  GUIOBJECTWND_PARENT::onRightButtonDown(x, y);
  //my_gui_object->guiobject_onRightButtonDown(x, y);
  return 1;
}

int GuiObjectWnd::onRightButtonUp(int x, int y) {
  GUIOBJECTWND_PARENT::onRightButtonUp(x, y);
  //my_gui_object->guiobject_onRightButtonUp(x, y);
  return 1;
}

int GuiObjectWnd::onLeftButtonDown(int x, int y) {
  GUIOBJECTWND_PARENT::onLeftButtonDown(x, y);
  //my_gui_object->guiobject_onLeftButtonDown(x, y);
  return 1;
}

int GuiObjectWnd::onLeftButtonUp(int x, int y) {
  GUIOBJECTWND_PARENT::onLeftButtonUp(x, y);
  //my_gui_object->guiobject_onLeftButtonUp(x, y);
  return 1;
}

int GuiObjectWnd::onMouseMove(int x, int y) {
  GUIOBJECTWND_PARENT::onMouseMove(x, y);
  //my_gui_object->guiobject_onMouseMove(x, y);
  return 1;
}

int GuiObjectWnd::onLeftButtonDblClk(int x, int y) {
  GUIOBJECTWND_PARENT::onLeftButtonDblClk(x, y);
  //my_gui_object->guiobject_onLeftButtonDblClk(x, y);
  return 1;
}

int GuiObjectWnd::onRightButtonDblClk(int x, int y) {
  GUIOBJECTWND_PARENT::onRightButtonDblClk(x, y);
  //my_gui_object->guiobject_onRightButtonDblClk(x, y);
  return 1;
}

int GuiObjectWnd::onResize() {
  GUIOBJECTWND_PARENT::onResize();
  if (!isInited()) return 1;
  RootWnd *w = NULL;//my_gui_object->guiobject_getRootWnd();
  RECT r;
  //w->getClientRect(&r);
  //my_gui_object->guiobject_onResize(r.left, r.top, r.right-r.left, r.bottom-r.top);
  return 1;
}

void GuiObjectWnd::onEnterArea() {
  GUIOBJECTWND_PARENT::onEnterArea();
  //my_gui_object->guiobject_onEnterArea();
}

void GuiObjectWnd::onLeaveArea() {
  GUIOBJECTWND_PARENT::onLeaveArea();
  //my_gui_object->guiobject_onLeaveArea();
}

void GuiObjectWnd::onSetVisible(int show) {
  GUIOBJECTWND_PARENT::onSetVisible(show);
  //my_gui_object->guiobject_onSetVisible(show);
}

GuiObject *GuiObjectWnd::getGuiObject() {
  return my_gui_object;
}

RootObject *GuiObjectWnd::getRootObject() {
  return NULL;//my_gui_object->guiobject_getRootObject();
}

int GuiObjectWnd::dragDrop(RootWnd *sourceWnd, int x, int y) {
  int r=0;//DropTargetEnum::throwDrop(my_gui_object->guiobject_getDropTarget(), sourceWnd, x, y);
  if (r == 0) {
    RootWnd *p=getParent();
    if (p != NULL) {
      DragInterface *d=p->getDragInterface();
      if (d != NULL) 
        return d->dragDrop(sourceWnd, x, y);
    }
  }
  return r;
}

int GuiObjectWnd::onActivate() {
  GUIOBJECTWND_PARENT::onActivate();
  invalidate();
  return 1;
}

int GuiObjectWnd::onDeactivate() {
  GUIOBJECTWND_PARENT::onDeactivate();
  invalidate();
  return 1;
}

void GuiObjectWnd::onCancelCapture() {
  GUIOBJECTWND_PARENT::onCancelCapture();
  //my_gui_object->guiobject_onCancelCapture();
}

int GuiObjectWnd::setXuiParam(int _xuihandle, int attrid, const char *name, const char *value) {
  //if (_xuihandle == xuihandle) getGuiObject()->guiobject_setXmlParamById(attrid, value);
  return 0;
}

int GuiObjectWnd::onUnknownXmlParam(const char *param, const char *value) {
  return onUnknownXuiParam(param, value);
}

int GuiObjectWnd::setXmlParamById(int xmlhandle, int attrid, const char *name, const char *value) {
  return setXuiParam(xmlhandle, attrid, name, value);
}

void *GuiObjectWnd::getInterface(GUID interface_guid) {
  void *r = GUIOBJECTWND_PARENT::getInterface(interface_guid);
  if (r) return r;
  return getRootObject()->rootobject_getScriptObject()->vcpu_getInterface(interface_guid);
}

int GuiObjectWnd::onAction(const char *action, const char *param, int x, int y, int p1, int p2, void *data, int datalen, RootWnd *source) {
  if (STRCASEEQL(action, "reload_config") && isInited()) {
    if (cfg_reentry) return 1;
    cfg_reentry = 1;
    int r = onReloadConfig();
    cfg_reentry = 0;
    return r;
  } 
  int rt = GUIOBJECTWND_PARENT::onAction(action, param, x, y, p1, p2, data, datalen, source);
  return rt;
}

void GuiObjectWnd::setContent(const char *groupid_orguid, int autoresizefromcontent) {
//  abstract_setAllowDeferredContent(0);
  //abstract_setContent(groupid_orguid, autoresizefromcontent);
}

void GuiObjectWnd::abstract_onNewContent() {
//  GUIOBJECTWND_PARENT::abstract_onNewContent();
  onNewContent();
  //if (getGuiObject()->guiobject_hasCfgAttrib())
  //  onReloadConfig();
}

//#include "profiler.h"
GuiObject *GuiObjectWnd::findObject(const char *object_id) {
//  PR_ENTER(StringPrintf("findObject %s", object_id));
  return NULL;//getGuiObject()->guiobject_findObject(object_id);
//  PR_LEAVE();
}

ScriptObject *GuiObjectWnd::findScriptObject(const char *object_id) {
  //GuiObject *fo = getGuiObject()->guiobject_findObject(object_id);
  //if (fo != NULL) return fo->guiobject_getScriptObject();
  return NULL;
}

const char *GuiObjectWnd::getId() {
  if (my_gui_object)
    return NULL;//my_gui_object->guiobject_getId();
  return GUIOBJECTWND_PARENT::getId();
}

int GuiObjectWnd::onPostOnInit() {
  int r = GUIOBJECTWND_PARENT::onPostOnInit();
  //if (getGuiObject()->guiobject_hasCfgAttrib())
  //  onReloadConfig();
  return r;
}

int GuiObjectWnd::onChar(unsigned int c) {
  int r = GUIOBJECTWND_PARENT::onChar(c);
  //getGuiObject()->guiobject_onChar(c);
  return r;
}

int GuiObjectWnd::onKeyDown(int vkcode) {
  int r = GUIOBJECTWND_PARENT::onKeyDown(vkcode);
  //getGuiObject()->guiobject_onKeyDown(vkcode);
  return r;
}

int GuiObjectWnd::onKeyUp(int vkcode) {
  int r = GUIOBJECTWND_PARENT::onKeyUp(vkcode);
  //getGuiObject()->guiobject_onKeyUp(vkcode);
  return r;
}

int GuiObjectWnd::onGetFocus() {
  int r = GUIOBJECTWND_PARENT::onGetFocus();
  //getGuiObject()->guiobject_onGetFocus();
  return r;
}

int GuiObjectWnd::onKillFocus() {
  int r = GUIOBJECTWND_PARENT::onKillFocus();
  //getGuiObject()->guiobject_onKillFocus();
  return r;
}

int GuiObjectWnd::onAcceleratorEvent(const char *name) {
  int r = GUIOBJECTWND_PARENT::onAcceleratorEvent(name);
  //getGuiObject()->guiobject_onAccelerator(name);
  return r;
}
