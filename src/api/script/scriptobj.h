#ifndef __SCRIPTOBJ_H
#define __SCRIPTOBJ_H

#include "vcputypes.h"
#include "bfc/dispatch.h"

#include "bfc/tlist.h"
#include "bfc/ptrlist.h"
#include "bfc/pair.h"

using namespace wasabi;

typedef struct {
  int scriptid;
  int varid;
  TList<int> dlfs;
} assvar;

class ScriptObjectController;
class ScriptHook;

// ----------------------------------------------------------------------------------------------------------
class MemberVar {
  public:
    MemberVar(const char *name, int scriptid, int rettype);
    virtual ~MemberVar();

    const char *getName() { return name; }
    int getScriptId() { return scriptid; }
    int getReturnType() { return rettype; }
    int getGlobalId() { return globalid; }

  private:  
    String name;
    int scriptid;
    int rettype;
    int globalid;
};

/*class MemberVarCompare {
  public:
    static int compareItem(void *p1, void *p2);
    static int compareAttrib(const char *attrib, void *p);
};*/

// ----------------------------------------------------------------------------------------------------------

class COMEXP ScriptObject : public Dispatchable {

  public:
    void *vcpu_getInterface(GUID g);
	  const char *vcpu_getClassName();

    int vcpu_getAssignedVariable(int start, int scriptid, int functionId, int *next, int *globalevententry, int *inheritedevent);
    void vcpu_removeAssignedVariable(int var, int scriptid);
    void vcpu_addAssignedVariable(int var, int scriptid);
    ScriptObjectController *vcpu_getController();

    void vcpu_setInterface(GUID g, void *v);
    void vcpu_setClassName(const char *name);
    void vcpu_setController(ScriptObjectController *c);

    void vcpu_addClassHook(ScriptHook *h);
    void vcpu_addObjectHook(ScriptHook *h);
    void vcpu_setScriptId(int i);
    int vcpu_getScriptId();
    int vcpu_getMember(const char *id, int scriptid, int rettype);
    void vcpu_delMembers(int scriptid);

  enum {
    GETINTERFACE            = 50,
    GETASSIGNEDVAR          = 100,
    REMOVEASSIGNEDVAR       = 200,
    ADDASSIGNEDVAR	        = 300,
    GETCLASSNAME	          = 400,
    GETCONTROLLER           = 500,
    ADDCLASSHOOK            = 600,
    ADDOBJHOOK              = 700,
    GETSCRIPTID             = 800,
    SETSCRIPTID             = 900,
    GETMEMBER               = 1000,
    DELMEMBERS              = 1100,
    SETINTERFACE            = 1200,
    SETCLASSNAME            = 1300,
    SETCONTROLLER           = 1400,
  };

};

inline void *ScriptObject::vcpu_getInterface(GUID g) {
  return _call(GETINTERFACE, (void *)NULL, g);
}

inline const char *ScriptObject::vcpu_getClassName() {
  return _call(GETCLASSNAME, (const char *)NULL);
}

inline int ScriptObject::vcpu_getAssignedVariable(int start, int scriptid, int functionId, int *next, int *globalevententry, int *inheritedevent) {
  return _call(GETASSIGNEDVAR, 0, start, scriptid, functionId, next, globalevententry, inheritedevent);
}

inline void ScriptObject::vcpu_removeAssignedVariable(int var, int scriptid) {
  _voidcall(REMOVEASSIGNEDVAR, var, scriptid);
}

inline void ScriptObject::vcpu_addAssignedVariable(int var, int scriptid) {
  _voidcall(ADDASSIGNEDVAR, var, scriptid);
}

inline ScriptObjectController *ScriptObject::vcpu_getController() {
  return _call(GETCONTROLLER, (ScriptObjectController *)NULL);
}

inline void ScriptObject::vcpu_addClassHook(ScriptHook *h) {
  _voidcall(ADDCLASSHOOK, h);
}

inline void ScriptObject::vcpu_addObjectHook(ScriptHook *h) {
  _voidcall(ADDCLASSHOOK, h);
}

inline void ScriptObject::vcpu_setScriptId(int i) {
  _voidcall(SETSCRIPTID, i);
}

inline int ScriptObject::vcpu_getScriptId() {
  return _call(GETSCRIPTID, 0);
}

inline int ScriptObject::vcpu_getMember(const char *id, int scriptid, int rettype) {
  return _call(GETMEMBER, 0, id, scriptid, rettype);
}

inline void ScriptObject::vcpu_delMembers(int scriptid) {
  _voidcall(DELMEMBERS, scriptid);
}

inline void ScriptObject::vcpu_setInterface(GUID g, void *v) {
  _voidcall(SETINTERFACE, g, v);
}

inline void ScriptObject::vcpu_setClassName(const char *name) {
  _voidcall(SETCLASSNAME, name);
}

inline void ScriptObject::vcpu_setController(ScriptObjectController *s) {
  _voidcall(SETCONTROLLER, s);
}

class COMEXP ScriptObjectI : public ScriptObject {

public:
  ScriptObjectI(const char *classname=NULL, ScriptObjectController *controller=NULL);
  virtual ~ScriptObjectI();

  virtual void *vcpu_getInterface(GUID g);
	virtual const char *vcpu_getClassName();
  virtual ScriptObjectController *vcpu_getController();
  virtual void vcpu_setInterface(GUID g, void *v);
  virtual void vcpu_setClassName(const char *name);
  virtual void vcpu_setController(ScriptObjectController *c);

  int vcpu_getAssignedVariable(int start, int scriptid, int functionId, int *next, int *globalevententry, int *inheritedevent);
  void vcpu_removeAssignedVariable(int var, int scriptid);
  void vcpu_addAssignedVariable(int var, int scriptid);
  void vcpu_setScriptId(int i);
  int vcpu_getScriptId();

  int vcpu_getMember(const char *id, int scriptid, int rettype);
  void vcpu_delMembers(int scriptid);

protected:
  int getEventForVar(assvar *var, int funcid, int *inheritedevent);
  void computeEventList(assvar *a);

private:
  PtrList<assvar> assignedVariables;
  PtrList<MemberVar/*, MemberVarCompare*/> memberVariables;
  int cache_count;
  int id;

  String membercacheid;
  int membercachesid;
  int membercachegid;
  PtrList< Pair < GUID, void * > > interfaceslist;
  String classname;
  ScriptObjectController *controller;
  int ingetinterface;
protected:
  RECVS_DISPATCH;
};

#endif