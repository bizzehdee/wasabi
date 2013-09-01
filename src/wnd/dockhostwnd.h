#ifndef DOCKHOSTWND_H
#define DOCKHOSTWND_H

#include <wnd/virtualwnd.h>

enum DockSites {
  DOCK_NONE=0,
  DOCK_LEFT=1,
  DOCK_TOP=2,
  DOCK_RIGHT=4,
  DOCK_BOTTOM=8,
  DOCK_ALL=(DOCK_LEFT|DOCK_TOP|DOCK_RIGHT|DOCK_BOTTOM),
};


class DockHostWnd;

class DockableWnd {
public:
  virtual ~DockableWnd() {}

  virtual int getDockedWidth()=0;
  virtual int getDockedHeight()=0;
  virtual void setDockedWidth(int w)=0;
  virtual void setDockedHeight(int h)=0;
  virtual int getDockSites()=0;
  virtual void onDock(DockHostWnd *host, int site)=0;
  virtual RootWnd *getWindow()=0;
  virtual int getCurDockSite()=0;
  virtual void saveCustomState(const char *inifile, const char *section)=0;
  virtual void loadCustomState(const char *inifile, const char *section)=0;
  virtual void onDockChange()=0;
};

class DockEntry {
public:
  DockEntry(DockableWnd *dockwnd, int docksite) : wnd(dockwnd), site(docksite) {}
  virtual ~DockEntry() {}

  int getDockSite() { return site; }
  void setDockSite(int s) { site = s; }
  DockableWnd *getDockableWindow() { return wnd; }
  int getSize() { 
    if (site == DOCK_LEFT || site == DOCK_RIGHT) {
      ASSERT(wnd != NULL);
      return wnd->getDockedWidth();
    }
    if (site == DOCK_TOP || site == DOCK_BOTTOM) {
      ASSERT(wnd != NULL);
      return wnd->getDockedHeight();
    }
    return -1;
  }

private:
  int site;
  DockableWnd *wnd;
};

class DockOrder {
public:
  DockOrder(DockEntry *entry, int order) : m_entry(entry), m_order(order) {}
  DockEntry *getDockEntry() { return m_entry; }
  int getDockOrder() { return m_order; }

private:
  int m_order;
  DockEntry *m_entry;
};

class DockEntrySort {
public:
  // comparator for sorting
  static int compareItem(DockEntry *p1, DockEntry *p2) {
    return CMP3(p1->getDockableWindow(), p2->getDockableWindow());
  }
  // comparator for searching
  static int compareAttrib(const char *attrib, DockEntry *item) {
    return CMP3(*(DockableWnd **)attrib, item->getDockableWindow());
  }
};

class DockOrderSort {
public:
  // comparator for sorting
  static int compareItem(DockOrder *p1, DockOrder *p2) {
    return CMP3(p1->getDockOrder(), p2->getDockOrder());
  }
  // comparator for searching
  static int compareAttrib(const char *attrib, DockOrder *item) {
    return CMP3(*(int *)attrib, item->getDockOrder());
  }
};


#define DOCKHOSTWND_PARENT VirtualWnd
class DockHostWnd : public DOCKHOSTWND_PARENT {
public:
  DockHostWnd();
  virtual ~DockHostWnd();

  virtual int onInit();
  virtual void allowDocking(int allowed_sites=DOCK_ALL);
  virtual int isDockingAllowed(int site);
  int getLeftDockSite();
  int getRightDockSite();
  int getTopDockSite();
  int getBottomDockSite();
  virtual int dockWindow(DockableWnd *window, int site, int insertion_pos=PTRLIST_POS_LAST);
  virtual int testDockWindow(DockableWnd *wnd, int x, int y, RECT *r=NULL, int *insertion_pos=NULL);

  virtual int onResize();
  void updateDockedWindows();
  void getClientRect(RECT *rect);
  void restoreDockedState();
  void saveDockedState(); // do NOT make virtual, as it's called in destructor

  virtual int getLeftOffset() { return 0; }
  virtual int getTopOffset() { return 0; }
  virtual int getRightOffset() { return 0; }
  virtual int getBottomOffset() { return 0; }

  int getNumDockedWindows() { return m_docked_windows.getNumItems(); }
  DockableWnd *enumDockedWindow(int n) { DockEntry *entry = m_docked_windows.enumItem(n); return entry ? entry->getDockableWindow() : NULL; }
  void invalidateDockedWindows();

private:
  int getDockedContentSize(int site);
  int inupddock;
  int m_docksites;
  PtrList<DockEntry> m_docked_windows;
  PtrListQuickSorted<DockEntry, DockEntrySort> m_sorted_docked_windows;
};

#endif
