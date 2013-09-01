#ifndef _SVC_ACTION_H
#define _SVC_ACTION_H

#include <bfc/dispatch.h>
#include <bfc/string/string.h>
#include <bfc/ptrlist.h>

#include <api/service/services.h>

class RootWnd;

class NOVTABLE svc_action : public Dispatchable {
protected:
  svc_action() { }

public:
  static GUID getServiceType() { return WaSvc::ACTION; }
  
  int hasAction(const char *name);
  int onAction(const char *action, const char *param=NULL, int p1=0, int p2=0, void *data=NULL, int datalen=0, RootWnd *source=NULL);

  enum {
    HASACTION=10,
    ONACTION=20,
  };

};

inline int svc_action::hasAction(const char *name) {
  return _call(HASACTION, 0, name);
}

inline int svc_action::onAction(const char *action, const char *param, int p1, int p2, void *data, int datalen, RootWnd *source) {
  return _call(ONACTION, 0, action, param, p1, p2, data, datalen, source);
}

class ActionEntry {
  public:
    ActionEntry(const char *_action, int _id) : action(_action), id(_id) {}
    virtual ~ActionEntry() { }

    const char *getAction() { return action; }
    int getId() { return id; }

  private:
    String action;
    int id;
};

class SortActions {
public:
  static int compareItem(ActionEntry *p1, ActionEntry *p2) {
    return STRICMP(p1->getAction(), p2->getAction());
  }
  static int compareAttrib(const char *attrib, ActionEntry *item) {
    return STRICMP(attrib, item->getAction());
  }
};

class NOVTABLE svc_actionI : public svc_action {
public:
    virtual ~svc_actionI();
    void registerAction(const char *actionid, int pvtid);
    virtual int onActionId(int pvtid, const char *action, const char *param=NULL, int p1=0, int p2=0, void *data=NULL, int datalen=0, RootWnd *source=NULL)=0;

protected:
  virtual int hasAction(const char *name);
  virtual int onAction(const char *action, const char *param=NULL, int p1=0, int p2=0, void *data=NULL, int datalen=0, RootWnd *source=NULL);

  PtrListQuickSorted<ActionEntry, SortActions> actions;

  RECVS_DISPATCH;
};
#include <api/service/servicei.h>
template <class T>
class ActionCreator : public waServiceFactoryT<svc_action, T> {};
template <class T>
class ActionCreatorSingle : public waServiceFactoryTSingle<svc_action, T> {
public:
  svc_action *getHandler() {
    return getSingleService();
  }
};

#include <api/service/svc_enum.h>
#include <bfc/string/string.h>

class ActionEnum : public SvcEnumT<svc_action> {
public:
  ActionEnum(const char *_action) : action(_action) { }
protected:
  virtual int testService(svc_action *svc) {
    return (!action.isempty() && svc->hasAction(action));
  }
private:
  String action;
};

class FireAction {
public:
  enum {
    ACTION_NOT_HANDLED = 0x80000000
  };
  /**
    Fire a named action out into the system with the given parameters.

    This method will only send the action to the first registered handler for that action.

    This prevents the action from being overridden or handled by newer wacs.

    The content and syntax of the generalized params are defined by the handler of the action string.

    Read: Using Wasabi: General Development: Actions
      
    @see                svc_actionI
    @param action       The action string.
    @param param        A string parameter to the action.
    @param p1           The first integer parameter to the action.
    @param p2           The second integer parameter to the action.
    @param data         An untyped data buffer parameter to the action.
    @param datalen      The size in bytes of the data buffer parameter.
    @param source       A window object that can be given as the source object, if the action handler is expecting one.  Actions bound to guiobjects use that guiobject's rootwnd pointer as the source.
    @param apply_to_all Send the action to everyone.  (If false only sends to first registered)
  */
  FireAction(const char *action, const char *param = NULL, int p1 = 0, int p2 = 0, void *data = NULL, int datalen = 0, RootWnd *source = NULL, int apply_to_all = TRUE) {
    lastretval = ACTION_NOT_HANDLED;
    ActionEnum ae(action);
    svc_action *act;
    while ((act = ae.getNext()) != NULL) {
      lastretval = act->onAction(action, param, p1, p2, data, datalen, source);
      ae.release(act);
      if (!apply_to_all) break;
    }
  }
  /**
    More robust retval handling is needed.

    I ought to be grabbing all of the return values into a list an exposing that.
    
    Later.

    Read: Using Wasabi: General Development: Actions
      
    @see                svc_actionI
    @ret                The return code of the action sent.
  */
  int getLastReturnValue() {
    return lastretval;
  }
private:
  int lastretval;
};

#endif
