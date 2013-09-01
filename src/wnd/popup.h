// PopupMenu NONPORTABLE, NewPopupMenu portable
#ifndef _POPUP_H
#define _POPUP_H

#include <wasabicfg.h>

class PopupMenu;

class PopupMenuCallback {
public:
  virtual PopupMenu *popupMenuCallback(PopupMenu *parent, int param)=0; // returns the new popupmenu to be displayed
};

enum { POPUP_ANCHOR_UL, POPUP_ANCHOR_LL, POPUP_ANCHOR_UR, POPUP_ANCHOR_LR };

#ifdef WIN32
#define OSMENUHANDLE HMENU
#else
#error port me!
#endif

class PopupMenuEntry {
  public:
    PopupMenuEntry(const char *txt, int cmd, PopupMenu *_submenu, int _checked, int _disabled, PopupMenuCallback *cb=NULL, int cbparam=0) : text(txt), command(cmd), submenu(_submenu), checked(_checked), disabled(_disabled), callback(cb), callbackparam(cbparam) {} // txt = null for separators and menus, cmd = -1 for separators and submenus, if submenu == null && cmd == -1 and text == NULL then it's a separator
    virtual ~PopupMenuEntry() {}
    int getCommand() { return command; }
    const char *getText() { return text; }
    PopupMenu *getSubmenu() { return submenu; }
    int getChecked() { return checked; }
    int getDisabled() { return disabled; }
    int isSeparator() { return command == -1 && submenu == NULL && text == NULL; }
    int isSubmenu() { return (command == -1 && submenu != NULL) || callback; }
    void setChecked(int check) { checked = check; }
    void setDisabled(int disable) { disabled = disable; }
    PopupMenuCallback *getCallback() { return callback; }
    int getCallbackParam() { return callbackparam; }
  private:
    String text;
    int command;
    PopupMenu *submenu;
    int checked;
    int disabled;
    PopupMenuCallback *callback;
    int callbackparam;
};

class SortMenuEntries {
public:
  static int compareItem(PopupMenuEntry *p1, PopupMenuEntry *p2) {
   if (p1->getCommand() < p2->getCommand()) return -1;
   if (p1->getCommand() > p2->getCommand()) return 1;
   return 0;
  }
  static int compareAttrib(const char *attrib, PopupMenuEntry *item) {
   int c = *(int *)attrib;
   if (c < item->getCommand()) return -1;
   if (c > item->getCommand()) return 1;
   return 0;
  }
};

class PopupMenuSortByOSHandle {
public:
  static int compareItem(PopupMenu *p1, PopupMenu* p2);
  static int compareAttrib(const char *attrib, PopupMenu *item);
};

class PopupMenu {
  public:
    PopupMenu(int commandcallbackid=-1);
    PopupMenu(RootWnd *parent, int commandcallbackid=-1);
    PopupMenu(PopupMenu *parent, int commandcallbackid=-1);
    ~PopupMenu();

    virtual void addSubMenu(PopupMenu *menu, const char *text, int disabled=FALSE);
    virtual void addSubMenuCallback(const char *text, PopupMenuCallback *cb, int param);
    virtual void addCommand(const char *text, int command=-1, int checked=0, int disabled=0, int addpos=-1);
    virtual void addSeparator(int addpos=-1);
    virtual void checkCommand(int cmd, int check);
    virtual void disableCommand(int cmd, int disable);
    virtual int popAtXY(int x, int y);
    virtual int popAnchored(int type = POPUP_ANCHOR_LL);	// dropped off the sourceWnd given above
    virtual int popAtMouse();
    virtual int getNumCommands();
    virtual const char *getCommandText(int command);
    OSMENUHANDLE getOSMenuHandle();
    virtual void abort();
    static PopupMenu *findMenuByOSHandle(OSMENUHANDLE handle);

    // these are used by commandcallbacks
    void setMenuCommandCallbackId(int id) { m_menuccid = id; }
    int getMenuCommandCallbackId() { return m_menuccid; }

  private:
    void rebuildMenu();
    void invalidateMenu(); // no virtual please (it's called in the destructor)
    RootWnd *getParent() { return parent; }

    PtrList<PopupMenuEntry> entries;
    PtrListQuickSorted<PopupMenuEntry, SortMenuEntries> sortedentries;
    OSMENUHANDLE hmenu;
    RootWnd *parent;
    int m_menuccid;
    int m_menulistinvalid;
    static PtrListQuickSorted<PopupMenu, PopupMenuSortByOSHandle> m_menulist;
};

#endif
