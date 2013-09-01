#ifndef _OSDIALOG_H
#define _OSDIALOG_H

// wraps up the host OS's dialog box facility

#include <bfc/std.h>
#include <bfc/depend.h>
#include <bfc/named.h>
#include <bfc/attrib/attribute.h>
#include <bfc/pair.h>
#include <bfc/ptrlist.h>

class OSDlgAttrSlot;
class OSDlgDisableSlot;
class OSDlgTabSlot;
class OSDlgSizeBindSlot;

// usage
// OSDialog *dlg = new OSDialog(IDD_DIALOGID);
// dlg->setParent(mywnd);	// optional: use NULL or omit for a popup
// dlg->createModeless();	// or createModal(), whichever
// /* do stuff */
// dlg->destroyModeless();

class OSDialog : public DependentViewerT<Attribute>, public Named {
public:
  OSDialog(int cfgdlgid, HWND parent=NULL, OSMODULEHANDLE handle=NULL);
  virtual ~OSDialog();

  // auto-bind an Attribute's value to a control
  // supported: edit box, static text, check box, combo box (see populateCombo)
  void registerAttribute(Attribute &attr, int dlgid, int autodel=FALSE);
  Attribute *registerAttribute(Attribute *attr, int dlgid, int autodel=FALSE);

  // this disables (grays out) a specific dialog item by id when the
  // attribute == disable_value
  void registerBoolDisable(Attribute &attr, int dlgid, int disable_value=FALSE);

  // this adds a line to the combo box with the given id
  void populateCombo(int dlgid, const char *text, unsigned int userdata=0);
  void setComboSel(int dlgid, int pos);
  void resetCombo(int dlgid);	// clears it out, so you can reload it

  void setSliderRange(int dlgid, int minimum, int maximum);
//later  void addSliderTic(int dlgid, int ticpos);

  // tab support: (one tab control per dialog)
  // setTabId(IDC_ID_OF_TAB_CONTROL);
  // OSDialog *tab1 = new OSDialog(IDD_TAB1);
  // tab1->registerAttribute(some_attrib, IDC_CONTROL_FOR_ATTRIB);
  // addTab(tab1, "hi my name is tab #1");
  void setTabId(int tab_id);
  int getTabId();
  void addTab(OSDialog *osd, const char *name);
  void setTabName(int pos, const char *name);

  void addSizeBinding(int dlgid, unsigned long option);

  void setParent(HWND par);

  void setFocusedControl(int dlgid);

//creation/destruction
  // modeless
  void createModeless(int aot=FALSE, int showing=TRUE);
  void destroyModeless();
  
  // modal
  int createModal();
  void destroyModal(int retcode=0);

  virtual void onCreate() { }
  virtual void onDestroy() { }	// called just before oswnd destruction (WARNING may be called from destructor (if you don't destroyMod*() before destruction

  // the dlgid given in the constructor
  int getDlgId();
  
  OSWINDOWHANDLE getOsWindowHandle();
  RECT clientRect();
  void resizeToRect(const RECT *r);
  void resizeToRect(const RECT &r) { resizeToRect(&r); }
  void moveTo(int x, int y);

  void enableControl(int id, int enabled);
  void setControlText(int id, const char *text);

  virtual int dlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
  virtual void onPreApplyDlgToAttr(Attribute *attr, const char *newval) { }
  virtual void onPostApplyDlgToAttr(Attribute *attr, const char *newval, int dlgid) { }
  virtual void onUserClose() { } // override this to catch window closing (nonmodal only)
  virtual void onUserButton(int id) { } // override to catch button clicks

  virtual void onUserDblClick(int id) { } // override to catch dclicks on statics with SS_NOTIFY set

  virtual void onDlgContextMenu() { } // override to catch right clicks

  virtual void onDlgResize() { } // override to catch resizing

  // override this and do whatever you want to the incoming string
  // which comes from the attribute and then is applied to the dialog control
  // i.e. str = StringStrftime(ATOI(str)) to format a timestamp
  virtual void formatDataAttribToDlg(int id, String &str) { }

  virtual void onSwitchToTab(OSDialog *tab_dlg, int pos) { }

  OSDialog *getCurTab();

  int numAttributes();
  Attribute *enumAttribute(int n);

  OSDlgAttrSlot *slotFromId(int dlgid);

// the rest of these are used internally
  virtual int viewer_onEvent(Attribute *item, int event, int param2, void *ptr, int ptrlen);

  void applyAttrToDlg(Attribute *item);	//also does enablers
  void applyDlgToAttr(Attribute *item, const char *val=NULL);	//only combo uses val right now

  void applyAllAttrsToDlg();

//from Named
  virtual void onSetName();

private:
  void applyDisablers(Attribute *item);

  void switchToTab(int n);

  void _onCreate();
  void _onDlgResize();

  int cfgdlgid;
  int modal;
  HWND myhwnd;
  OSMODULEHANDLE m_handle;
  HWND parent;
  PtrList<OSDlgAttrSlot> slots;
  int ignore_change;

  int initial_focus;

  PtrList<OSDlgDisableSlot> disable_slots;

  int tab_id;	// id of tab control
  PtrList<OSDlgTabSlot> tab_slots;
  OSDialog *cur_tab_dlg;	// currently showing tab

  OSDialog *tab_parent;	// if we are a tab, this is our parent dlg

  RECT origr;
  PtrList<OSDlgSizeBindSlot> sizebind_slots;

  typedef Triple<int, String, unsigned int> comboslot;
  PtrList<comboslot> combostash;
  typedef Pair<int, int> comboselslot;
  PtrList<comboselslot> combosel;
  typedef Triple<int, int, int> sliderslot;
  PtrList<sliderslot> sliderstash;
};

namespace OSDialogSizeBind {
enum {	// bitfield
  VCENTER=2,
  LEFTEDGETORIGHT=4,
  RIGHTEDGETORIGHT=8,
  RIGHT=OSDialogSizeBind::LEFTEDGETORIGHT|OSDialogSizeBind::RIGHTEDGETORIGHT,
  TOPEDGETOBOTTOM=16,
  BOTTOMEDGETOBOTTOM=32,
  BOTTOM=OSDialogSizeBind::TOPEDGETOBOTTOM|OSDialogSizeBind::BOTTOMEDGETOBOTTOM,
};
};

#endif
