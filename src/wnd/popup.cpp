#include <precomp.h>
#include "popup.h"
#include <wnd/rootwnd.h>
#include <bfc/std_wnd.h>

PopupMenu::PopupMenu(int commandcallbackid) {
  m_menuccid = commandcallbackid;
  hmenu = NULL;
  parent = NULL;
  m_menulistinvalid = 1;
  m_menulist.setAutoSort(0);
  m_menulist.addItem(this);
}

PopupMenu::PopupMenu(RootWnd *_parent, int commandcallbackid) {
  m_menuccid = commandcallbackid;
  hmenu = NULL;
  parent = _parent;
  m_menulistinvalid = 1;
  m_menulist.setAutoSort(0);
  m_menulist.addItem(this);
}

PopupMenu::PopupMenu(PopupMenu *_parent, int commandcallbackid) {
  m_menuccid = commandcallbackid;
  hmenu = NULL;
  parent = _parent->getParent();
  m_menulistinvalid = 1;
  m_menulist.setAutoSort(0);
  m_menulist.addItem(this);
}

PopupMenu::~PopupMenu() {
  invalidateMenu();
  entries.deleteAll();
  sortedentries.removeAll();
  m_menulist.removeItem(this);
}

void PopupMenu::addSubMenu(PopupMenu *menu, const char *text, int disabled) {
  ASSERT(menu != this);
  invalidateMenu();
  PopupMenuEntry *e = new PopupMenuEntry(text, -1, menu, 0, disabled);
  entries.addItem(e);
  sortedentries.addItem(e);
}

void PopupMenu::addSubMenuCallback(const char *text, PopupMenuCallback *cb, int param) {
  invalidateMenu();
  PopupMenuEntry *e = new PopupMenuEntry(text, -1, NULL, 0, 0, cb, param);
  entries.addItem(e);
  sortedentries.addItem(e);
}

void PopupMenu::addCommand(const char *text, int command, int checked, int disabled, int addpos) {
  invalidateMenu();
  PopupMenuEntry *e = new PopupMenuEntry(text, command, NULL, checked, disabled);
  entries.addItem(e, addpos);
  sortedentries.addItem(e);
}

void PopupMenu::addSeparator(int addpos) {
  invalidateMenu();
  PopupMenuEntry *e = new PopupMenuEntry(NULL, -1, NULL, 0, 0);
  entries.addItem(e, addpos);
  sortedentries.addItem(e);
};

void PopupMenu::checkCommand(int cmd, int check) {
  invalidateMenu();
  PopupMenuEntry *e = sortedentries.findItem((const char *)&cmd);
  if (e) e->setChecked(check);
}

void PopupMenu::disableCommand(int cmd, int disable) {
  invalidateMenu();
  PopupMenuEntry *e = sortedentries.findItem((const char *)&cmd);
  if (e) e->setDisabled(disable);
}

static LRESULT CALLBACK dummyMenuWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int PopupMenu::popAtXY(int x, int y) {
  rebuildMenu();
#ifdef WASABI_API_WND
  return TrackPopupMenu(getOSMenuHandle(), TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON |TPM_RIGHTBUTTON, x, y, 0, (parent ? parent->gethWnd() : WASABI_API_WND->main_getRootWnd()->gethWnd()), NULL)-1;
#else
  HWND dummy = StdWnd::createWnd(0, 0, 1, 1, TRUE, FALSE, NULL, WASABI_API_APP->main_gethInstance(), "DUMMY", dummyMenuWndProc);
  int r = TrackPopupMenu(getOSMenuHandle(), TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON |TPM_RIGHTBUTTON, x, y, 0, dummy, NULL)-1;
  StdWnd::destroyWnd(dummy);
  return r;
#endif
}

int PopupMenu::popAnchored(int type) {	// dropped off the sourceWnd given above
  int flag = 0;
  switch (type) {
    case POPUP_ANCHOR_UL: flag |= TPM_LEFTALIGN | TPM_TOPALIGN; break;
    case POPUP_ANCHOR_LL: flag |= TPM_LEFTALIGN | TPM_BOTTOMALIGN; break;
    case POPUP_ANCHOR_UR: flag |= TPM_RIGHTALIGN | TPM_TOPALIGN; break;
    case POPUP_ANCHOR_LR: flag |= TPM_RIGHTALIGN | TPM_BOTTOMALIGN; break;
  }
  rebuildMenu();
  int x, y;
  StdWnd::getMousePos(&x, &y);
#ifdef WASABI_API_WND
  return TrackPopupMenu(getOSMenuHandle(), flag | TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON |TPM_RIGHTBUTTON, x, y, 0, (parent ? parent->gethWnd() : WASABI_API_WND->main_getRootWnd()->gethWnd()), NULL)-1;
#else
  HWND dummy = StdWnd::createWnd(0, 0, 1, 1, TRUE, FALSE, NULL, WASABI_API_APP->main_gethInstance(), "DUMMY", dummyMenuWndProc);
  int r = TrackPopupMenu(getOSMenuHandle(), flag | TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON |TPM_RIGHTBUTTON, x, y, 0, dummy, NULL)-1;
  StdWnd::destroyWnd(dummy);
  return r;
#endif
}

int PopupMenu::popAtMouse() {
  rebuildMenu();
  int x, y;
  StdWnd::getMousePos(&x, &y);
#ifdef WASABI_API_WND
  return TrackPopupMenu(getOSMenuHandle(), TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON |TPM_RIGHTBUTTON, x, y, 0, (parent ? parent->gethWnd() : WASABI_API_WND->main_getRootWnd()->gethWnd()), NULL)-1;
#else
  HWND dummy = StdWnd::createWnd(0, 0, 1, 1, TRUE, FALSE, NULL, WASABI_API_APP->main_gethInstance(), "DUMMY", dummyMenuWndProc);
  int r = TrackPopupMenu(getOSMenuHandle(), TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON |TPM_RIGHTBUTTON, x, y, 0, dummy, NULL)-1;
  StdWnd::destroyWnd(dummy);
  return r;
#endif
}

int PopupMenu::getNumCommands() {
  return entries.getNumItems();
}

const char *PopupMenu::getCommandText(int command) {
  PopupMenuEntry *e = sortedentries.findItem((const char *)&command);
  if (e) return e->getText();
  return NULL;
}

OSMENUHANDLE PopupMenu::getOSMenuHandle() {
  rebuildMenu();
  return hmenu;
}

void PopupMenu::rebuildMenu() {
#ifdef WIN32
  if (hmenu != NULL) return;
  hmenu = CreatePopupMenu();
  int i = 0;
  foreach(entries)
    PopupMenuEntry *e = entries.getfor();
    OSMENUHANDLE submenu = NULL;
    if (e->getCallback()) {
      PopupMenu *m = e->getCallback()->popupMenuCallback(this, e->getCallbackParam());
      if (m) submenu = m->getOSMenuHandle(); 
    } else if (e->isSubmenu()) {
      submenu = e->getSubmenu()->getOSMenuHandle();
    }
    InsertMenu(hmenu, i++, MF_BYPOSITION | (e->getChecked() ? MF_CHECKED : MF_UNCHECKED) | (e->getDisabled() ? MF_GRAYED: 0) | (e->isSeparator() ? MF_SEPARATOR : (e->isSubmenu() ? MF_POPUP : 0) | (e->getText() ? MF_STRING : 0)), e->isSubmenu() ? (int)submenu : e->getCommand()+1, e->getText());
  endfor;
#else
port me!
#endif
  m_menulistinvalid = 1;
}

void PopupMenu::invalidateMenu() {
#ifdef WIN32
  if (hmenu) DestroyMenu(hmenu);
#else
port me!
#endif
  hmenu = NULL;
  m_menulistinvalid = 1;
}

void PopupMenu::abort() {
  #ifdef WIN32

#ifdef WASABI_API_WND
  HWND w = (parent ? parent->gethWnd() : WASABI_API_WND->main_getRootWnd()->gethWnd());
#else
  HWND w = GetDesktopWindow();
#endif

  PostMessage(w, WM_LBUTTONDOWN, 0, 0xdeadc0de);
  PostMessage(w, WM_LBUTTONUP, 0, 0xdeadc0de);

  #else
  
  #error port me! 
  
  #endif
}

PopupMenu *PopupMenu::findMenuByOSHandle(OSMENUHANDLE handle) { 
  m_menulist.sort(1);
  return m_menulist.findItem((const char *)&handle); 
}

int PopupMenuSortByOSHandle::compareItem(PopupMenu *p1, PopupMenu* p2) {
  return CMP3(p1->getOSMenuHandle(), p2->getOSMenuHandle());
}

int PopupMenuSortByOSHandle::compareAttrib(const char *attrib, PopupMenu *item) {
  return CMP3(*(OSMENUHANDLE *)attrib, item->getOSMenuHandle());
}

PtrListQuickSorted<PopupMenu, PopupMenuSortByOSHandle> PopupMenu::m_menulist;

