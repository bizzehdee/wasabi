#include <precomp.h>
#include <bfc/util/inifile.h>
#include <bfc/std_wnd.h>
#include "dockhostwnd.h"

DockHostWnd::DockHostWnd() {
  inupddock = 0;
  m_docksites = DOCK_ALL;	// call allowDocking(DOCK_NONE) to turn off
}

DockHostWnd::~DockHostWnd() {
  saveDockedState();
  foreach(m_docked_windows)
    DockableWnd *dockablewnd = m_docked_windows.getfor()->getDockableWindow();
    delete dockablewnd;
  endfor
  m_docked_windows.deleteAll();
}

int DockHostWnd::onResize() {
  DOCKHOSTWND_PARENT::onResize();
  if (!inupddock) updateDockedWindows();
  return 1;
}

void DockHostWnd::getClientRect(RECT *rect) {
  DOCKHOSTWND_PARENT::getClientRect(rect);
  rect->left += getLeftOffset();
  rect->top += getTopOffset();
  rect->right -= getRightOffset();
  rect->bottom -= getBottomOffset();
  if (m_docksites & DOCK_LEFT) rect->left += getDockedContentSize(DOCK_LEFT);
  if (m_docksites & DOCK_TOP) rect->top += getDockedContentSize(DOCK_TOP);
  if (m_docksites & DOCK_RIGHT) rect->right -= getDockedContentSize(DOCK_RIGHT);
  if (m_docksites & DOCK_BOTTOM) rect->bottom -= getDockedContentSize(DOCK_BOTTOM);
}

void DockHostWnd::allowDocking(int docksites) {
  m_docksites = docksites;
}

int DockHostWnd::isDockingAllowed(int site) {
  return !!(m_docksites & site);
}

int DockHostWnd::getLeftDockSite() {
  if (!(m_docksites & DOCK_LEFT)) return -1;
  return getDockedContentSize(DOCK_LEFT);
}

int DockHostWnd::getRightDockSite() {
  if (!(m_docksites & DOCK_RIGHT)) return -1;
  return getDockedContentSize(DOCK_RIGHT);
}

int DockHostWnd::getTopDockSite() {
  if (!(m_docksites & DOCK_TOP)) return -1;
  return getDockedContentSize(DOCK_TOP);
}

int DockHostWnd::getBottomDockSite() {
  if (!(m_docksites & DOCK_BOTTOM)) return -1;
  return getDockedContentSize(DOCK_BOTTOM);
}

int DockHostWnd::dockWindow(DockableWnd *window, int site, int insertion_pos) {
  if (!(m_docksites & site)) return 0;

  int pos = -1;
  DockEntry *old = m_sorted_docked_windows.findItem((const char *)&window, &pos);

  if (!window->getWindow()->isInited()) {
    window->getWindow()->setParent(this);
    window->getWindow()->init(this);
  } else {
    ASSERTPR(old, "Do not init your dockWindow, that's the DockHostWnd's job");
  }

  if (old && pos != -1) {
    delete old;
    m_sorted_docked_windows.removeByPos(pos);
    m_docked_windows.removeItem(old);
    old = NULL;
  }

  DockEntry *entry = new DockEntry(window, site);
  m_docked_windows.addItem(entry, insertion_pos);
  m_sorted_docked_windows.addItem(entry);

  if (!(window->getDockSites() & site)) {
    window->onDock(this, DOCK_NONE);
    entry->setDockSite(DOCK_NONE);
  } else {
    window->onDock(this, site);
    entry->setDockSite(site);
  }

  if (isPostOnInit()) {
    updateDockedWindows();
    onResize();
  }
  
  return 1;
}

int DockHostWnd::onInit() {
  DOCKHOSTWND_PARENT::onInit();
  updateDockedWindows();
  return 1;
}

int DockHostWnd::getDockedContentSize(int site) {
  if (!(m_docksites & site)) return -1;
  int size = 0;
  foreach(m_docked_windows)
    if (!m_docked_windows.getfor()->getDockableWindow()->getWindow()->isVisible(1)) continue;
    if (m_docked_windows.getfor()->getDockSite() == site) {
      int s = m_docked_windows.getfor()->getSize();
      if (s == -1) s = 24;
      size += s;
    }
  endfor;
  return size;
}


void DockHostWnd::updateDockedWindows() {
  if (m_docksites == DOCK_NONE || inupddock) return;
  inupddock = 1;

  RECT r;
  DOCKHOSTWND_PARENT::getClientRect(&r);

  r.left += getLeftOffset();
  r.top += getTopOffset();
  r.right -= getRightOffset();
  r.bottom -= getBottomOffset();

  foreach(m_docked_windows)

    DockEntry *entry = m_docked_windows.getfor();
    if (!entry->getDockableWindow()->getWindow()->isVisible(1)) continue;
    switch (entry->getDockSite()) {
      case DOCK_LEFT: {
        int w = entry->getSize();
        entry->getDockableWindow()->getWindow()->resize(r.left, r.top, w, r.bottom-r.top);
        entry->getDockableWindow()->onDockChange();
        r.left += w;
      }
      break;
      case DOCK_TOP: {
        int h = entry->getSize();
        entry->getDockableWindow()->getWindow()->resize(r.left, r.top, r.right-r.left, h);
        entry->getDockableWindow()->onDockChange();
        r.top += h;
      }
      break;
      case DOCK_RIGHT: {
        int w = entry->getSize();
        entry->getDockableWindow()->getWindow()->resize(r.right-w, r.top, w, r.bottom-r.top);
        entry->getDockableWindow()->onDockChange();
        r.right -= w;
      }
      break;
      case DOCK_BOTTOM: {
        int h = entry->getSize();
        entry->getDockableWindow()->getWindow()->resize(r.left, r.bottom-h, r.right-r.left, h);
        entry->getDockableWindow()->onDockChange();
        r.bottom -= h;
      }
      break;
    }
  endfor;
  if (isPostOnInit()) {
    onResize();    
    foreach(m_docked_windows)
      RootWnd *w = m_docked_windows.getfor()->getDockableWindow()->getWindow();
      w->invalidate();
      if (!w->isVirtual()) StdWnd::update(w->gethWnd());
    endfor;
  }
  inupddock = 0;
}

void DockHostWnd::saveDockedState() {
  const char *appname = WASABI_API_APP->main_getAppName();
  if (appname  == NULL || *appname == 0) { DebugString("DockHostWnd::saveDockedState requires you to set an app name via WasabiVersion::setName"); return; }
  const char *hostname = getName();
  if (hostname == NULL || *hostname == 0) { DebugString("DockHostWnd::saveDockedState requires you to set a window name via setName"); return; }
  int p=0;
  foreach(m_docked_windows)
    #ifdef WASABI_API_CONFIG
      //todo
    #else
      String inifile = StringPrintf("%s.ini", appname);
      IniFile ini(inifile);
      const char *wndname = m_docked_windows.getfor()->getDockableWindow()->getWindow()->getRootWndName();
      if (wndname == NULL || *wndname == 0) continue;
      ini.setInt(StringPrintf("%s_dockhost", hostname), StringPrintf("%s_order", wndname), p);
      ini.setInt(StringPrintf("%s_dockhost", hostname), StringPrintf("%s_site", wndname), m_docked_windows.getfor()->getDockSite());
      ini.setInt(StringPrintf("%s_dockhost", hostname), StringPrintf("%s_width", wndname), m_docked_windows.getfor()->getDockableWindow()->getDockedWidth());
      ini.setInt(StringPrintf("%s_dockhost", hostname), StringPrintf("%s_height", wndname), m_docked_windows.getfor()->getDockableWindow()->getDockedHeight());
      m_docked_windows.getfor()->getDockableWindow()->saveCustomState(inifile, StringPrintf("%s_dockhost", hostname));
      p++;
    #endif
  endfor;
}

void DockHostWnd::restoreDockedState() {
  const char *appname = WASABI_API_APP->main_getAppName();
  if (appname == NULL || *appname == 0) { DebugString("DockHostWnd::saveDockedState requires you to set an app name via WasabiVersion::setName"); return; }
  const char *hostname = getName();
  if (hostname == NULL || *hostname == 0) { DebugString("DockHostWnd::saveDockedState requires you to set a window name via setName"); return; }

  PtrListInsertSorted<DockOrder, DockOrderSort> sorter;

  #ifdef WASABI_API_CONFIG
    //todo
  #else
    String inifile = StringPrintf("%s.ini", appname);
    IniFile ini(inifile);

    int p = 0;
    foreach(m_docked_windows)
      DockEntry *entry = m_docked_windows.getfor();
      const char *wndname = m_docked_windows.getfor()->getDockableWindow()->getWindow()->getRootWndName();
      if (wndname == NULL || *wndname == 0) {
        sorter.addItem(new DockOrder(entry, -99999+foreach_index));
        continue;
      }
      int order = ini.getInt(StringPrintf("%s_dockhost", hostname), StringPrintf("%s_order", wndname), p);
      int site = ini.getInt(StringPrintf("%s_dockhost", hostname), StringPrintf("%s_site", wndname), entry->getDockSite());
      int width = ini.getInt(StringPrintf("%s_dockhost", hostname), StringPrintf("%s_width", wndname), entry->getDockableWindow()->getDockedWidth());
      int height = ini.getInt(StringPrintf("%s_dockhost", hostname), StringPrintf("%s_height", wndname), entry->getDockableWindow()->getDockedHeight());
      entry->setDockSite(site);
      entry->getDockableWindow()->onDock(this, site);
      entry->getDockableWindow()->setDockedWidth(width);
      entry->getDockableWindow()->setDockedHeight(height);
      m_docked_windows.getfor()->getDockableWindow()->loadCustomState(inifile, StringPrintf("%s_dockhost", hostname));
      sorter.addItem(new DockOrder(entry, order));
      p++;
    endfor;
  #endif

  m_docked_windows.removeAll();
  foreach(sorter)
    m_docked_windows.addItem(sorter.getfor()->getDockEntry());
  endfor

  sorter.deleteAll();
  updateDockedWindows();
}

int DockHostWnd::testDockWindow(DockableWnd *wnd, int x, int y, RECT *r, int *pos) {
  if (m_docksites == DOCK_NONE) return -1;
  int best = DOCK_NONE;
  int notthisrect = 0;

  RECT _r;
  wnd->getWindow()->getClientRect(&_r);

  RECT rect;
  getClientRect(&rect);

  int width = rect.right - rect.left;
  int height = rect.bottom - rect.top;

  // check if it overlaps an existing docked window, if so, use its side and 
  // its index so that dropping will insert the dragged window before that window

  foreach(m_docked_windows)
    RECT dw;
    DockableWnd *w = m_docked_windows.getfor()->getDockableWindow();
    RootWnd *_wnd = w->getWindow();
    _wnd->getClientRect(&dw);
    _wnd->clientToScreen((int *)&dw.left, (int *)&dw.top);
    _wnd->clientToScreen((int *)&dw.right, (int *)&dw.bottom);
    screenToClient(&dw);
    if (x >= dw.left && y >= dw.top && x <= dw.right && y <= dw.bottom) {
      if (pos) *pos = foreach_index;
      int site = w->getCurDockSite();
      if (r) {
        _wnd->getClientRect(r);
        width = r->right - r->left;
        height = r->bottom - r->top;
        if (site == DOCK_LEFT || site == DOCK_RIGHT) {
          r->right = r->left + wnd->getDockedWidth();
          r->bottom = r->top + height;
        } else if (site == DOCK_TOP || site == DOCK_BOTTOM) {
          r->bottom = r->top + wnd->getDockedHeight();
          r->right = r->left + width;
        }
      }
      return site;
    }
  endfor

  // use the inside client rectangle and find the edge closest to the mouse.
  // ignore the side if the mouse is further than 64 pixels. if a side works,
  // make the insertion point be at the bottom of the list

  // if we're outside the rect by more than 64px in any direction, that's not a valid drop position
  if (y < rect.top-64 || y >= rect.bottom+64) { notthisrect=1; }
  if (x < rect.left-64 || x >= rect.right+64) { notthisrect=1; }

  int bestd = 64;
  int d = 64;

  if (!notthisrect) {
    d = ABS<int>(rect.top-y);
    if (d < bestd) { best = DOCK_TOP; bestd = d; }
    d = ABS<int>(rect.bottom-y);
    if (d < bestd) { best = DOCK_BOTTOM; bestd = d; }
    d = ABS<int>(rect.left-x);
    if (d < width) { best = DOCK_LEFT; bestd = d; }
    d = ABS<int>(rect.right-x);
    if (d < bestd) { best = DOCK_RIGHT; bestd = d; }
    if (best != DOCK_NONE && pos) *pos = -1;
  }
  
  // use the outside client rectangle and find the closest edge to the mouse,
  // but only if that edge is closer to whatever we've found so far. if that
  // works, make the insertion point be the top of the list

  int secd = d;
  
  DOCKHOSTWND_PARENT::getClientRect(&rect);
  rect.left += getLeftOffset();
  rect.top += getTopOffset();
  rect.right -= getRightOffset();
  rect.bottom -= getBottomOffset();

  // if we're outside the rect by more than 64px in any direction, that's not a valid drop position
  notthisrect = 0;
  if (y < rect.top-64 || y >= rect.bottom+64) { notthisrect=1; }
  if (x < rect.left-64 || x >= rect.right+64) { notthisrect=1; }

  if (!notthisrect) {
    int sec = 0;
    d = ABS<int>(rect.top-y);
    if (d < bestd) { best = DOCK_TOP; bestd = d; sec = 1; }
    d = ABS<int>(rect.bottom-y);
    if (d < bestd) { best = DOCK_BOTTOM; bestd = d; sec = 1; }
    d = ABS<int>(rect.left-x);
    if (d < bestd) { best = DOCK_LEFT; bestd = d; sec = 1; }
    d = ABS<int>(rect.right-x);
    if (d < bestd) { best = DOCK_RIGHT; bestd = d; sec = 1; }
  
    if (sec && pos) *pos = 0;
  }

  // calculate a target drop size

  if (r) {
    *r = _r;
    if (best == DOCK_LEFT || best == DOCK_RIGHT) {
      r->right = r->left + wnd->getDockedWidth();
      r->bottom = r->top + height;
    } else if (best == DOCK_TOP || best == DOCK_BOTTOM) {
      r->bottom = r->top + wnd->getDockedHeight();
      r->right = r->left + width;
    }
  }

  return best;
}

void DockHostWnd::invalidateDockedWindows() {
  foreach(m_docked_windows)
    m_docked_windows.getfor()->getDockableWindow()->getWindow()->invalidate();
  endfor;
}

