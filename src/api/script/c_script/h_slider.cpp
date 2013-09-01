/*

  Nullsoft WASABI Source File License

  Copyright 1999-2001 Nullsoft, Inc.

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.


  Brennan Underwood
  brennan@nullsoft.com

*/

/* This file was generated by Maki Compiler, do not edit manually */

#include "../../../studio/api.h"
#include "../objcontroller.h"
#include "h_slider.h"

H_Slider::H_Slider(ScriptObject *o) : H_GuiObject(o) {
  inited=0;
  H_hook(o);
}

H_Slider::H_Slider() {
  inited=0;
}

void H_Slider::H_hook(ScriptObject *o) {
  ASSERT(!inited);
  ScriptObjectController *controller = o->vcpu_getController();
  obj = controller->cast(o, sliderGuid);
  if (obj != o)
    o = obj;
  else
    obj = NULL;

  addMonitorObject(o, &sliderGuid);
  if (loaded == 0) {
    onsetposition_id= api->maki_addDlfRef(o->vcpu_getController(), "onSetPosition", this);
    onpostedposition_id= api->maki_addDlfRef(o->vcpu_getController(), "onPostedPosition", this);
    onsetfinalposition_id= api->maki_addDlfRef(o->vcpu_getController(), "onSetFinalPosition", this);
    loaded = 1;
  }
  inited=1;
}

H_Slider::~H_Slider() {
  if (!inited) return;
  api->maki_remDlfRef(this);
}

ScriptObject *H_Slider::getHookedObject() {
  if (obj != NULL) return obj;
  return H_SLIDER_PARENT::getHookedObject();
}

int H_Slider::eventCallback(ScriptObject *object, int dlfid, scriptVar **params, int nparams) {
  if (object != getHookedObject()) return 0;
  if (H_SLIDER_PARENT::eventCallback(object, dlfid, params, nparams)) return 1;
  if (dlfid == onsetposition_id) { hook_onSetPosition(GET_SCRIPT_INT(*params[0])); return 1; }
  if (dlfid == onpostedposition_id) { hook_onPostedPosition(GET_SCRIPT_INT(*params[0])); return 1; }
  if (dlfid == onsetfinalposition_id) { hook_onSetFinalPosition(GET_SCRIPT_INT(*params[0])); return 1; }
  return 0;
}

int H_Slider::onsetposition_id=0;
int H_Slider::onpostedposition_id=0;
int H_Slider::onsetfinalposition_id=0;
int H_Slider::loaded=0;
