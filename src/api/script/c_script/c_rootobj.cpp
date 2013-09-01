#include "precomp.h"
//#include "../../../studio/api.h"
#include "../scriptobj.h"
#include "../objcontroller.h"
#include "c_rootobj.h"

C_RootObject::C_RootObject(ScriptObject *o) {
  inited=0;
  C_hook(o);
}

C_RootObject::C_RootObject() {
  inited=0;
}

void C_RootObject::C_hook(ScriptObject *o) {
  ASSERT(!inited);
  object = o;/*
  if (count++ == 0) {
    getclassname_id = api->maki_addDlfRef(o->vcpu_getController(), "getClassName", this);
    notify_id = api->maki_addDlfRef(o->vcpu_getController(), "notify", this);
  }*/
  inited=1;
}

C_RootObject::~C_RootObject() {
  if (!inited) return;
  //api->maki_remDlfRef(this);
  //count--;
}

ScriptObject *C_RootObject::getScriptObject() {
  ASSERT(inited);
  return object;
}

const char *C_RootObject::getClassName() {
  ASSERT(inited);
  return NULL;//GET_SCRIPT_STRING(api->maki_callFunction(object, getclassname_id, NULL));
}

void C_RootObject::notify(const char *a, const char *b, int c, int d) {
  ASSERT(inited);
  scriptVar _a = MAKE_SCRIPT_STRING(a);
  scriptVar _b = MAKE_SCRIPT_STRING(b);
  scriptVar _c = MAKE_SCRIPT_INT(c);
  scriptVar _d = MAKE_SCRIPT_INT(d);
  scriptVar *params[4]={&_a, &_b, &_c, &_d};
  //api->maki_callFunction(object, notify_id, params);
}

int C_RootObject::getclassname_id=0;
int C_RootObject::notify_id=0;
int C_RootObject::inited=0;
int C_RootObject::count=0;
