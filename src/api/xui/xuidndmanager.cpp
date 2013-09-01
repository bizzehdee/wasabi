#include <precomp.h>
#include "xuidndmanager.h"

XUIDndManager xuiDndManager;
XUIDndManager *g_xuiDndManager = &xuiDndManager;

XUIDndManager::XUIDndManager() : m_session(NULL) {
}

XUIDndManager::~XUIDndManager() {
  delete m_session;
}

XUIDndSession *XUIDndManager::newSession(XUIObject *dragSource) {
  m_session = new XUIDndSession(dragSource);
  return m_session;
}

void XUIDndManager::endSession() {
  delete m_session;
  m_session = NULL;
}

XUIDndSession *XUIDndManager::getSession() {
  return m_session;
}
