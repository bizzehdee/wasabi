#ifndef XUIDNDSESSION_H
#define XUIDNDSESSION_H

#include "widgets/js/JSXUIDndSession.h"
#include "widgets/sourcesink/XUIDndSessionEventSource.h"

class XUIObject;

class XUIDndSession : public JSScriptable,
                      public XUIDndSessionEventSource,
                      public JSXUIDndSession {
public:
  XUIDndSession(XUIObject *dragSource);
  virtual ~XUIDndSession();

  SCRIPT void acceptDrop();
  SCRIPT bool dropAccepted();
  SCRIPT int getNumDragTypes();
  SCRIPT const char *enumDragType(int n);
  SCRIPT int getNumDragObjects(const char *type);
  SCRIPT void *enumDragObject(const char *type, int n);
  SCRIPT bool hasDragType(const char *type);

  SCRIPT XUIObject *getDropTarget();

  SCRIPT EVENT void onDragEnter(XUIObject *target) { eventSource_onDragEnter(target); }
  SCRIPT EVENT void onDragLeave(XUIObject *target) { eventSource_onDragLeave(target); }
  SCRIPT EVENT void onDragOver(XUIObject *target, int x, int y) { eventSource_onDragOver(target, x, y); }
  SCRIPT EVENT void onDragDrop(XUIObject *target, int x, int y) { eventSource_onDragDrop(target, x, y); }

  SCRIPT void addDragObject(const char *dragType, void *dragObject);
  //SCRIPT void addDragObjects(const char *dragType, PtrList<void *>dragObjects);
  
  void setDropTarget(XUIObject *target);

  void setDragSource(XUIObject *dragsource);
  void resetAcceptDrop();

  bool m_canDrop;
  XUIObject *m_dropTarget;
  XUIObject *m_dragSource;
};

#endif
