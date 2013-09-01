#include "precomp.h"
#include "scriptobj.h"
#include "objcontroller.h"
#include "scriptguid.h"

#define CBCLASS ScriptObjectI
START_DISPATCH;
  CB(GETINTERFACE,       vcpu_getInterface);
  VCB(SETINTERFACE,      vcpu_setInterface);
  CB(GETASSIGNEDVAR,     vcpu_getAssignedVariable);
  VCB(REMOVEASSIGNEDVAR, vcpu_removeAssignedVariable);
  VCB(ADDASSIGNEDVAR,    vcpu_addAssignedVariable);
  CB(GETCLASSNAME,       vcpu_getClassName);
  VCB(SETCLASSNAME,      vcpu_setClassName);
  CB(GETCONTROLLER,      vcpu_getController);
  VCB(SETCONTROLLER,     vcpu_setController);
  CB(GETSCRIPTID,        vcpu_getScriptId);
  VCB(SETSCRIPTID,       vcpu_setScriptId);
  CB(GETMEMBER,          vcpu_getMember);
  VCB(DELMEMBERS,        vcpu_delMembers);
END_DISPATCH;

ScriptObjectI::ScriptObjectI(const char *class_name, ScriptObjectController *object_controller) {
  classname = class_name;
  controller = object_controller;
  cache_count = -1;
  membercachegid = -1;
  membercachesid = -1;
  //api->vcpu_addScriptObject(this);
  ingetinterface = 0;
}

ScriptObjectI::~ScriptObjectI() {
  assignedVariables.deleteAll();
  memberVariables.deleteAll();
  interfaceslist.deleteAll();
  //api->vcpu_removeScriptObject(this);
}

int ScriptObjectI::vcpu_getAssignedVariable(int start, int scriptid, int functionId, int *next, int *globalevententry, int *inheritedevent) {
  if (start < 0) start = 0;
  if (start >= assignedVariables.getNumItems()) return -1;
  for (int i=start;i<assignedVariables.getNumItems();i++) {
    assvar *v = assignedVariables.enumItem(i);/*
    if (api->vcpu_getCacheCount() != cache_count) {
      if (!api->vcpu_isValidScriptId(v->scriptid)) {
        vcpu_removeAssignedVariable(v->varid, v->scriptid);
        i--;
        continue;
      }
    }*/
    if (scriptid == -1 || v->scriptid == scriptid) {
      int r = getEventForVar(v, functionId, inheritedevent);
      if (r == -1) continue;
      if (next) *next = i+1;
      if (globalevententry) *globalevententry = r;
      return -1;//api->vcpu_mapVarId(v->varid, v->scriptid);
    }    
  }
  return -1;
}

void ScriptObjectI::vcpu_removeAssignedVariable(int var, int id) {
  for (int i=0;i<assignedVariables.getNumItems();i++) {
    assvar *v = assignedVariables.enumItem(i);
    if (v->varid == var && v->scriptid == id) {
      delete v;
      assignedVariables.removeItem(v);
      return;
    }
  }
}

void ScriptObjectI::vcpu_addAssignedVariable(int var, int scriptid) {
  do {
    assvar *v = new assvar;
    v->scriptid = scriptid;
    v->varid = var;
    assignedVariables.addItem(v);
    computeEventList(v);
    var = -1;//api->vcpu_getUserAncestorId(var, scriptid);
  } while (var != -1);
}

int ScriptObjectI::getEventForVar(assvar *var, int funcid, int *inheritedevent) { /*
  if (api->vcpu_getCacheCount() != cache_count) {
    for (int i=0;i<assignedVariables.getNumItems();i++)
      computeEventList(assignedVariables.enumItem(i));
    cache_count = api->vcpu_getCacheCount();
  }*/
  TList<int> *list = &var->dlfs;
  for (int i=0;i<list->getNumItems();i+=4) 
    if (list->enumItem(i) == funcid && list->enumItem(i+1) == var->varid) {
      *inheritedevent = list->enumItem(i+3);
      return list->enumItem(i+2);
    }

  return -1;
}

void ScriptObjectI::computeEventList(assvar *a) {
  a->dlfs.removeAll();

  int dlfid;
  int scriptid;
  int varid;

  int var = a->varid;
  int inheritedevent=0;

  do {/*
    for (int i=0;i<api->vcpu_getNumEvents();i++) {
      api->vcpu_getEvent(i, &dlfid, &scriptid, &varid);
      if (scriptid == a->scriptid && varid == var) {
        a->dlfs.addItem(dlfid);
        a->dlfs.addItem(varid);
        a->dlfs.addItem(i);
        a->dlfs.addItem(inheritedevent);
      }
    }*/
    var = -1;//api->vcpu_getUserAncestorId(var, a->scriptid);
    inheritedevent = 1;
  } while (var != -1);
}

ScriptObjectController *ScriptObjectI::vcpu_getController() {
  return controller;
}

int ScriptObjectI::vcpu_getScriptId() {
  return id;
}

void ScriptObjectI::vcpu_setScriptId(int i) {
  id = i;
}

int ScriptObjectI::vcpu_getMember(const char *id, int scriptid, int rettype) {
  if (membercachesid == scriptid && STRCASEEQL(membercacheid, id)) 
    return membercachegid;
  membercacheid = id;
  membercachesid = scriptid;
  for (int i=0;i<memberVariables.getNumItems();i++) {
    MemberVar *m = memberVariables.enumItem(i);
    if (m->getScriptId() == scriptid && STRCASEEQL(m->getName(), id)) {
      membercachegid = m->getGlobalId();
      return membercachegid;
    }
  }
  MemberVar *m = new MemberVar(id, scriptid, rettype);
  memberVariables.addItem(m);
  membercachegid = m->getGlobalId();
  return membercachegid;
}

void ScriptObjectI::vcpu_delMembers(int scriptid) {
  for (int i=0;i<memberVariables.getNumItems();i++)
    if (memberVariables.enumItem(i)->getScriptId() == scriptid) {
      delete memberVariables.enumItem(i);
      memberVariables.removeByPos(i--);
    }
}

void *ScriptObjectI::vcpu_getInterface(GUID g) {
  if (g == scriptObjectGuid) return this;
  foreach(interfaceslist)
    if (interfaceslist.getfor()->a == g) return interfaceslist.getfor()->b; 
  endfor;
  if (ingetinterface) return NULL;
  ingetinterface = 1;

  void *i = NULL;
  ScriptObjectController *c = controller;
  while (i == NULL && c != NULL) {
    i = c->cast(this, g);
    if (i != NULL) break;
    c = c->getAncestorController();
  }

  ingetinterface = 0;
  return i;
}

void ScriptObjectI::vcpu_setInterface(GUID g, void *v) {
  for (int i=0;i<interfaceslist.getNumItems();i++)
    if (interfaceslist.enumItem(i)->a == g) {
      Pair<GUID, void *> *p = interfaceslist.enumItem(i);
      delete p;
      interfaceslist.removeByPos(i);
      i--;
    }
  interfaceslist.addItem(new Pair<GUID, void *>(g,v));
}

void ScriptObjectI::vcpu_setClassName(const char *name) {
  classname = name;
}

const char *ScriptObjectI::vcpu_getClassName() {
  return classname;
}

void ScriptObjectI::vcpu_setController(ScriptObjectController *c) {
  controller = c;
}



MemberVar::MemberVar(const char *_name, int _scriptid, int _rettype) {
  name = _name;
  rettype = _rettype;
  scriptid = _scriptid;
  globalid = -1;//api->maki_createOrphan(rettype);
}

MemberVar::~MemberVar() {
  //api->maki_killOrphan(globalid); // heh :)
}

