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
#include "c_dropdownlist.h"
#include "../objcontroller.h"

C_DropDownList::C_DropDownList(ScriptObject *object) : C_GuiObject(object) {
  inited = 0;
  C_hook(object);
}

C_DropDownList::C_DropDownList() {
  inited = 0;
}

void C_DropDownList::C_hook(ScriptObject *object) {
  ASSERT(!inited);
  ScriptObjectController *controller = object->vcpu_getController();
  obj = controller->cast(object, dropDownListGuid);
  if (obj != object && obj != NULL)
    controller = obj->vcpu_getController();
  else
    obj = NULL;

  if (!loaded) {
    loaded = 1;
    getitemselected_id = api->maki_addDlfRef(controller, "getItemSelected", this);
  }
  inited = 1;
}

C_DropDownList::~C_DropDownList() {
}

ScriptObject *C_DropDownList::getScriptObject() {
  if (obj != NULL) return obj;
  return C_DROPDOWNLIST_PARENT::getScriptObject();
}

const char *C_DropDownList::getItemSelected() {
  ASSERT(inited);
  return GET_SCRIPT_STRING(api->maki_callFunction(getScriptObject(), getitemselected_id, NULL));
}

int C_DropDownList::loaded=0;
int C_DropDownList::getitemselected_id=0;
