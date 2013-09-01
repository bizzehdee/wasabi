#include <precomp.h>

#include "svc_action.h"

#define CBCLASS svc_actionI
START_DISPATCH;
  CB(HASACTION, hasAction);
  CB(ONACTION, onAction);
END_DISPATCH;
#undef CBCLASS

svc_actionI::~svc_actionI() {
  actions.deleteAll();
}

void svc_actionI::registerAction(const char *actionid, int pvtid) {
  ASSERT(actionid != NULL);
  actions.addItem(new ActionEntry(actionid, pvtid));
}

int svc_actionI::hasAction(const char *name) {
  if (name == NULL) return FALSE;
  return (actions.findItem(name) != NULL);
}

int svc_actionI::onAction(const char *action, const char *param, int p1, int p2, void *data, int datalen, RootWnd *source) {
  if (action == NULL) return 0;
  int pos=-1;
  if (actions.findItem(action, &pos)) {
    return onActionId(actions.enumItem(pos)->getId(), action, param, p1, p2, data, datalen, source);
  }
  return 0;
}
