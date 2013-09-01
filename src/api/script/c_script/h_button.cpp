#include "precomp.h"
//#include "../../../studio/api.h"
#include "../objcontroller.h"
#include "h_button.h"

H_Button::H_Button(ScriptObject *o) : H_GuiObject(o) {
  inited=0;
  H_hook(o);
}

H_Button::H_Button() {
  inited=0;
}

void H_Button::H_hook(ScriptObject *o) {
  ASSERT(!inited);
  ScriptObjectController *controller = o->vcpu_getController();
  obj = controller->cast(o, buttonGuid);
  if (obj != o && obj != NULL)
    o = obj;
  else
    obj = NULL;

  addMonitorObject(o, &buttonGuid);
  if (loaded == 0) {/*
    onactivate_id= api->maki_addDlfRef(o->vcpu_getController(), "onActivate", this);
    onleftclick_id= api->maki_addDlfRef(o->vcpu_getController(), "onLeftClick", this);
    onrightclick_id= api->maki_addDlfRef(o->vcpu_getController(), "onRightClick", this);*/
    loaded = 1;
  }
  inited=1;
}

H_Button::~H_Button() {
  if (!inited) return;
  //api->maki_remDlfRef(this);
}

ScriptObject *H_Button::getHookedObject() {
  if (obj != NULL) return obj;
  return H_BUTTON_PARENT::getHookedObject();
}

int H_Button::eventCallback(ScriptObject *object, int dlfid, scriptVar **params, int nparams) {
  if (object != getHookedObject()) return 0;
  if (H_BUTTON_PARENT::eventCallback(object, dlfid, params, nparams)) return 1;/*
  if (dlfid == onactivate_id) { hook_onActivate(GET_SCRIPT_INT(*params[0])); return 1; }
  if (dlfid == onleftclick_id) { hook_onLeftClick(); return 1; }
  if (dlfid == onrightclick_id) { hook_onRightClick(); return 1; }*/
  return 0;
}

int H_Button::onactivate_id=0;
int H_Button::onleftclick_id=0;
int H_Button::onrightclick_id=0;
int H_Button::loaded=0;
