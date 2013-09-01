#include "precomp.h"
//#include "../../../studio/api.h"
#include "c_text.h"
#include "../objcontroller.h"

C_Text::C_Text(ScriptObject *object) : C_GuiObject(object) {
  inited = 0;
  C_hook(object);
}

C_Text::C_Text() {
  inited = 0;
}

void C_Text::C_hook(ScriptObject *object) {
  ASSERT(!inited);
  ScriptObjectController *controller = object->vcpu_getController();
  obj = controller->cast(object, textGuid);
  if (obj != object && obj != NULL)
    controller = obj->vcpu_getController();
  else
    obj = NULL;

  if (!loaded) {
    loaded = 1;/*
    settext_id = api->maki_addDlfRef(controller, "setText", this);
    setalternatetext_id = api->maki_addDlfRef(controller, "setAlternateText", this);
    gettext_id = api->maki_addDlfRef(controller, "getText", this);
    gettextwidth_id = api->maki_addDlfRef(controller, "getTextWidth", this);
    ontextchanged_id = api->maki_addDlfRef(controller, "onTextChanged", this);*/
  }
  inited = 1;
}

C_Text::~C_Text() {
}

ScriptObject *C_Text::getScriptObject() {
  if (obj != NULL) return obj;
  return C_TEXT_PARENT::getScriptObject();
}

void C_Text::setText(const char *txt) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_STRING(txt);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), settext_id, params);
}

void C_Text::setAlternateText(const char *txt) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_STRING(txt);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), setalternatetext_id, params);
}

const char *C_Text::getText() {
  ASSERT(inited);
  return NULL;//GET_SCRIPT_STRING(api->maki_callFunction(getScriptObject(), gettext_id, NULL));
}

int C_Text::getTextWidth() {
  ASSERT(inited);
  return 0;//GET_SCRIPT_INT(api->maki_callFunction(getScriptObject(), gettextwidth_id, NULL));
}

void C_Text::onTextChanged(const char *newtxt) {
  ASSERT(inited);
  scriptVar a = MAKE_SCRIPT_STRING(newtxt);
  scriptVar *params[1] = {&a};
  //api->maki_callFunction(getScriptObject(), ontextchanged_id, params);
}

int C_Text::loaded=0;
int C_Text::settext_id=0;
int C_Text::setalternatetext_id=0;
int C_Text::gettext_id=0;
int C_Text::gettextwidth_id=0;
int C_Text::ontextchanged_id=0;
