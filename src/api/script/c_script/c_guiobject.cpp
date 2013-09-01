#include "precomp.h"
//#include "../../../studio/api.h"
#include "c_guiobject.h"
#include "../objcontroller.h"

C_GuiObject::C_GuiObject(ScriptObject *object) : C_RootObject(object) {
  inited = 0;
  C_hook(object);
}

C_GuiObject::C_GuiObject() {
  inited = 0;
}

void C_GuiObject::C_hook(ScriptObject *object) {
  ASSERT(!inited);
  ScriptObjectController *controller = object->vcpu_getController();
  obj = controller->cast(object, guiObjectGuid);
  if (obj != object && obj != NULL)
    controller = obj->vcpu_getController();
  else
    obj = NULL;

  if (!loaded) {
    loaded = 1;/*
    show_id = api->maki_addDlfRef(controller, "show", this);
    hide_id = api->maki_addDlfRef(controller, "hide", this);
    isvisible_id = api->maki_addDlfRef(controller, "isVisible", this);
    onsetvisible_id = api->maki_addDlfRef(controller, "onSetVisible", this);
    setalpha_id = api->maki_addDlfRef(controller, "setAlpha", this);
    getalpha_id = api->maki_addDlfRef(controller, "getAlpha", this);
    onleftbuttonup_id = api->maki_addDlfRef(controller, "onLeftButtonUp", this);
    onleftbuttondown_id = api->maki_addDlfRef(controller, "onLeftButtonDown", this);
    onrightbuttonup_id = api->maki_addDlfRef(controller, "onRightButtonUp", this);
    onrightbuttondown_id = api->maki_addDlfRef(controller, "onRightButtonDown", this);
    onrightbuttondblclk_id = api->maki_addDlfRef(controller, "onRightButtonDblClk", this);
    onleftbuttondblclk_id = api->maki_addDlfRef(controller, "onLeftButtonDblClk", this);
    onmousemove_id = api->maki_addDlfRef(controller, "onMouseMove", this);
    onenterarea_id = api->maki_addDlfRef(controller, "onEnterArea", this);
    onleavearea_id = api->maki_addDlfRef(controller, "onLeaveArea", this);
    setenabled_id = api->maki_addDlfRef(controller, "setEnabled", this);
    getenabled_id = api->maki_addDlfRef(controller, "getEnabled", this);
    onenable_id = api->maki_addDlfRef(controller, "onEnable", this);
    resize_id = api->maki_addDlfRef(controller, "resize", this);
    onresize_id = api->maki_addDlfRef(controller, "onResize", this);
    ismouseover_id = api->maki_addDlfRef(controller, "isMouseOver", this);
    getleft_id = api->maki_addDlfRef(controller, "getLeft", this);
    gettop_id = api->maki_addDlfRef(controller, "getTop", this);
    getwidth_id = api->maki_addDlfRef(controller, "getWidth", this);
    getheight_id = api->maki_addDlfRef(controller, "getHeight", this);
    settargetx_id = api->maki_addDlfRef(controller, "setTargetX", this);
    settargety_id = api->maki_addDlfRef(controller, "setTargetY", this);
    settargetw_id = api->maki_addDlfRef(controller, "setTargetW", this);
    settargeth_id = api->maki_addDlfRef(controller, "setTargetH", this);
    settargeta_id = api->maki_addDlfRef(controller, "setTargetA", this);
    settargetspeed_id = api->maki_addDlfRef(controller, "setTargetSpeed", this);
    gototarget_id = api->maki_addDlfRef(controller, "gotoTarget", this);
    ontargetreached_id = api->maki_addDlfRef(controller, "onTargetReached", this);
    canceltarget_id = api->maki_addDlfRef(controller, "cancelTarget", this);
    isgoingtotarget_id = api->maki_addDlfRef(controller, "isGoingToTarget", this);
    setxmlparam_id = api->maki_addDlfRef(controller, "setXmlParam", this);
    init_id = api->maki_addDlfRef(controller, "init", this);
    bringtofront_id = api->maki_addDlfRef(controller, "bringToFront", this);
    bringtoback_id = api->maki_addDlfRef(controller, "bringToBack", this);
    bringabove_id = api->maki_addDlfRef(controller, "bringAbove", this);
    bringbelow_id = api->maki_addDlfRef(controller, "bringBelow", this);
    getguix_id = api->maki_addDlfRef(controller, "getGuiX", this);
    getguiy_id = api->maki_addDlfRef(controller, "getGuiY", this);
    getguiw_id = api->maki_addDlfRef(controller, "getGuiW", this);
    getguih_id = api->maki_addDlfRef(controller, "getGuiH", this);
    getguirelatx_id = api->maki_addDlfRef(controller, "getGuiRelatX", this);
    getguirelaty_id = api->maki_addDlfRef(controller, "getGuiRelatY", this);
    getguirelatw_id = api->maki_addDlfRef(controller, "getGuiRelatW", this);
    getguirelath_id = api->maki_addDlfRef(controller, "getGuiRelatH", this);
    isactive_id = api->maki_addDlfRef(controller, "isActive", this);
    getparent_id = api->maki_addDlfRef(controller, "getParent", this);
    getparentlayout_id = api->maki_addDlfRef(controller, "getParentLayout", this);
    runmodal_id = api->maki_addDlfRef(controller, "runModal", this);
    endmodal_id = api->maki_addDlfRef(controller, "endModal", this);
    findobject_id = api->maki_addDlfRef(controller, "findObject", this);
    getname_id = api->maki_addDlfRef(controller, "getName", this);
    clienttoscreenx_id = api->maki_addDlfRef(controller, "clientToScreenX", this);
    clienttoscreeny_id = api->maki_addDlfRef(controller, "clientToScreenY", this);
    screentoclientx_id = api->maki_addDlfRef(controller, "screenToClientX", this);
    screentoclienty_id = api->maki_addDlfRef(controller, "screenToClientY", this);
    getautowidth_id = api->maki_addDlfRef(controller, "getAutoWidth", this);
    getautoheight_id = api->maki_addDlfRef(controller, "getAutoHeight", this);
    setfocus_id = api->maki_addDlfRef(controller, "setFocus", this);
    onchar_id = api->maki_addDlfRef(controller, "onChar", this);
    onkeydown_id = api->maki_addDlfRef(controller, "onKeyDown", this);
    onkeyup_id = api->maki_addDlfRef(controller, "onKeyUp", this);*/
  }
  inited = 1;
}

C_GuiObject::~C_GuiObject() {
}

ScriptObject *C_GuiObject::getScriptObject() {
  if (obj != NULL) return obj;
  return C_GUIOBJECT_PARENT::getScriptObject();
}

void C_GuiObject::show() {
  ASSERT(inited);
  //api->maki_callFunction(getScriptObject(), show_id, NULL);
}

void C_GuiObject::hide() {
  ASSERT(inited);
  //api->maki_callFunction(getScriptObject(), hide_id, NULL);
}

int C_GuiObject::isVisible() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), isvisible_id, NULL));
}

void C_GuiObject::onSetVisible(int onoff) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(onoff);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), onsetvisible_id, params);
}

void C_GuiObject::setAlpha(int alpha) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(alpha);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), setalpha_id, params);
}

int C_GuiObject::getAlpha() {
  ASSERT(inited);
  return 255;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getalpha_id, NULL));
}

void C_GuiObject::onLeftButtonUp(int x, int y) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(x);
  scriptVar b = MAKE_SCRIPT_INT(y);
  scriptVar *params[2] = {&a, &b};
  //api->maki_callFunction(getScriptObject(), onleftbuttonup_id, params);
}

void C_GuiObject::onLeftButtonDown(int x, int y) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(x);
  scriptVar b = MAKE_SCRIPT_INT(y);
  scriptVar *params[2] = {&a, &b};
  //api->maki_callFunction(getScriptObject(), onleftbuttondown_id, params);
}

void C_GuiObject::onRightButtonUp(int x, int y) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(x);
  scriptVar b = MAKE_SCRIPT_INT(y);
  scriptVar *params[2] = {&a, &b};
  //api->maki_callFunction(getScriptObject(), onrightbuttonup_id, params);
}

void C_GuiObject::onRightButtonDown(int x, int y) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(x);
  scriptVar b = MAKE_SCRIPT_INT(y);
  scriptVar *params[2] = {&a, &b};
  //api->maki_callFunction(getScriptObject(), onrightbuttondown_id, params);
}

void C_GuiObject::onRightButtonDblClk(int x, int y) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(x);
  scriptVar b = MAKE_SCRIPT_INT(y);
  scriptVar *params[2] = {&a, &b};
  //api->maki_callFunction(getScriptObject(), onrightbuttondblclk_id, params);
}

void C_GuiObject::onLeftButtonDblClk(int x, int y) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(x);
  scriptVar b = MAKE_SCRIPT_INT(y);
  scriptVar *params[2] = {&a, &b};
  //api->maki_callFunction(getScriptObject(), onleftbuttondblclk_id, params);
}

void C_GuiObject::onMouseMove(int x, int y) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(x);
  scriptVar b = MAKE_SCRIPT_INT(y);
  scriptVar *params[2] = {&a, &b};
  //api->maki_callFunction(getScriptObject(), onmousemove_id, params);
}

void C_GuiObject::onEnterArea() {
  ASSERT(inited);
  //api->maki_callFunction(getScriptObject(), onenterarea_id, NULL);
}

void C_GuiObject::onLeaveArea() {
  ASSERT(inited);
  //api->maki_callFunction(getScriptObject(), onleavearea_id, NULL);
}

void C_GuiObject::setEnabled(int onoff) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(onoff);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), setenabled_id, params);
}

int C_GuiObject::getEnabled() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getenabled_id, NULL));
}

void C_GuiObject::onEnable(int onoff) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(onoff);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), onenable_id, params);
}

void C_GuiObject::resize(int x, int y, int w, int h) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(x);
  scriptVar b = MAKE_SCRIPT_INT(y);
  scriptVar c = MAKE_SCRIPT_INT(w);
  scriptVar d = MAKE_SCRIPT_INT(h);
  scriptVar *params[4] = {&a, &b, &c, &d};
  //api->maki_callFunction(getScriptObject(), resize_id, params);
}

void C_GuiObject::onResize(int x, int y, int w, int h) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(x);
  scriptVar b = MAKE_SCRIPT_INT(y);
  scriptVar c = MAKE_SCRIPT_INT(w);
  scriptVar d = MAKE_SCRIPT_INT(h);
  scriptVar *params[4] = {&a, &b, &c, &d};
  //api->maki_callFunction(getScriptObject(), onresize_id, params);
}

int C_GuiObject::isMouseOver(int x, int y) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(x);
  scriptVar b = MAKE_SCRIPT_INT(y);
  scriptVar *params[2] = {&a, &b};
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), ismouseover_id, params));
}

int C_GuiObject::getLeft() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getleft_id, NULL));
}

int C_GuiObject::getTop() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), gettop_id, NULL));
}

int C_GuiObject::getWidth() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getwidth_id, NULL));
}

int C_GuiObject::getHeight() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getheight_id, NULL));
}

void C_GuiObject::setTargetX(int x) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(x);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), settargetx_id, params);
}

void C_GuiObject::setTargetY(int y) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(y);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), settargety_id, params);
}

void C_GuiObject::setTargetW(int w) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(w);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), settargetw_id, params);
}

void C_GuiObject::setTargetH(int r) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(r);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), settargeth_id, params);
}

void C_GuiObject::setTargetA(int alpha) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(alpha);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), settargeta_id, params);
}

void C_GuiObject::setTargetSpeed(float insecond) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_FLOAT(insecond);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), settargetspeed_id, params);
}

void C_GuiObject::gotoTarget() {
  ASSERT(inited);
  //api->maki_callFunction(getScriptObject(), gototarget_id, NULL);
}

void C_GuiObject::onTargetReached() {
  ASSERT(inited);
  //api->maki_callFunction(getScriptObject(), ontargetreached_id, NULL);
}

void C_GuiObject::cancelTarget() {
  ASSERT(inited);
  //api->maki_callFunction(getScriptObject(), canceltarget_id, NULL);
}

int C_GuiObject::isGoingToTarget() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), isgoingtotarget_id, NULL));
}

void C_GuiObject::setXmlParam(const char *param, const char *value) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_STRING(param);
  scriptVar b = MAKE_SCRIPT_STRING(value);
  scriptVar *params[2] = {&a, &b};
  //api->maki_callFunction(getScriptObject(), setxmlparam_id, params);
}

void C_GuiObject::init(ScriptObject *parent) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_OBJECT(parent);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), init_id, params);
}

void C_GuiObject::bringToFront() {
  ASSERT(inited);
  //api->maki_callFunction(getScriptObject(), bringtofront_id, NULL);
}

void C_GuiObject::bringToBack() {
  ASSERT(inited);
  //api->maki_callFunction(getScriptObject(), bringtoback_id, NULL);
}

void C_GuiObject::bringAbove(ScriptObject *guiobj) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_OBJECT(guiobj);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), bringabove_id, params);
}

void C_GuiObject::bringBelow(ScriptObject *guiobj) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_OBJECT(guiobj);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), bringbelow_id, params);
}

int C_GuiObject::getGuiX() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getguix_id, NULL));
}

int C_GuiObject::getGuiY() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getguiy_id, NULL));
}

int C_GuiObject::getGuiW() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getguiw_id, NULL));
}

int C_GuiObject::getGuiH() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getguih_id, NULL));
}

int C_GuiObject::getGuiRelatX() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getguirelatx_id, NULL));
}

int C_GuiObject::getGuiRelatY() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getguirelaty_id, NULL));
}

int C_GuiObject::getGuiRelatW() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getguirelatw_id, NULL));
}

int C_GuiObject::getGuiRelatH() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getguirelath_id, NULL));
}

int C_GuiObject::isActive() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), isactive_id, NULL));
}

ScriptObject *C_GuiObject::getParent() {
  ASSERT(inited);
  return NULL;//GET_SCRIPT_OBJECT(api->maki_callFunction(getScriptObject(), getparent_id, NULL));
}

ScriptObject *C_GuiObject::getParentLayout() {
  ASSERT(inited);
  return NULL;//GET_SCRIPT_OBJECT(api->maki_callFunction(getScriptObject(), getparentlayout_id, NULL));
}

int C_GuiObject::runModal() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), runmodal_id, NULL));
}

void C_GuiObject::endModal(int retcode) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(retcode);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), endmodal_id, params);
}

ScriptObject *C_GuiObject::findObject(const char *id) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_STRING(id);
  scriptVar *params[1] = {&a};
  return NULL;//GET_SCRIPT_OBJECT(api->maki_callFunction(getScriptObject(), findobject_id, params));
}

const char *C_GuiObject::getName() {
  ASSERT(inited);
  return NULL;//GET_SCRIPT_STRING(api->maki_callFunction(getScriptObject(), getname_id, NULL));
}

int C_GuiObject::clientToScreenX(int x) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(x);
  scriptVar *params[1] = {&a};
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), clienttoscreenx_id, params));
}

int C_GuiObject::clientToScreenY(int y) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(y);
  scriptVar *params[1] = {&a};
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), clienttoscreeny_id, params));
}

int C_GuiObject::screenToClientX(int x) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(x);
  scriptVar *params[1] = {&a};
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), screentoclientx_id, params));
}

int C_GuiObject::screenToClientY(int y) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(y);
  scriptVar *params[1] = {&a};
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), screentoclienty_id, params));
}

int C_GuiObject::getAutoWidth() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getautowidth_id, NULL));
}

int C_GuiObject::getAutoHeight() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getautoheight_id, NULL));
}

void C_GuiObject::setFocus() {
  ASSERT(inited);
  //api->maki_callFunction(getScriptObject(), setfocus_id, NULL);
}

void C_GuiObject::onChar(const char *c) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_STRING(c);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), onchar_id, params);
}

void C_GuiObject::onKeyDown(int vk_code) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(vk_code);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), onkeydown_id, params);
}

void C_GuiObject::onKeyUp(int vk_code) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(vk_code);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), onkeyup_id, params);
}

int C_GuiObject::loaded=0;
int C_GuiObject::show_id=0;
int C_GuiObject::hide_id=0;
int C_GuiObject::isvisible_id=0;
int C_GuiObject::onsetvisible_id=0;
int C_GuiObject::setalpha_id=0;
int C_GuiObject::getalpha_id=0;
int C_GuiObject::onleftbuttonup_id=0;
int C_GuiObject::onleftbuttondown_id=0;
int C_GuiObject::onrightbuttonup_id=0;
int C_GuiObject::onrightbuttondown_id=0;
int C_GuiObject::onrightbuttondblclk_id=0;
int C_GuiObject::onleftbuttondblclk_id=0;
int C_GuiObject::onmousemove_id=0;
int C_GuiObject::onenterarea_id=0;
int C_GuiObject::onleavearea_id=0;
int C_GuiObject::setenabled_id=0;
int C_GuiObject::getenabled_id=0;
int C_GuiObject::onenable_id=0;
int C_GuiObject::resize_id=0;
int C_GuiObject::onresize_id=0;
int C_GuiObject::ismouseover_id=0;
int C_GuiObject::getleft_id=0;
int C_GuiObject::gettop_id=0;
int C_GuiObject::getwidth_id=0;
int C_GuiObject::getheight_id=0;
int C_GuiObject::settargetx_id=0;
int C_GuiObject::settargety_id=0;
int C_GuiObject::settargetw_id=0;
int C_GuiObject::settargeth_id=0;
int C_GuiObject::settargeta_id=0;
int C_GuiObject::settargetspeed_id=0;
int C_GuiObject::gototarget_id=0;
int C_GuiObject::ontargetreached_id=0;
int C_GuiObject::canceltarget_id=0;
int C_GuiObject::isgoingtotarget_id=0;
int C_GuiObject::setxmlparam_id=0;
int C_GuiObject::init_id=0;
int C_GuiObject::bringtofront_id=0;
int C_GuiObject::bringtoback_id=0;
int C_GuiObject::bringabove_id=0;
int C_GuiObject::bringbelow_id=0;
int C_GuiObject::getguix_id=0;
int C_GuiObject::getguiy_id=0;
int C_GuiObject::getguiw_id=0;
int C_GuiObject::getguih_id=0;
int C_GuiObject::getguirelatx_id=0;
int C_GuiObject::getguirelaty_id=0;
int C_GuiObject::getguirelatw_id=0;
int C_GuiObject::getguirelath_id=0;
int C_GuiObject::isactive_id=0;
int C_GuiObject::getparent_id=0;
int C_GuiObject::getparentlayout_id=0;
int C_GuiObject::runmodal_id=0;
int C_GuiObject::endmodal_id=0;
int C_GuiObject::findobject_id=0;
int C_GuiObject::getname_id=0;
int C_GuiObject::clienttoscreenx_id=0;
int C_GuiObject::clienttoscreeny_id=0;
int C_GuiObject::screentoclientx_id=0;
int C_GuiObject::screentoclienty_id=0;
int C_GuiObject::getautowidth_id=0;
int C_GuiObject::getautoheight_id=0;
int C_GuiObject::setfocus_id=0;
int C_GuiObject::onchar_id=0;
int C_GuiObject::onkeydown_id=0;
int C_GuiObject::onkeyup_id=0;
