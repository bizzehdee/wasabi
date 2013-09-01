#include <precomp.h>
#include <wnd/basewnd.h>
#include "widgets/xuiobject.h"
#include "xuidndsession.h"

XUIDndSession::XUIDndSession(XUIObject *dragSource) : 
               m_canDrop(false),
               m_dropTarget(NULL),
               m_dragSource(dragSource) {
}

XUIDndSession::~XUIDndSession() {
}

void XUIDndSession::acceptDrop() { 
  m_canDrop = true; 
}

bool XUIDndSession::dropAccepted() { 
  return m_canDrop; 
}

XUIObject *XUIDndSession::getDropTarget() { 
  return m_dropTarget;
}

SCRIPT void XUIDndSession::addDragObject(const char *dragType, void *dragObject);
//SCRIPT void addDragObjects(const char *dragType, PtrList<void *>dragObjects);

void XUIDndSession::setDropTarget(XUIObject *target) { 
  m_dropTarget = target;
}

void XUIDndSession::setDragSource(XUIObject *dragsource) {
  m_dragSource = dragsource;
}

void XUIDndSession::resetAcceptDrop() {
  m_canDrop = false;
}

void XUIDndSession::addDragObject(const char *dragType, void *dragObject) {
  m_dragSource->addDragObject(dragType, dragObject);
}

int XUIDndSession::getNumDragTypes() {
  DragInterface *drag = m_dragSource->getRootWnd()->getDragInterface();
  if (drag) {
    return drag->dragGetNumTypes();
  }
  return 0;
}

const char *XUIDndSession::enumDragType(int n) {
  DragInterface *drag = m_dragSource->getRootWnd()->getDragInterface();
  if (drag) {
    return drag->dragGetType(n);
  }
  return NULL;
}

int XUIDndSession::getNumDragObjects(const char *type) {
  DragInterface *drag = m_dragSource->getRootWnd()->getDragInterface();
  if (drag) {
    int n = 0;
    drag->dragCheckData(type, &n);
    return n;
  }
  return 0;
}

void *XUIDndSession::enumDragObject(const char *type, int n) {
  DragInterface *drag = m_dragSource->getRootWnd()->getDragInterface();
  if (drag) {
    int slot = drag->dragCheckData(type);
    if (slot != -1)
      return drag->dragGetData(slot, n);
  }
  return NULL;
}

bool XUIDndSession::hasDragType(const char *type) {
  return getNumDragObjects(type) > 0;
}