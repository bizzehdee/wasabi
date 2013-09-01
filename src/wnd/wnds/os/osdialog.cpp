#include "precomp.h"

#include <windows.h>
#include <commctrl.h>

#include <bfc/std_wnd.h>
#include <bfc/tlist.h>

#ifdef WASABI_COMPILE_CONFIG
#include <api/config/api_config.h>
#endif

#include "osdialog.h"

TList<OSWINDOWHANDLE> dlglist;

class OSDlgAttrSlot {
public:
  OSDlgAttrSlot(Attribute *a, int d, int ad=0) : attr(a), dlgid(d), autodel(ad) { }
  ~OSDlgAttrSlot() { if (autodel) delete attr; }
  Attribute *attr;
  int dlgid;
  int autodel;
};

class OSDlgDisableSlot {
public:
  OSDlgDisableSlot(Attribute *a, int dlgid, int _disable_value=FALSE) :
    attrslot(a, dlgid), disable_value(_disable_value) { }

  OSDlgAttrSlot attrslot;
  int disable_value;
};

class OSDlgTabSlot {
public:
  OSDlgTabSlot(OSDialog *osd, const char *n) : dlg(osd), name(n) {
    //CUT ASSERT(dlg != NULL);
    ZERO(tcitem);
    tcitem.mask = TCIF_TEXT;
    tcitem.pszText = const_cast<char*>(name.v());
  }
  ~OSDlgTabSlot() {
    delete dlg;
  }

  String name;
  OSDialog *dlg;
  TCITEM tcitem;
};

class OSDlgSizeBindSlot {
public:
  OSDlgSizeBindSlot(int _dlgid, unsigned long _side) : dlgid(_dlgid), side(_side) {
    x = y = 0;
    ZERO(prevr);
  }
  ~OSDlgSizeBindSlot() { }
 
  int dlgid;
  RECT prevr;
  unsigned long side;
  int x, y;
};

static BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
   if (uMsg == WM_INITDIALOG)
     SetWindowLong(hwnd, GWL_USERDATA, lParam);
  OSDialog *cfgdlg = (OSDialog*)GetWindowLong(hwnd, GWL_USERDATA);
  if (cfgdlg == NULL) return 0;
  return cfgdlg->dlgProc(hwnd, uMsg, wParam, lParam);
}

OSDialog::OSDialog(int id, HWND par, OSMODULEHANDLE handle) : cfgdlgid(id), parent(par), m_handle(handle) {
  modal = -1;
  myhwnd = NULL;
  ignore_change = 0;
  tab_id = -1;
  cur_tab_dlg = NULL;
  tab_parent = NULL;
  ZERO(origr);
  initial_focus = -1;

  // for tab control
  INITCOMMONCONTROLSEX icce = { sizeof(INITCOMMONCONTROLSEX), ICC_WIN95_CLASSES };
  InitCommonControlsEx(&icce); 
}

OSDialog::~OSDialog() {
  SetWindowLong(myhwnd, GWL_USERDATA, 0);//CUT
  if (modal == 1)
    destroyModal();
  else if (modal == 0)
    destroyModeless();
  modal = -1;
  slots.deleteAll();
  disable_slots.deleteAll();
  tab_slots.deleteAll();
  sizebind_slots.deleteAll();

  combostash.deleteAll();
  combosel.deleteAll();
  sliderstash.deleteAll();
}

void OSDialog::registerAttribute(Attribute &attr, int dlgid, int autodel) {
  registerAttribute(&attr, dlgid, autodel);
}

Attribute *OSDialog::registerAttribute(Attribute *attr, int dlgid, int autodel) {
  slots.addItem(new OSDlgAttrSlot(attr, dlgid, autodel));
  viewer_addViewItem(attr);	// dup checks for us
  if (myhwnd != NULL) applyAttrToDlg(attr);
  return attr;
}

void OSDialog::registerBoolDisable(Attribute &attr, int dlgid, int disable_val) {
  disable_slots.addItem(new OSDlgDisableSlot(&attr, dlgid, disable_val));
  viewer_addViewItem(&attr);	// dup checks for us
  if (myhwnd != NULL) {
    // process disables here
    applyDisablers(&attr);
  }
}

void OSDialog::populateCombo(int dlgid, const char *text, unsigned int userdata) {
  HWND ctrl = GetDlgItem(myhwnd, dlgid);
  if (myhwnd == NULL || ctrl == NULL) {	// stash it buddy
    combostash.addItem(new comboslot(dlgid, String(text), userdata));
  } else {
    int pos = SendMessage(ctrl, CB_ADDSTRING, 0, (LPARAM)text);
    SendMessage(ctrl, CB_SETITEMDATA, pos, userdata);
  }
}

void OSDialog::setComboSel(int dlgid, int pos) {
  HWND ctrl = GetDlgItem(myhwnd, dlgid);
  if (myhwnd == NULL || ctrl == NULL) {	// stash it buddy
    combosel.addItem(new comboselslot(dlgid, pos));
  } else {
    SendMessage(ctrl, CB_SETCURSEL, pos, 0);
  }
}

void OSDialog::resetCombo(int dlgid) {
  HWND ctrl = GetDlgItem(myhwnd, dlgid);
  if (myhwnd == NULL || ctrl == NULL) {	// stash it buddy
//FUCKO: delete combostash entries
  } else {
    SendMessage(ctrl, CB_RESETCONTENT, 0, 0);
  }
}

void OSDialog::setSliderRange(int dlgid, int low, int high) {
  HWND ctrl = GetDlgItem(myhwnd, dlgid);
  low = MINMAX(low, 0, 65535);
  high = MINMAX(high, 0, 65535);
  if (myhwnd == NULL || ctrl == NULL) {	// stash it buddy
    sliderstash.addItem(new sliderslot(dlgid, low, high));
  } else {
    SendMessage(ctrl, TBM_SETRANGE, TRUE, MAKELONG(low, high));
  }
}

//latervoid OSDialog::addSliderTic(int dlgid, int ticpos) {
//later  HWND ctrl = GetDlgItem(myhwnd, dlgid);
//later}

void OSDialog::addTab(OSDialog *osd, const char *name) {
  ASSERTPR(myhwnd == NULL, "have to add tabs before create()");
  ASSERTPR(tab_id >= 0, "have to set tab control id before addTab()");
  if (osd) osd->tab_parent = this;
  tab_slots.addItem(new OSDlgTabSlot(osd, name));
}

void OSDialog::setTabName(int pos, const char *name) {
  OSDlgTabSlot *slot = tab_slots[pos];
  if (slot == NULL) return;
  slot->name = name;
  if (myhwnd) {
    if (tab_id < 0) {
      DebugString("Warning: setTabName() called before setTabId()");
    }
    HWND ctrl = GetDlgItem(myhwnd, tab_id);
    TCITEM tcitem;
    ZERO(tcitem);
    tcitem.mask = TCIF_TEXT;
    tcitem.pszText = const_cast<char*>(slot->name.v());
    SendMessage(ctrl, TCM_SETITEM, pos, (long)&tcitem);
  }
}

void OSDialog::setTabId(int ntab_id) {
  ASSERT(ntab_id >= 0);
  ASSERTPR(myhwnd == NULL, "do tab stuff before create()");
  tab_id = ntab_id;
}

int OSDialog::getTabId() {
  return tab_id;
}

void OSDialog::addSizeBinding(int dlgid, unsigned long side) {
//CUT  ASSERTPR(myhwnd == NULL, "do sizebind stuff before create()");
  OSDlgSizeBindSlot *sb = new OSDlgSizeBindSlot(dlgid, side);
  if (myhwnd != NULL) {
    RECT r;
    GetWindowRect(myhwnd, &r);
    // stash the original positions of control (dup of code in WM_INITDIALOG)
    HWND ctrl = GetDlgItem(myhwnd, sb->dlgid);
    if (ctrl != NULL) {
      GetWindowRect(ctrl, &sb->prevr);
      // put control rect into parent dlg coords
      StdWnd::screenToClient(myhwnd, &sb->prevr);
    }
  }
  sizebind_slots.addItem(sb);
}

void OSDialog::setParent(HWND par) {
  parent = par;
}

void OSDialog::setFocusedControl(int dlgid) {
  if (myhwnd == NULL) {
    initial_focus = dlgid;
    return;
  }
  HWND ctrl = GetDlgItem(myhwnd, dlgid);
  if (ctrl != 0) StdWnd::setFocus(ctrl);
}

void OSDialog::createModeless(int aot, int showing) {
  if (myhwnd != NULL) return;

  modal = 0;

  // modeless eh
  myhwnd = CreateDialogParam(m_handle != NULL ? m_handle : WASABI_API_APP->main_gethInstance(), MAKEINTRESOURCE(cfgdlgid), parent, DlgProc, (LPARAM)this);
  ASSERT(myhwnd != NULL);

  dlglist.addItem(myhwnd);

  // apply always-on-top flag
  if (aot)
    SetWindowPos(myhwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

  if (showing) StdWnd::showWnd(getOsWindowHandle());
}

void OSDialog::destroyModeless() {
  onDestroy();
  ASSERT(modal == 0);
  HWND tmp_wnd = myhwnd; myhwnd = NULL;

  dlglist.removeItem(tmp_wnd);

  if (tmp_wnd) DestroyWindow(tmp_wnd);

  modal = -1;
}

int OSDialog::createModal() {
  modal = 1;
  int r = DialogBoxParam(m_handle != NULL ? m_handle : WASABI_API_APP->main_gethInstance(), MAKEINTRESOURCE(cfgdlgid), parent, DlgProc, (LPARAM)this);
  return r;
}

void OSDialog::destroyModal(int retcode) {
  onDestroy();
  ASSERT(modal == 1);
  if (myhwnd) EndDialog(getOsWindowHandle(), retcode);
  myhwnd = NULL;
  modal = -1;
}

void OSDialog::_onCreate() {
  foreach(combostash)
    comboslot *s = combostash.getfor();
    populateCombo(s->a, s->b, s->c);
  endfor

  foreach(combosel)
    comboselslot *s = combosel.getfor();
    setComboSel(s->a, s->b);
  endfor

  foreach(sliderstash)
    sliderslot *s = sliderstash.getfor();
    setSliderRange(s->a, s->b, s->c);
  endfor

  if (initial_focus >= 0) setFocusedControl(initial_focus);	// apply inital focus item

  onCreate();
}

int OSDialog::getDlgId() {
  return cfgdlgid;
}

int OSDialog::dlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_INITDIALOG: {
      ASSERT(myhwnd == NULL);
      if (myhwnd == NULL) myhwnd = hwnd;

      applyAllAttrsToDlg();

      // stash original size of dialog
      // dup of code in addSizeBinding()
      GetClientRect(myhwnd, &origr);
      RECT r;
      GetWindowRect(myhwnd, &r);
      // stash the original positions of controls
      foreach(sizebind_slots)
        OSDlgSizeBindSlot *sb = sizebind_slots.getfor();
        HWND ctrl = GetDlgItem(myhwnd, sb->dlgid);
        if (ctrl == NULL) continue;
        GetWindowRect(ctrl, &sb->prevr);

        // put control rect into parent dlg coords
        StdWnd::screenToClient(myhwnd, &sb->prevr);
      endfor

      // insert all the tabs if we have a tab control
      if (tab_id >= 0) {
        HWND tab_ctrl = GetDlgItem(myhwnd, tab_id);
        if (tab_ctrl) {
          foreach(tab_slots)
            SendMessage(tab_ctrl, TCM_INSERTITEM, (WPARAM)foreach_index, (LPARAM)&tab_slots.getfor()->tcitem);
          endfor
        }
      }

      _onCreate();

      if (tab_id >= 0) {
        int ctab = 0;

        // restore which tab was open
#ifdef WASABI_COMPILE_CONFIG
        String myname = getName();
        if (myname.notempty()) {
          ctab = WASABI_API_CONFIG->getIntPrivate(StringPrintf("last tab/%s", myname.v()), 0);
        }
#endif

        // this just selects the tab, it doesn't cause a switchToTab
        HWND tab_ctrl = GetDlgItem(myhwnd, tab_id);
        SendMessage(tab_ctrl, TCM_SETCURSEL, ctab, 0);

        // so we do it here
        switchToTab(ctab);
      }

      _onDlgResize();

      return TRUE;
    }
    break;

    case WM_CLOSE:
      if (modal)
        destroyModal(FALSE);
      else
        onUserClose();
    break;

    case WM_SETCURSOR:
      SetCursor(LoadCursor(NULL, IDC_ARROW));
    return TRUE;

    case WM_SIZE:
      _onDlgResize();
    break;

    case WM_COMMAND: {
      int code = (int)HIWORD(wParam);
      int dlgid = (int)LOWORD(wParam);

      char classname[1024];
      GetClassName((HWND)lParam, classname, 1023);
      classname[1023] = '\0';
      if (STRCASEEQL(classname, "STATIC")) {
        switch (code) {
          case STN_DBLCLK:
            onUserDblClick(dlgid);
          break;
        }
      } else {
        OSDlgAttrSlot *slot = slotFromId(dlgid);
        if (slot != NULL) switch (code) {
          case CBN_SELCHANGE: {
//DebugString("SEL CHANGE!");
            HWND ctrl = GetDlgItem(hwnd, dlgid);
            int pos = SendMessage(ctrl, CB_GETCURSEL, 0, 0);
//DebugString("new pos %d", pos);
            char tmp[WA_MAX_PATH]="";
            SendMessage(ctrl, CB_GETLBTEXT, pos, (LPARAM)tmp);
            applyDlgToAttr(slot->attr, tmp);
          }
          break;
          case BN_CLICKED:
          case EN_CHANGE:
          case CBN_EDITCHANGE:
            applyDlgToAttr(slot->attr);
//CUT DebugString("pusho %d", slot->attr->getValueAsInt());
          break;
        }
        if (code == BN_CLICKED) {
          if (tab_parent) tab_parent->onUserButton(dlgid);
          onUserButton(dlgid);	// send last because we might be deleted here
          if ((dlgid == IDOK || dlgid == IDCANCEL) && modal == 1) {
            destroyModal(dlgid/*CUT wtf == IDOK*/);
          }
        }
      }//not STATIC
    }
    break;

    case WM_NOTIFY: {
      NMHDR *nm = (NMHDR *)lParam;
      HWND ctrl = nm->hwndFrom;
      char classname[1024];
      GetClassName(ctrl, classname, 1023);
      int ctrlid = nm->idFrom;

      if (STRCASEEQL(classname, "SysTabControl32")) {
        switch (nm->code) {
          case TCN_SELCHANGE: {	// time to show a tab
            int which = SendMessage(ctrl, TCM_GETCURSEL, 0, 0);
            switchToTab(which);
          }
          break;
        }
      } else if (STRCASEEQL(classname, TRACKBAR_CLASS)) {
        switch (nm->code) {
          case NM_RELEASEDCAPTURE: {
            OSDlgAttrSlot *slot = slotFromId(ctrlid);
            if(slot) applyDlgToAttr(slot->attr);
          }
          break;
        }
      }
    }
    break;

    // sliders
    case WM_VSCROLL:
    case WM_HSCROLL: {
      int ctrlid = GetDlgCtrlID((HWND)lParam);
      switch (LOWORD(wParam)) {
        case TB_THUMBTRACK:
          int pos = HIWORD(wParam);
          OSDlgAttrSlot *slot = slotFromId(ctrlid);
          if(slot) applyDlgToAttr(slot->attr);
//DebugString("tracking! %d for %d", pos, ctrl);
        break;
      }
    }
    break;

    case WM_PARENTNOTIFY: {
      switch (LOWORD(wParam)) {
        case WM_LBUTTONDOWN:
          onUserButton(HIWORD(wParam));
        return 0;
      }
    }
    break;

    case WM_CONTEXTMENU:
      onDlgContextMenu();
    break;

    default:
      return 0;	// in dialog box, means, didn't handle it
  }
  return 1;
}

OSWINDOWHANDLE OSDialog::getOsWindowHandle() {
  return myhwnd;
}

RECT OSDialog::clientRect() {
  RECT ret;
  StdWnd::getClientRect(getOsWindowHandle(), &ret);
  return ret;
}

int OSDialog::viewer_onEvent(Attribute *item, int event, int param2, void *ptr, int ptrlen) {
  switch (event) {
    case Attribute::Event_DATACHANGE:
      if (ignore_change > 0) {
        ignore_change--;
        return 0;
      }
      applyAttrToDlg(item);
    break;
  }
  return 1;
}

OSDialog *OSDialog::getCurTab() {
  return cur_tab_dlg;
}

int OSDialog::numAttributes() {
  return slots.getNumItems();
}

Attribute *OSDialog::enumAttribute(int n) {
  OSDlgAttrSlot *slot = slots[n];
  if (slot == NULL) return NULL;
  return slot->attr;
}

OSDlgAttrSlot *OSDialog::slotFromId(int dlgid) {
  OSDlgAttrSlot *slot = NULL;
  foreach(slots)
    if (slots.getfor()->dlgid == dlgid) {
      slot = slots.getfor();
      break;
    }
  endfor
  return slot;
}

void OSDialog::resizeToRect(const RECT *r) {
  ASSERT(r != NULL);
  if (myhwnd != NULL) {
    StdWnd::setWndPos(myhwnd, NULL,
      r->left, r->top, r->right - r->left, r->bottom - r->top,
      TRUE, TRUE, FALSE, FALSE, FALSE);
  }
}

void OSDialog::moveTo(int x, int y) {
  if (myhwnd != NULL) {
    StdWnd::setWndPos(myhwnd, NULL,
      x, y, 0, 0,
      TRUE, TRUE, FALSE, FALSE, TRUE);
  }
}

void OSDialog::enableControl(int id, int enabled) {
  if (myhwnd == NULL) return;
  HWND ctrl = GetDlgItem(myhwnd, id);
  if (ctrl) EnableWindow(ctrl, !!enabled);
}

void OSDialog::setControlText(int id, const char *text) {
  if (myhwnd == NULL) return;
  HWND ctrl = GetDlgItem(myhwnd, id);
  if (ctrl) SetWindowText(ctrl, text);
}

void OSDialog::applyAttrToDlg(Attribute *item) {
  if (myhwnd == NULL) return;	// dying

  OSDlgAttrSlot *slot = NULL;
  foreach(slots)
    if (slots.getfor()->attr == item) slot = slots.getfor();
  endfor
//DebugString("** slot is %x", slot);
  if (slot == NULL) return;

//DebugString("** is %d", slot->dlgid);
  ASSERT(myhwnd != NULL);
  HWND ctrl = GetDlgItem(myhwnd, slot->dlgid);
  if (ctrl == NULL) return;

  char bufo[4096]="";
  slot->attr->getData(bufo, 4096);
  bufo[4095] = 0;
  String val(bufo);

  // allow descendents to reformat the data on the way out
  formatDataAttribToDlg(slot->dlgid, val);
  if (tab_parent) tab_parent->formatDataAttribToDlg(slot->dlgid, val);

//DebugString("** got control handle");
  // figure out what kind of thingy it is
  char buf[1024];
  GetClassName(ctrl, buf, 1023);
  buf[1023] = '\0';
//DebugString("** class is %s", buf);
  if (STRCASEEQL(buf, "BUTTON")) {
    unsigned long style = GetWindowLong(ctrl, GWL_STYLE);
//DebugString("** style is %x", style);
//DebugString("** val is %d", !!slot->attr->getValueAsInt());
    if (style & BS_CHECKBOX) {
      SendDlgItemMessage(myhwnd, slot->dlgid, BM_SETCHECK, !!ATOI(val), 0);
//DebugString("** it's a checkbox");
    }
  } else if (STRCASEEQL(buf, "EDIT")) {
    SetWindowText(ctrl, val.vs());
  } else if (STRCASEEQL(buf, "STATIC")) {
    SetWindowText(ctrl, val.vs());
  } else if (STRCASEEQL(buf, "COMBOBOX")) {	// set text
    SetWindowText(ctrl, val.vs());
  } else if (STRCASEEQL(buf, TRACKBAR_CLASS)) {	// set slider pos
    int pos = ATOI(val.vs());
    SendMessage(ctrl, TBM_SETPOS, TRUE, pos);
  } else if (STRCASEEQL(buf, PROGRESS_CLASS)) {	// set slider pos
    int pos = ATOI(val.vs());
    SendMessage(ctrl, PBM_SETPOS, pos, 0);
  }

  applyDisablers(item);
}

void OSDialog::applyDlgToAttr(Attribute *attr, const char *val) {
  onPreApplyDlgToAttr(attr, val);
  if (tab_parent) tab_parent->onPreApplyDlgToAttr(attr, val);

  OSDlgAttrSlot *slot = NULL;
  foreach(slots)
    if (slots.getfor()->attr == attr) slot = slots.getfor();
  endfor
//DebugString("** slot is %x", slot);
  if (slot == NULL) return;

//CUT  Attribute *attr = slot->attr;

//DebugString("** is %d", slot->dlgid);
ASSERT(myhwnd != NULL);
  HWND ctrl = GetDlgItem(myhwnd, slot->dlgid);
  if (ctrl == NULL) return;

//DebugString("** got control handle");
  // figure out what kind of thingy it is
  MemBlock<char> buf(1024);
  GetClassName(ctrl, buf, buf.getSizeInBytes());
  buf.terminate();
  if (STRCASEEQL(buf, "BUTTON")) {
    unsigned long style = GetWindowLong(ctrl, GWL_STYLE);
//DebugString("** style is %x", style);
    if (style & BS_CHECKBOX) {
      int b = (SendDlgItemMessage(myhwnd, slot->dlgid, BM_GETCHECK, 0, 0) == BST_CHECKED);
//DebugString("** it's a checkbox");
      ignore_change++;
      attr->setData(StringPrintf(b));
    }
  } else if (STRCASEEQL(buf, "EDIT")) {
    char bufo[1024], attrbuf[1024];
    GetWindowText(ctrl, bufo, 1023);
    bufo[1023] = '\0';
    attr->getData(attrbuf, 1024);
    attrbuf[1023] = '\0';
    if (!STREQL(bufo, attrbuf)) {	// make sure something really did change
      ignore_change++;	// disallow getting the dependency change -> dlg loop
      attr->setData(bufo);
    }
  } else if (STRCASEEQL(buf, "COMBOBOX")) {
//DebugString("** it's a COMBO");
    char bufo[1024], attrbuf[1024];
    if (val != NULL)
      STRCPY(bufo, val);
    else
      GetWindowText(ctrl, bufo, 1023);
    bufo[1023] = '\0';
//DebugString("** text %s", bufo);
    attr->getData(attrbuf, 1024);
    attrbuf[1023] = '\0';
//DebugString("** attr %s", attrbuf);
    if (!STREQL(bufo, attrbuf)) {	// make sure something really did change
      ignore_change++;	// disallow getting the dependency change -> dlg loop
      attr->setData(bufo);
//FUCKO: add a line to combobox
    }
  } else if (STRCASEEQL(buf, TRACKBAR_CLASS)) {
    ignore_change++;	// disallow getting the dependency change -> dlg loop
    attr->setData(val ? val : StringPrintf(SendMessage(ctrl, TBM_GETPOS, 0, 0)));
  }

  applyDisablers(attr);

  if (val == NULL) {
    attr->getData(buf, buf.getSizeInBytes());
    val = buf;
  }

  onPostApplyDlgToAttr(attr, val, slot->dlgid);
  if (tab_parent) tab_parent->onPostApplyDlgToAttr(attr, val, slot->dlgid);
}
      //// push data attrib->dlgbox
void OSDialog::applyAllAttrsToDlg() {
  foreach(slots)
    applyAttrToDlg(slots.getfor()->attr);
  endfor
}

void OSDialog::_onDlgResize() {
  RECT dlgcr;
  GetClientRect(getOsWindowHandle(), &dlgcr);
//      dlgcr.right -= dlgcr.left; dlgcr.left = 0;
//      dlgcr.top -= dlgcr.bottom; dlgcr.bottom = 0;
  if (dlgcr.right == dlgcr.left || dlgcr.bottom == dlgcr.top) return;// um
  int neww = dlgcr.right - dlgcr.left;
  int newh = dlgcr.bottom - dlgcr.top;

  HDWP hdwp = BeginDeferWindowPos(sizebind_slots.n());

  foreach(sizebind_slots)

    OSDlgSizeBindSlot *sb = sizebind_slots.getfor();
    HWND ctrl = GetDlgItem(getOsWindowHandle(), sb->dlgid);
    if (ctrl == NULL) continue;

    RECT cr = sb->prevr;

    if (sb->side & OSDialogSizeBind::RIGHTEDGETORIGHT) {
      cr.right = neww - (origr.right - sb->prevr.right);
    }

    if (sb->side & OSDialogSizeBind::LEFTEDGETORIGHT) {
      cr.left = neww - (origr.right - sb->prevr.left);
    }

    if (sb->side & OSDialogSizeBind::BOTTOMEDGETOBOTTOM) {
      cr.bottom = newh - (origr.bottom - sb->prevr.bottom);
    }

    if (sb->side & OSDialogSizeBind::TOPEDGETOBOTTOM) {
      cr.top = newh - (origr.bottom - sb->prevr.top);
    }

    if (sb->side & OSDialogSizeBind::VCENTER) {
      cr.top = ((dlgcr.bottom-dlgcr.top)-(sb->prevr.bottom - sb->prevr.top))/2;
      cr.bottom = cr.top + (sb->prevr.bottom - sb->prevr.top);
    }

#if 0//CUT
    StdWnd::setWndPos(ctrl, NULL,
                      cr.left, cr.top, cr.right-cr.left, cr.bottom-cr.top,
                      TRUE, TRUE, TRUE, FALSE, FALSE);
#else
    if (hdwp) hdwp = DeferWindowPos(hdwp, ctrl, NULL,
                      cr.left, cr.top, cr.right-cr.left, cr.bottom-cr.top,
                      FALSE);
#endif
  endfor

  if (hdwp) EndDeferWindowPos(hdwp);

  onDlgResize();	// call the user-overrideable callback
}

void OSDialog::onSetName() {
  SetWindowText(getOsWindowHandle(), getName());
}

void OSDialog::applyDisablers(Attribute *item) {
  foreach(disable_slots)
    OSDlgDisableSlot *s = disable_slots.getfor();
    if (s->attrslot.attr != item) continue;
    int dis = 0;
    if (item->getValueAsInt() == s->disable_value) dis = 1;
    HWND di = GetDlgItem(myhwnd, s->attrslot.dlgid);
    EnableWindow(di, !dis);
  endfor
}

void OSDialog::switchToTab(int tab_n) {
  if (tab_id < 0) return;

  OSDlgTabSlot *s = tab_slots[tab_n];
  if (s == NULL) return;

  HWND tab_ctrl = GetDlgItem(myhwnd, tab_id);

  if (cur_tab_dlg) cur_tab_dlg->destroyModeless();

  if (s->dlg) {
    // create the tab in place

// child tabs need to be children of the main dlg, not the tabsheet!!
// see: http://www.guyswithtowels.com/articles/2002-08-15-1600.html
// it's an OS bug --BU
//    s->dlg->setParent(ctrl);//NO!
    s->dlg->setParent(myhwnd);

    s->dlg->createModeless();
    cur_tab_dlg = s->dlg;
    OSWINDOWHANDLE td = s->dlg->getOsWindowHandle();
    RECT mainr;
    GetWindowRect(myhwnd, &mainr);

    RECT r;
    GetClientRect(tab_ctrl, &r);
    StdWnd::clientToScreen(tab_ctrl, &r);
    StdWnd::screenToClient(myhwnd, &r);

    SendMessage(tab_ctrl, TCM_ADJUSTRECT, FALSE, (LPARAM)&r);

    int w = r.right - r.left, h = r.bottom - r.top;
    int x = r.left;
    int y = r.top;

    StdWnd::setWndPos(td, 0, x, y, w, h, TRUE, TRUE, FALSE, FALSE, FALSE);
    StdWnd::showWnd(td, TRUE);
  }

  cur_tab_dlg = s->dlg;

  // notify
  onSwitchToTab(s->dlg, tab_n);

#ifdef WASABI_COMPILE_CONFIG
  // keep track of last tab chosen
  String myname = getName();
  if (myname.notempty()) {
    WASABI_API_CONFIG->setIntPrivate(StringPrintf("last tab/%s", myname.v()), tab_n);
  }
#endif
}

