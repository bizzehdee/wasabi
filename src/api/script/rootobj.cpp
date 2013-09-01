#include "precomp.h"
#include "scriptobj.h"
#include "rootobj.h"
//#include "../../studio/api.h"
#include "scriptguid.h"

RootObjectInstance::RootObjectInstance() {
  my_script_object = new ScriptObjectI;
  //my_root_object = static_cast<RootObject *>(api->maki_encapsulate(rootObjectGuid, my_script_object)); // creates an encapsulated RootObject
  my_script_object->vcpu_setInterface(rootObjectInstanceGuid, static_cast<void *>(this));
  my_script_object->vcpu_setClassName("Object");
  //my_script_object->vcpu_setController(api->maki_getController(rootObjectGuid));
}

RootObjectInstance::~RootObjectInstance() {
  //api->maki_deencapsulate(rootObjectGuid, my_root_object);
  delete my_script_object;
}

void RootObjectInstance::notify(const char *s, const char *t, int u, int v) {
  my_root_object->rootobject_notify(s, t, u, v);
}

const char *RootObjectInstance::getClassName() {
  return my_root_object->rootobject_getClassName();
}

RootObject *RootObjectInstance::getRootObject() {
  return my_root_object;
}

ScriptObject *RootObjectInstance::getScriptObject() {
  return my_script_object;
}

