#ifndef XUIDNDMANAGER_H
#define XUIDNDMANAGER_H

#include "xuidndsession.h"

class XUIDndManager {
public:
  XUIDndManager();
  virtual ~XUIDndManager();

  XUIDndSession *newSession(XUIObject *dragSource);
  XUIDndSession *getSession();
  void endSession();

private:
  XUIDndSession *m_session;
};

extern XUIDndManager *g_xuiDndManager;

#endif