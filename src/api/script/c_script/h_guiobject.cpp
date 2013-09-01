#include "precomp.h"
//#include "../../../studio/api.h"
#include "../objcontroller.h"
#include "h_guiobject.h"

H_GuiObject::H_GuiObject(ScriptObject *o) : H_RootObject(o) {
  inited=0;
  H_hook(o);
}

H_GuiObject::H_GuiObject() {
  inited=0;
}

void H_GuiObject::H_hook(ScriptObject *o) {
  ASSERT(!inited);
  ScriptObjectController *controller = o->vcpu_getController();
  obj = controller->cast(o, guiObjectGuid);
  if (obj != o && obj != NULL)
    o = obj;
  else
    obj = NULL;

  addMonitorObject(o, &guiObjectGuid);
  if (loaded == 0) { 
    /*
    onsetvisible_id= api->maki_addDlfRef(o->vcpu_getController(), "onSetVisible", this);
    onleftbuttonup_id= api->maki_addDlfRef(o->vcpu_getController(), "onLeftButtonUp", this);
    onleftbuttondown_id= api->maki_addDlfRef(o->vcpu_getController(), "onLeftButtonDown", this);
    onrightbuttonup_id= api->maki_addDlfRef(o->vcpu_getController(), "onRightButtonUp", this);
    onrightbuttondown_id= api->maki_addDlfRef(o->vcpu_getController(), "onRightButtonDown", this);
    onrightbuttondblclk_id= api->maki_addDlfRef(o->vcpu_getController(), "onRightButtonDblClk", this);
    onleftbuttondblclk_id= api->maki_addDlfRef(o->vcpu_getController(), "onLeftButtonDblClk", this);
    onmousemove_id= api->maki_addDlfRef(o->vcpu_getController(), "onMouseMove", this);
    onenterarea_id= api->maki_addDlfRef(o->vcpu_getController(), "onEnterArea", this);
    onleavearea_id= api->maki_addDlfRef(o->vcpu_getController(), "onLeaveArea", this);
    onenable_id= api->maki_addDlfRef(o->vcpu_getController(), "onEnable", this);
    onresize_id= api->maki_addDlfRef(o->vcpu_getController(), "onResize", this);
    ontargetreached_id= api->maki_addDlfRef(o->vcpu_getController(), "onTargetReached", this);
    onchar_id= api->maki_addDlfRef(o->vcpu_getController(), "onChar", this);
    onkeydown_id= api->maki_addDlfRef(o->vcpu_getController(), "onKeyDown", this);
    onkeyup_id= api->maki_addDlfRef(o->vcpu_getController(), "onKeyUp", this);*/
    loaded = 1;
  }
  inited=1;
}

H_GuiObject::~H_GuiObject() {
  if (!inited) return;
  //api->maki_remDlfRef(this);
}

ScriptObject *H_GuiObject::getHookedObject() {
  if (obj != NULL) return obj;
  return H_GUIOBJECT_PARENT::getHookedObject();
}

int H_GuiObject::eventCallback(ScriptObject *object, int dlfid, scriptVar **params, int nparams) {
  if (object != getHookedObject()) return 0;
  if (H_GUIOBJECT_PARENT::eventCallback(object, dlfid, params, nparams)) return 1;/*
  if (dlfid == onsetvisible_id) { hook_onSetVisible(GET_SCRIPT_INT(*params[0])); return 1; }
  if (dlfid == onleftbuttonup_id) { hook_onLeftButtonUp(GET_SCRIPT_INT(*params[0]), GET_SCRIPT_INT(*params[1])); return 1; }
  if (dlfid == onleftbuttondown_id) { hook_onLeftButtonDown(GET_SCRIPT_INT(*params[0]), GET_SCRIPT_INT(*params[1])); return 1; }
  if (dlfid == onrightbuttonup_id) { hook_onRightButtonUp(GET_SCRIPT_INT(*params[0]), GET_SCRIPT_INT(*params[1])); return 1; }
  if (dlfid == onrightbuttondown_id) { hook_onRightButtonDown(GET_SCRIPT_INT(*params[0]), GET_SCRIPT_INT(*params[1])); return 1; }
  if (dlfid == onrightbuttondblclk_id) { hook_onRightButtonDblClk(GET_SCRIPT_INT(*params[0]), GET_SCRIPT_INT(*params[1])); return 1; }
  if (dlfid == onleftbuttondblclk_id) { hook_onLeftButtonDblClk(GET_SCRIPT_INT(*params[0]), GET_SCRIPT_INT(*params[1])); return 1; }
  if (dlfid == onmousemove_id) { hook_onMouseMove(GET_SCRIPT_INT(*params[0]), GET_SCRIPT_INT(*params[1])); return 1; }
  if (dlfid == onenterarea_id) { hook_onEnterArea(); return 1; }
  if (dlfid == onleavearea_id) { hook_onLeaveArea(); return 1; }
  if (dlfid == onenable_id) { hook_onEnable(GET_SCRIPT_INT(*params[0])); return 1; }
  if (dlfid == onresize_id) { hook_onResize(GET_SCRIPT_INT(*params[0]), GET_SCRIPT_INT(*params[1]), GET_SCRIPT_INT(*params[2]), GET_SCRIPT_INT(*params[3])); return 1; }
  if (dlfid == ontargetreached_id) { hook_onTargetReached(); return 1; }
  if (dlfid == onchar_id) { hook_onChar(GET_SCRIPT_STRING(*params[0])); return 1; }
  if (dlfid == onkeydown_id) { hook_onKeyDown(GET_SCRIPT_INT(*params[0])); return 1; }
  if (dlfid == onkeyup_id) { hook_onKeyUp(GET_SCRIPT_INT(*params[0])); return 1; }*/
  return 0;
}

int H_GuiObject::onsetvisible_id=0;
int H_GuiObject::onleftbuttonup_id=0;
int H_GuiObject::onleftbuttondown_id=0;
int H_GuiObject::onrightbuttonup_id=0;
int H_GuiObject::onrightbuttondown_id=0;
int H_GuiObject::onrightbuttondblclk_id=0;
int H_GuiObject::onleftbuttondblclk_id=0;
int H_GuiObject::onmousemove_id=0;
int H_GuiObject::onenterarea_id=0;
int H_GuiObject::onleavearea_id=0;
int H_GuiObject::onenable_id=0;
int H_GuiObject::onresize_id=0;
int H_GuiObject::ontargetreached_id=0;
int H_GuiObject::onchar_id=0;
int H_GuiObject::onkeydown_id=0;
int H_GuiObject::onkeyup_id=0;
int H_GuiObject::loaded=0;
