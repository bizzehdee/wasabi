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
#include "c_edit.h"
#include "../objcontroller.h"

C_Edit::C_Edit(ScriptObject *object) : C_GuiObject(object) {
  inited = 0;
  C_hook(object);
}

C_Edit::C_Edit() {
  inited = 0;
}

void C_Edit::C_hook(ScriptObject *object) {
  ASSERT(!inited);
  ScriptObjectController *controller = object->vcpu_getController();
  obj = controller->cast(object, editGuid);
  if (obj != object && obj != NULL)
    controller = obj->vcpu_getController();
  else
    obj = NULL;

  if (!loaded) {
    loaded = 1;
    onenter_id = api->maki_addDlfRef(controller, "onEnter", this);
    onabort_id = api->maki_addDlfRef(controller, "onAbort", this);
    onidleeditupdate_id = api->maki_addDlfRef(controller, "onIdleEditUpdate", this);
    oneditupdate_id = api->maki_addDlfRef(controller, "onEditUpdate", this);
    settext_id = api->maki_addDlfRef(controller, "setText", this);
    setautoenter_id = api->maki_addDlfRef(controller, "setAutoEnter", this);
    getautoenter_id = api->maki_addDlfRef(controller, "getAutoEnter", this);
    gettext_id = api->maki_addDlfRef(controller, "getText", this);
    selectall_id = api->maki_addDlfRef(controller, "selectAll", this);
    enter_id = api->maki_addDlfRef(controller, "enter", this);
    setidleenabled_id = api->maki_addDlfRef(controller, "setIdleEnabled", this);
    getidleenabled_id = api->maki_addDlfRef(controller, "getIdleEnabled", this);
  }
  inited = 1;
}

C_Edit::~C_Edit() {
}

ScriptObject *C_Edit::getScriptObject() {
  if (obj != NULL) return obj;
  return C_EDIT_PARENT::getScriptObject();
}

void C_Edit::onEnter() {
  ASSERT(inited);
  api->maki_callFunction(getScriptObject(), onenter_id, NULL);
}

void C_Edit::onAbort() {
  ASSERT(inited);
  api->maki_callFunction(getScriptObject(), onabort_id, NULL);
}

void C_Edit::onIdleEditUpdate() {
  ASSERT(inited);
  api->maki_callFunction(getScriptObject(), onidleeditupdate_id, NULL);
}

void C_Edit::onEditUpdate() {
  ASSERT(inited);
  api->maki_callFunction(getScriptObject(), oneditupdate_id, NULL);
}

void C_Edit::setText(const char *txt) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_STRING(txt);
  scriptVar *params[1] = {&a};
  api->maki_callFunction(getScriptObject(), settext_id, params);
}

void C_Edit::setAutoEnter(int onoff) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(onoff);
  scriptVar *params[1] = {&a};
  api->maki_callFunction(getScriptObject(), setautoenter_id, params);
}

int C_Edit::getAutoEnter() {
  ASSERT(inited);
  return GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getautoenter_id, NULL));
}

const char *C_Edit::getText() {
  ASSERT(inited);
  return GET_SCRIPT_STRING(api->maki_callFunction(getScriptObject(), gettext_id, NULL));
}

void C_Edit::selectAll() {
  ASSERT(inited);
  api->maki_callFunction(getScriptObject(), selectall_id, NULL);
}

void C_Edit::enter() {
  ASSERT(inited);
  api->maki_callFunction(getScriptObject(), enter_id, NULL);
}

void C_Edit::setIdleEnabled(int onoff) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_INT(onoff);
  scriptVar *params[1] = {&a};
  api->maki_callFunction(getScriptObject(), setidleenabled_id, params);
}

int C_Edit::getIdleEnabled() {
  ASSERT(inited);
  return GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), getidleenabled_id, NULL));
}

int C_Edit::loaded=0;
int C_Edit::onenter_id=0;
int C_Edit::onabort_id=0;
int C_Edit::onidleeditupdate_id=0;
int C_Edit::oneditupdate_id=0;
int C_Edit::settext_id=0;
int C_Edit::setautoenter_id=0;
int C_Edit::getautoenter_id=0;
int C_Edit::gettext_id=0;
int C_Edit::selectall_id=0;
int C_Edit::enter_id=0;
int C_Edit::setidleenabled_id=0;
int C_Edit::getidleenabled_id=0;
