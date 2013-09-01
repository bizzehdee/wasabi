#ifndef MDIFRAME_H
#define MDIFRAME_H

#define DC_CLOSECHILD 100

#include <wnd/wnds/os/osmdiparentwnd.h>

class OSToolBarWnd;
class MDIChild;
class Commands;

#define MDIFRAME_PARENT OSMDIParentWnd
class MDIFrame : public MDIFRAME_PARENT {
public:
  MDIFrame();
  virtual ~MDIFrame();

  virtual int onInit();
  virtual int onDeferredCallback(int param1, int param2);

  void registerChild(MDIChild *child) { m_children.addItem(child); }
  void unregisterChild(MDIChild *child) { m_children.removeItem(child); }

  void onOpen();

private:
  PtrList<MDIChild> m_children;
  OSToolBarWnd *m_toolbar;
  Commands *m_cmds;
};

extern MDIChild *g_curchild;
extern MDIFrame *g_frame;

#endif
