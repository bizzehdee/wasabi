#include "precomp.h"

#include "rootwnd.h"

// helper functions definitions
OSWINDOWHANDLE RootWnd::getOsWindowHandle() {
  return _call(GETOSWINDOWHANDLE, (OSWINDOWHANDLE)NULL);
}

OSMODULEHANDLE RootWnd::getOsModuleHandle() {
  return _call(GETOSMODULEHANDLE, (OSMODULEHANDLE)NULL);
}

void RootWnd::performBatchProcesses() {
  _voidcall(BATCHPROCESSES);
}

const char *RootWnd::getRootWndName() {
  return _call(GETROOTWNDNAME, (const char *)NULL);
}

const char *RootWnd::getId() {
  return _call(GETID, (const char *)NULL);
}

DragInterface *RootWnd::getDragInterface() {
  return _call(GETDRAGINTERFACE, (DragInterface*)0);
}

RootWnd *RootWnd::rootWndFromPoint(POINT *pt, int throughtest) {
  return _call(FROMPOINT, (RootWnd*)0, pt, throughtest);
}

void RootWnd::getWindowRect(RECT *r) {
  if (!_voidcall(GETWINDOWRECT, r)) Std::setRect(r, 0, 0, 0, 0);
}

int RootWnd::isVisible(int within) {
  return _call(ISVISIBLE, 0, within);
}

int RootWnd::getSkinId() {
  return _call(GETSKINID, 0);
}

int RootWnd::onMetricChange(int metricid, int param1, int param2) {
  return _call(ONMETRICCHANGE, 0, metricid, param1, param2);
}

int RootWnd::onPaint(CanvasBase *canvas, Region *r) {
  return _call(ONPAINT, 0, canvas, r);
}

int RootWnd::paintTree(CanvasBase *canvas, Region *r) {
  return _call(PAINTTREE, 0, canvas, r);
}

Canvas *RootWnd::getFrameBuffer() {
  return _call(GETFRAMEBUFFER, (Canvas *)NULL);
}

void RootWnd::commitFrameBuffer(Canvas *canvas, RECT *r, double ratio) {
  _voidcall(COMMITFRAMEBUFFER, canvas, r, ratio);
}

int RootWnd::paint(Canvas *canvas, Region *r) {
  return _call(PAINT, 0, canvas, r);
}

RootWnd *RootWnd::getParent() {
  return _call(GETPARENT, (RootWnd *)0);
}

RootWnd *RootWnd::getRootParent() {
  return _call(GETROOTPARENT, (RootWnd *)0);
}

RootWnd *RootWnd::getDesktopParent() {
  return _call(GETDESKTOPPARENT, (RootWnd *)0);
}

void RootWnd::setParent(RootWnd *parent) {
  _voidcall(SETPARENT, parent);
}

void RootWnd::postDeferredCallback(int p1, int p2/* =0 */, int mindelay/* =0 */) {
  _voidcall(POSTDEFERREDCALLBACK, p1, p2, mindelay);
}

int RootWnd::onDeferredCallback(int param1, int param2) {
  return _call(ONDEFERREDCALLBACK, 0, param1, param2);
}

int RootWnd::childNotify(RootWnd *child, int msg, int p1, int p2) {
  return _call(CHILDNOTIFY, 0, child, msg, p1, p2);
}

void RootWnd::broadcastNotify(RootWnd *wnd, int msg, int p1, int p2) {
  _voidcall(BROADCASTNOTIFY, 0, wnd, msg, p1, p2);
}

int RootWnd::getPreferences(int what) {
  return _call(GETPREFERENCES, 0, what);
}

void RootWnd::setPreferences(int what, int v) {
  _voidcall(SETPREFERENCES, 0, what, v);
}

int RootWnd::beginCapture() {
  return _call(BEGINCAPTURE, 0);
}

int RootWnd::endCapture() {
  return _call(ENDCAPTURE, 0);
}

int RootWnd::getCapture() {
  return _call(GETCAPTURE, 0);
}

void RootWnd::cancelCapture() {
  _voidcall(CANCELCAPTURE, 0);
}

void RootWnd::onCancelCapture() {
  _voidcall(ONCANCELCAPTURE, 0);
}

void RootWnd::setAllowCapture(int allow) {
  _voidcall(SETALLOWCAPTURE, allow);
}

int RootWnd::isCaptureAllowed() {
  return _call(ISCAPTUREALLOWED, 1);
}

int RootWnd::isClickThrough() {
  return _call(CLICKTHROUGH, 0);
}

int RootWnd::isDndThrough() {
  return _call(DNDTHROUGH, 0);
}

void RootWnd::setClickThrough(int ct) {
  _voidcall(SETCLICKTHROUGH, ct);
}

RootWnd *RootWnd::getForwardWnd() {
  return _call(GETFORWARDWND, this);
}

void RootWnd::clientToScreen(int *x, int *y) {
  _voidcall(CLIENTSCREEN, x, y);
}

void RootWnd::screenToClient(int *x, int *y) {
  _voidcall(SCREENCLIENT, x, y);
}

int RootWnd::init(RootWnd *parent, int nochild) {
  return _call(INIT, 0, parent, nochild);
}

int RootWnd::init2(OSMODULEHANDLE moduleHandle, OSWINDOWHANDLE parent, int nochild) {
  return _call(INIT2, 0, moduleHandle, parent, nochild);
}

int RootWnd::isInited() {
  return _call(ISINITED, 0);
}

int RootWnd::isPostOnInit() {
  return _call(ISPOSTONINIT, 0);
}

int RootWnd::setVirtual(int i) {
  return _call(SETVIRTUAL, 0, i);
}

int RootWnd::getCursorType(int x, int y) {
  return _call(GETCURSORTYPE, 0, x, y);
}

OSCURSORHANDLE RootWnd::getCustomCursor(int x, int y) {
  return _call(GETCUSTOMCURSOR, (OSCURSORHANDLE)NULL, x, y);
}

void RootWnd::getClientRect(RECT *r) {
  _voidcall(GETCLIENTRECT, r);
}

void RootWnd::getNonClientRect(RECT *rect) {
  _voidcall(GETNONCLIENTRECT, rect);
}

void RootWnd::getPosition(POINT *pt) {
  _voidcall(GETPOSITION, pt);
}

void RootWnd::setVisible(int show) {
  _voidcall(SETVISIBLE, show);
}

void RootWnd::setCloaked(int cloak) {
  _voidcall(SETCLOAKED, cloak);
}

void RootWnd::onSetVisible(int show) {
  _voidcall(ONSETVISIBLE, show);
}

void RootWnd::invalidate() {
  _voidcall(INVALIDATE);
}
  
void RootWnd::invalidateRect(RECT *r) {
  _voidcall(INVALIDATERECT, r);
}

void RootWnd::invalidateRgn(Region *r) {
  _voidcall(INVALIDATERGN, r);
}

void RootWnd::onChildInvalidate(Region *r, RootWnd *who) {
  _voidcall(ONCHILDINVALIDATE, r, who);
}

void RootWnd::invalidateFrom(RootWnd *who) {
  _voidcall(INVALIDATEFROM, who);
}

void RootWnd::invalidateRectFrom(RECT *r, RootWnd *who) {
  _voidcall(INVALIDATERECTFROM, r, who);
}

void RootWnd::invalidateRgnFrom(Region *r, RootWnd *who) {
  _voidcall(INVALIDATERGNFROM, r, who);
}

void RootWnd::validate() {
  _voidcall(VALIDATE);
}

void RootWnd::validateRect(RECT *r) {
  _voidcall(VALIDATERECT, r);
}

void RootWnd::validateRgn(Region *reg) {
  _voidcall(VALIDATERGN, reg);
}

int RootWnd::onSiblingInvalidateRgn(Region *r, RootWnd *who, int who_idx, int my_idx) {
  return _call(ONSIBINVALIDATE, 0, r, who, who_idx, my_idx);
}

int RootWnd::wantSiblingInvalidations() {
  return _call(WANTSIBINVALIDATE, 0);
}

int RootWnd::cascadeRepaintFrom(RootWnd *who, int pack) {
  return _call(CASCADEREPAINTFROM, 0, who, pack);
}

int RootWnd::cascadeRepaintRgnFrom(Region *reg, RootWnd *who, int pack) {
  return _call(CASCADEREPAINTRGNFROM, 0, reg, who, pack);
}

int RootWnd::cascadeRepaintRectFrom(RECT *r, RootWnd *who, int pack) {
  return _call(CASCADEREPAINTRECTFROM, 0, r, who, pack);
}

int RootWnd::cascadeRepaint(int pack) {
  return _call(CASCADEREPAINT, 0, pack);
}

int RootWnd::cascadeRepaintRgn(Region *reg, int pack) {
  return _call(CASCADEREPAINTRGN, 0, reg, pack);
}

int RootWnd::cascadeRepaintRect(RECT *r, int pack) {
  return _call(CASCADEREPAINTRECT, 0, r, pack);
}

void RootWnd::repaint() {
  _voidcall(REPAINT);
}

RootWnd *RootWnd::getBaseTextureWindow() {
  return _call(GETTEXTUREWND, (RootWnd *)0);
}

int RootWnd::onActivate() {
  return _call(ONACTIVATE, 0);
}

void RootWnd::activate() {
  _voidcall(ACTIVATE);
}
  
int RootWnd::onDeactivate() {
  return _call(ONDEACTIVATE, 0);
}

int RootWnd::isActive() {
  return _call(ISACTIVATED, 0);
}

int RootWnd::getDesktopAlpha() {
  return _call(GETDESKTOPALPHA, 0);
}

Region *RootWnd::getRegion() {
  return _call(GETREGION, (Region *)NULL);
} 

int RootWnd::handleTransparency() {
  return _call(HANDLETRANSPARENCY, 0);
}

int RootWnd::handleDesktopAlpha() {
  return _call(HANDLEDESKTOPALPHA, 0);
}

void RootWnd::setStartHidden(int sh) {
  _voidcall(SETSTARTHIDDEN, sh);
}

double RootWnd::getRenderRatio() {
  return _call(GETRENDERRATIO, 0.0);
}

void RootWnd::setRenderRatio(double r) {
  _voidcall(SETRENDERRATIO, r);
}

void RootWnd::setRatioLinked(int l) {
  _voidcall(SETRATIOLINKED, l);
}

int RootWnd::handleRatio() {
  return _call(HANDLERATIO, 0);
}

void RootWnd::resize(int x, int y, int w, int h, int wantcb) {
  _voidcall(_RESIZE, x, y, w, h, wantcb);
}

void RootWnd::move(int x, int y) {
  _voidcall(_MOVE, x, y);
}

int RootWnd::checkDoubleClick(int button, int x, int y) {
  return _call(CHECKDBLCLK, 0, button, x, y);
}

void RootWnd::registerRootWndChild(RootWnd *child) {
  _voidcall(REGISTERROOTWNDCHILD, child);
}

void RootWnd::unregisterRootWndChild(RootWnd *child) {
  _voidcall(UNREGISTERROOTWNDCHILD, child);
}

RootWnd *RootWnd::findRootWndChild(int x, int y, int only_virtuals, int throughtest) {
  return _call(FINDROOTWNDCHILD, (RootWnd *)NULL, x, y, only_virtuals, throughtest);
}

RootWnd *RootWnd::enumRootWndChildren(int _enum) {
  return _call(ENUMROOTWNDCHILDREN, (RootWnd *)NULL, _enum);
}

int RootWnd::getNumRootWndChildren() {
  return _call(GETNUMROOTWNDCHILDREN, 0);  
}

int RootWnd::isVirtual() {
  return _call(ISVIRTUAL, 0);
}

void RootWnd::bringVirtualToFront(RootWnd *w) {
  _voidcall(BRINGVTOFRONT, w);
}

void RootWnd::bringVirtualToBack(RootWnd *w) {
  _voidcall(BRINGVTOBACK, w);
}

void RootWnd::bringVirtualAbove(RootWnd *w, RootWnd *b) {
  _voidcall(BRINGVABOVE, w, b);
}

void RootWnd::bringVirtualBelow(RootWnd *w, RootWnd *b) {
  _voidcall(BRINGVBELOW, w, b);
}

void RootWnd::setVirtualChildCapture(RootWnd *child) {
  _voidcall(SETVCAPTURE, child);
}

RootWnd *RootWnd::getVirtualChildCapture() {
  return _call(GETVCAPTURE, (RootWnd *)NULL);
}

BOOL RootWnd::ptInRegion(int x, int y) {
  return _call(PTINREGION, (BOOL)0, x, y);
}

int RootWnd::onLeftButtonDblClk(int x, int y) {
  return _call(ONLBDBLCLK, 0, x, y);
}

int RootWnd::onMiddleButtonDblClk(int x, int y) {
  return _call(ONMBDBLCLK, 0, x, y);
}

int RootWnd::onRightButtonDblClk(int x, int y) {
  return _call(ONRBDBLCLK, 0, x, y);
}

int RootWnd::onLeftButtonUp(int x, int y) {
  return _call(ONLBUP, 0, x, y);
}

int RootWnd::onMiddleButtonUp(int x, int y) {
  return _call(ONMBUP, 0, x, y);
}

int RootWnd::onRightButtonUp(int x, int y) {
  return _call(ONRBUP, 0, x, y);
}

int RootWnd::onLeftButtonDown(int x, int y) {  
  return _call(ONLBDOWN, 0, x, y);
}

int RootWnd::onMiddleButtonDown(int x, int y) {  
  return _call(ONMBDOWN, 0, x, y);
}

int RootWnd::onRightButtonDown(int x, int y) {
  return _call(ONRBDOWN, 0, x, y);
}

int RootWnd::onMouseMove(int x, int y) {
  return _call(ONMOUSEMOVE, 0, x, y);
}

int RootWnd::getNotifyId() {
  return _call(GETNOTIFYID, 0);
}

void *RootWnd::getInterface(GUID interface_guid) {
  return _call(GETINTERFACE, (void *)NULL, interface_guid);
}

void RootWnd::setEnabled(int e) {
  _voidcall(SETENABLED, e);
}

int RootWnd::isEnabled(int within) {
  return _call(ISENABLED, 0, within);
}

int RootWnd::onEnable(int e) {
  return _call(ONENABLE, 0, e);
}

void RootWnd::setAlpha(int activealpha, int inactivealpha) {
  _voidcall(SETALPHA, activealpha, inactivealpha);
}

void RootWnd::getAlpha(int *active, int *inactive) {
  _voidcall(GETALPHA, active, inactive);
}

int RootWnd::getPaintingAlpha() {
  return _call(GETPAINTALPHA, 255);
}

void RootWnd::setTip(const char *tip) {
  _voidcall(SETTOOLTIP, tip);
}

int RootWnd::runModal() {
  return _call(RUNMODAL, 0);
}

void RootWnd::endModal(int retcode) {
  _voidcall(ENDMODAL, retcode);
}

int RootWnd::wantAutoContextMenu() {
  return _call(WANTAUTOCONTEXTMENU, 0);
}

int RootWnd::wantActivation() {
  return _call(WANTACTIVATION, 1); 
}

void RootWnd::bringToFront() {
  _voidcall(BRINGTOFRONT);
}

void RootWnd::bringToBack() {
  _voidcall(BRINGTOBACK);
}

void RootWnd::setFocus() {
  _voidcall(SETFOCUS);
}

int RootWnd::gotFocus() {
  return _call(GOTFOCUS, 0);
}

int RootWnd::onGetFocus() {
  return _call(ONGETFOCUS, 0);
}

int RootWnd::onKillFocus() {
  return _call(ONKILLFOCUS, 0);
}

RootWnd *RootWnd::getNextVirtualFocus(RootWnd *cur) {
  return _call(GETNEXTVFOCUS, (RootWnd *)NULL, cur);
}

int RootWnd::wantFocus() {
  return _call(WANTFOCUS, 0);
}

int RootWnd::excludeFromTabList() {
  return _call(EXCLUDEFROMTABLIST, 0);
}

int RootWnd::onAcceleratorEvent(const char *name) {
  return _call(ONACCELERATOREVENT, 0, name);
}

int RootWnd::onChar(unsigned int c) {
  return _call(ONCHAR, 0, c);
}

int RootWnd::onKeyDown(int keycode) {
  return _call(ONKEYDOWN, 0, keycode);
}

int RootWnd::onKeyUp(int keycode) {
  return _call(ONKEYUP, 0, keycode);
}

int RootWnd::onSysKeyDown(int keycode, int keydata) {
  return _call(ONSYSKEYDOWN, 0, keycode, keydata);
}

int RootWnd::onSysKeyUp(int keycode, int keydata) {
  return _call(ONSYSKEYUP, 0, keycode, keydata);
}

void RootWnd::setVirtualChildFocus(RootWnd *w) {
  _voidcall(SETVFOCUS, w);
}

int RootWnd::getRegionOp() {
  return _call(GETREGIONOP, 0);
}

void RootWnd::invalidateWindowRegion() {
  _voidcall(INVALWNDRGN);
}

Region *RootWnd::getComposedRegion() {
  return _call(GETCOMPOSEDRGN, (Region *)NULL);
}

Region *RootWnd::getSubtractorRegion() {
  return _call(GETSUBTRACTORRGN, (Region *)NULL);
}

void RootWnd::setRegionOp(int op) {
  _voidcall(SETREGIONOP, op);
}

void RootWnd::setRectRgn(int rrgn) {
  _voidcall(SETRECTRGN, rrgn);
}

int RootWnd::isRectRgn() {
  return _call(ISRECTRGN, 0);
}

TimerClient *RootWnd::getTimerClient() {
  return _call(GETTIMERCLIENT, (TimerClient *)NULL);
}

Dependent *RootWnd::getDependencyPtr() {
  return _call(GETDEPENDENCYPTR, (Dependent *)NULL);
}

void RootWnd::addMinMaxEnforcer(RootWnd *w) {
  _voidcall(ADDMINMAXENFORCER, w);
}

void RootWnd::removeMinMaxEnforcer(RootWnd *w) {
  _voidcall(REMOVEMINMAXENFORCER, w);
}

RootWnd *RootWnd::enumMinMaxEnforcer(int n) {
  return _call(ENUMMINMAXENFORCER, (RootWnd *)NULL, n);
}

int RootWnd::getNumMinMaxEnforcers() {
  return _call(GETNUMMINMAXENFORCERS, 0);
}

void RootWnd::signalMinMaxEnforcerChanged() {
  _voidcall(SIGNALMINMAXCHANGED, 0);
}

int RootWnd::bypassModal() {
  return _call(BYPASSMODAL, 0);
}

int RootWnd::onAction(const char *action, const char *param, int x, int y, int p1, int p2, void *data, int datalen, RootWnd *source) {
  return _call(ROOTWNDONACTION, 0, action, param, x, y, p1, p2, data, datalen, source);
}

void RootWnd::setRenderBaseTexture(int i) {
  _voidcall(SETRENDERBASETEXTURE, i);
}

int RootWnd::getRenderBaseTexture() {
  return _call(GETRENDERBASETEXTURE, 0);
}

GuiObject *RootWnd::getGuiObject() {
  return _call(GETGUIOBJECT, (GuiObject *) NULL);
}

int RootWnd::triggerEvent(int event, int p1, int p2) {
  return _call(TRIGGEREVENT, 0, event, p1, p2);
}

int RootWnd::windowEvent(int event, unsigned long p1, unsigned long p2, unsigned long p3, unsigned long p4) {
  return _call(WINDOWEVENT, -1, event, p1, p2, p3, p4);
}

int RootWnd::getFlag(int flag) {
  return _call(GETFLAG, 0, flag);
}

int RootWnd::allowDeactivation() {
  return _call(ALLOWDEACTIVATE, 1);
}

void RootWnd::setAllowDeactivation(int allow) {
  _voidcall(SETALLOWDEACTIVATE, allow);
}

RootWnd *RootWnd::findWindow(const char *id) {
  return _call(FINDWND_BYID, (RootWnd *)NULL, id);
}

RootWnd *RootWnd::findWindowByInterface(GUID interface_guid) {
  return _call(FINDWND_BYGUID, (RootWnd *)NULL, interface_guid);
}

RootWnd *RootWnd::findWindowByCallback(FindObjectCallback *cb) {
  return _call(FINDWND_BYCB, (RootWnd *)NULL, cb);
}

RootWnd *RootWnd::findWindowChain(FindObjectCallback *cb, RootWnd *wcaller) {
  return _call(FINDWNDCHAIN, (RootWnd *)NULL, cb, wcaller);
}

void RootWnd::setTabOrder(int a) {
  _voidcall(SETTABORDER, a);
}

int RootWnd::getTabOrder() {
  return _call(GETTABORDER, -1);
}

void RootWnd::setAutoTabOrder() {
  _voidcall(SETAUTOTABORDER, 0);
}

void RootWnd::setVirtualTabOrder(RootWnd *w, int a) {
  _voidcall(SETVIRTUALTABORDER, w, a);
}

int RootWnd::getVirtualTabOrder(RootWnd *w) {
  return _call(GETVIRTUALTABORDER, 0, w);
}

void RootWnd::setVirtualAutoTabOrder(RootWnd *w) {
  _voidcall(SETVIRTUALAUTOTABORDER, w);
}

RootWnd *RootWnd::getCurVirtualChildFocus() {
  return _call(GETCURVIRTUALCHILDFOCUS, (RootWnd *)NULL);
}

RootWnd *RootWnd::getTab(int what) {
  return _call(GETTAB, (RootWnd *)NULL, what);
}

void RootWnd::focusNext() {
  _voidcall(FOCUSNEXT);
}

void RootWnd::focusPrevious() {
  _voidcall(FOCUSPREVIOUS);
}

int RootWnd::getNumTabs() {
  return _call(GETNUMTABS, 0);
}

RootWnd *RootWnd::enumTab(int i) {
  return _call(ENUMTAB, (RootWnd *)NULL, i);
}

void RootWnd::onSetRootFocus(RootWnd *w) {
  _voidcall(ONSETROOTFOCUS, w);
}

int RootWnd::getFocusOnClick() {
  return _call(GETFOCUSONCLICK, 0);
}

void RootWnd::setFocusOnClick(int i) {
  _voidcall(SETFOCUSONCLICK, i);
}

void RootWnd::setNoDoubleClicks(int no) {
  _voidcall(SETNODOUBLECLICKS, no);
}

void RootWnd::setNoLeftClicks(int no) {
  _voidcall(SETNOLEFTCLICKS, no);
}

void RootWnd::setNoMiddleClicks(int no) {
  _voidcall(SETNOMIDDLECLICKS, no);
}

void RootWnd::setNoRightClicks(int no) {
  _voidcall(SETNORIGHTCLICKS, no);
}

void RootWnd::setNoMouseMoves(int no) {
  _voidcall(SETNOMOUSEMOVES, no);
}

void RootWnd::setNoContextMenus(int no) {
  _voidcall(SETNOCONTEXTMENUS, no);
}

int RootWnd::wantDoubleClicks() {
  return _call(WANTDOUBLECLICKS, 1);
}

int RootWnd::wantRightClicks() {
  return _call(WANTRIGHTCLICKS, 1);
}

int RootWnd::wantLeftClicks() {
  return _call(WANTLEFTCLICKS, 1);
}

int RootWnd::wantMiddleClicks() {
  return _call(WANTMIDDLECLICKS, 1);
}

int RootWnd::wantMouseMoves() {
  return _call(WANTMOUSEMOVES, 1);
}

int RootWnd::wantContextMenus() {
  return _call(WANTCONTEXTMENUS, 1);
}

void RootWnd::setDefaultCursor(Cursor *c) {
  _voidcall(SETDEFAULTCURSOR, c);
}

#ifdef WASABI_API_ACCESSIBILITY
Accessible *RootWnd::getAccessibleObject(int createifnotexist) {
  return _call(GETACCESSIBLEOBJECT, (Accessible*)NULL, createifnotexist);
}

void RootWnd::detachAccessibleObject() {
  _voidcall(DETACHACCESSIBLEOBJECT);
}

const char *RootWnd::accessibility_getName() {
  return _call(ACCGETNAME, (const char *)NULL);
}

const char *RootWnd::accessibility_getValue() {
  return _call(ACCGETVALUE, (const char *)NULL);
}

const char *RootWnd::accessibility_getDescription() {
  return _call(ACCGETDESC, (const char *)NULL);
}

int RootWnd::accessibility_getRole() {
  return _call(ACCGETROLE, 0);
}

int RootWnd::accessibility_getState() {
  return _call(ACCGETSTATE, 0);
}

const char *RootWnd::accessibility_getHelp() {
  return _call(ACCGETHELP, (const char *)NULL);
}

const char *RootWnd::accessibility_getAccelerator() {
  return _call(ACCGETACCELERATOR, (const char *)NULL);
}

const char *RootWnd::accessibility_getDefaultAction() {
  return _call(ACCGETDEFACTION, (const char *)NULL);
}

void RootWnd::accessibility_doDefaultAction() {
  _voidcall(ACCDODEFACTION);
}

int RootWnd::accessibility_getNumItems() {
  return _call(ACCGETNUMITEMS, 0);
}

const char *RootWnd::accessibility_getItemName(int n) {
  return _call(ACCGETITEMNAME, (const char *)NULL, n);
}

const char *RootWnd::accessibility_getItemValue(int n) {
  return _call(ACCGETITEMVALUE, (const char *)NULL, n);
}

const char *RootWnd::accessibility_getItemDescription(int n) {
  return _call(ACCGETITEMDESC, (const char *)NULL, n);
}

int RootWnd::accessibility_getItemRole(int n) {
  return _call(ACCGETITEMROLE, 0, n);
}

int RootWnd::accessibility_getItemState(int n) {
  return _call(ACCGETITEMSTATE, 0, n);
}

const char *RootWnd::accessibility_getItemHelp(int n) {
  return _call(ACCGETITEMHELP, (const char *)NULL, n);
}

const char *RootWnd::accessibility_getItemAccelerator(int n) {
  return _call(ACCGETITEMACCELERATOR, (const char *)NULL, n);
}

const char *RootWnd::accessibility_getItemDefaultAction(int n) {
  return _call(ACCGETITEMDEFACTION, (const char *)NULL, n);
}

void RootWnd::accessibility_doItemDefaultAction(int n) {
  _voidcall(ACCDOITEMDEFACTION, n);
}

void RootWnd::accessibility_getItemRect(int n, RECT *r) {
  _voidcall(ACCGETITEMRECT, n, r);
}

int RootWnd::accessibility_getItemXY(int x, int y) {
  return _call(ACCGETITEMXY, -1, x, y);
}

RootWnd *RootWnd::accessibility_getStdObject(int objid) {
  return _call(ACCGETSTDOBJECT, (RootWnd *)NULL, objid);
}

int RootWnd::accessibility_isLeaf() {
  return _call(ACCISLEAF, 0);
}
#endif

void RootWnd::enableFocusEvents(int a) {
  _voidcall(ENABLEFOCUSEVENTS, a);
}

int RootWnd::getFocusEventsEnabled() {
  return _call(GETFOCUSEVENTSENABLED, 1);
}

#ifdef EXPERIMENTAL_INDEPENDENT_AOT
void RootWnd::setAlwaysOnTop(int i) {
  _voidcall(SETALWAYSONTOP, i);
}

int RootWnd::getAlwaysOnTop() {
  return _call(GETALWAYSONTOP, 0);
}
#endif

void RootWnd::setNCRole(int ncrole) {
  _voidcall(SETNCROLE, ncrole);
}

int RootWnd::getNCRole() {
  return _call(GETNCROLE, 0);
}

#ifdef WASABI_COMPILE_SCRIPT
ScriptObject *RootWnd::getGuiScriptObject() {
  return _call(GETGUISCRIPTOBJECT, (ScriptObject *)NULL);
}
#endif

#ifdef WASABI_PLATFORM_WIN32
#ifndef WA3COMPATIBILITY
void RootWnd::setDropTarget(void *dt) {
  _voidcall(SETDROPTARGET, dt);
}

void *RootWnd::getDropTarget() {
  return _call(GETDROPTARGET, (void *)NULL);
}
#endif
#endif

int RootWnd::isMinimized() {
  return _call(ISMINIMIZED, 0);
}

int RootWnd::isMaximized() {
  return _call(ISMAXIMIZED, 0);
}

void RootWnd::setNonBuffered(int nb) {
  _voidcall(SETNONBUFFERED, nb);
}

int RootWnd::isBuffered() {
  return _call(ISBUFFERED, 1);
}

#define CBCLASS RootWndI
START_DISPATCH;
  CB(GETTIMERCLIENT, getTimerClient);
  VCB(BATCHPROCESSES, performBatchProcesses);
  CB(GETOSWINDOWHANDLE, getOsWindowHandle);
  CB(GETOSMODULEHANDLE, getOsModuleHandle);
  CB(GETROOTWNDNAME, getRootWndName);
  CB(GETID, getId);
  CB(GETGUIOBJECT, getGuiObject);
  CB(INIT, init);
  CB(INIT2, init2);
  CB(ISINITED, isInited);
  CB(ISPOSTONINIT, isPostOnInit);
  CB(SETVIRTUAL, setVirtual);
  CB(CLICKTHROUGH, isClickThrough);
  CB(DNDTHROUGH, isDndThrough);
  CB(GETFORWARDWND, getForwardWnd);
  CB(ONMOUSEMOVE, onMouseMove);
  CB(ONLBUP, onLeftButtonUp);
  CB(ONMBUP, onMiddleButtonUp);
  CB(ONRBUP, onRightButtonUp);
  CB(ONLBDOWN, onLeftButtonDown);
  CB(ONMBDOWN, onMiddleButtonDown);
  CB(ONRBDOWN, onRightButtonDown);
  CB(ONLBDBLCLK, onLeftButtonDblClk);
  CB(ONMBDBLCLK, onMiddleButtonDblClk);
  CB(ONRBDBLCLK, onRightButtonDblClk);
  CB(GETDRAGINTERFACE, getDragInterface);
  CB(GETCURSORTYPE, getCursorType);
  CB(GETCUSTOMCURSOR, getCustomCursor);
  CB(FROMPOINT, rootWndFromPoint);
  VCB(GETCLIENTRECT, getClientRect);
  VCB(GETNONCLIENTRECT, getNonClientRect);
  VCB(GETWINDOWRECT, getWindowRect);
  VCB(SETVISIBLE, setVisible);
  VCB(SETCLOAKED, setCloaked);
  VCB(ONSETVISIBLE, onSetVisible);
  CB(ISVISIBLE, isVisible);
  CB(GETSKINID, getSkinId);
  CB(GETINTERFACE, getInterface);
  CB(ONMETRICCHANGE, onMetricChange);
  VCB(INVALIDATE, invalidate);
  VCB(INVALIDATERECT, invalidateRect);
  VCB(INVALIDATERGN, invalidateRgn);
  VCB(INVALIDATEFROM, invalidateFrom);
  VCB(INVALIDATERECTFROM, invalidateRectFrom);
  VCB(INVALIDATERGNFROM, invalidateRgnFrom);
  VCB(ONCHILDINVALIDATE, onChildInvalidate);
  VCB(VALIDATE, validate);
  VCB(VALIDATERECT, validateRect);
  VCB(VALIDATERGN, validateRgn);
  CB(ONPAINT, rootwnd_onPaint);
  CB(PAINTTREE, rootwnd_paintTree);
  CB(PAINT, paint);
  CB(GETFRAMEBUFFER, getFrameBuffer);
  VCB(COMMITFRAMEBUFFER, commitFrameBuffer);
  CB(GETPARENT, getParent);
  CB(GETROOTPARENT, getRootParent);
  CB(GETDESKTOPPARENT, getDesktopParent);
  VCB(SETPARENT, setParent);
  CB(ONSIBINVALIDATE, onSiblingInvalidateRgn);
  CB(WANTSIBINVALIDATE, wantSiblingInvalidations);
  CB(CASCADEREPAINTFROM, cascadeRepaintFrom);
  CB(CASCADEREPAINTRGNFROM, cascadeRepaintRgnFrom);
  CB(CASCADEREPAINTRECTFROM, cascadeRepaintRectFrom);
  CB(CASCADEREPAINT, cascadeRepaint);
  CB(CASCADEREPAINTRGN, cascadeRepaintRgn);
  CB(CASCADEREPAINTRECT, cascadeRepaintRect);
  VCB(REPAINT, repaint);
  CB(GETTEXTUREWND, getBaseTextureWindow);
  CB(ONDEFERREDCALLBACK, onDeferredCallback);
  VCB(POSTDEFERREDCALLBACK, postDeferredCallback);
  CB(CHILDNOTIFY, childNotify);
  VCB(BROADCASTNOTIFY, broadcastNotify);
  CB(GETPREFERENCES, getPreferences);
  VCB(SETPREFERENCES, setPreferences);
  CB(GETREGION, getRegion);
  CB(GETDESKTOPALPHA, getDesktopAlpha);
  VCB(SETSTARTHIDDEN, setStartHidden);
  CB(GETRENDERRATIO, getRenderRatio);
  VCB(SETRENDERRATIO, setRenderRatio);
  VCB(SETRATIOLINKED, setRatioLinked);
  CB(HANDLERATIO, handleRatio);
  VCB(_RESIZE, resize);
  VCB(_MOVE, move);
  VCB(GETPOSITION, getPosition);
  VCB(REGISTERROOTWNDCHILD, registerRootWndChild);
  VCB(UNREGISTERROOTWNDCHILD, unregisterRootWndChild);
  CB(FINDROOTWNDCHILD, findRootWndChild);
  CB(ENUMROOTWNDCHILDREN, enumRootWndChildren);
  CB(GETNUMROOTWNDCHILDREN, getNumRootWndChildren);
  CB(ISVIRTUAL, isVirtual);
  VCB(BRINGVTOFRONT, bringVirtualToFront);
  VCB(BRINGVTOBACK, bringVirtualToBack);
  VCB(BRINGVABOVE, bringVirtualAbove);
  VCB(BRINGVBELOW, bringVirtualBelow);
  CB(CHECKDBLCLK, checkDoubleClick);
  CB(BEGINCAPTURE, beginCapture);
  CB(ENDCAPTURE, endCapture);
  CB(GETCAPTURE, getCapture);
  VCB(SETVCAPTURE, setVirtualChildCapture);
  CB(GETVCAPTURE, getVirtualChildCapture);
  VCB(SETALLOWCAPTURE, setAllowCapture);
  CB(ISCAPTUREALLOWED, isCaptureAllowed);
  CB(PTINREGION, ptInRegion);
  VCB(CLIENTSCREEN, clientToScreen);
  VCB(SCREENCLIENT, screenToClient);
  CB(ONACTIVATE, onActivate);
  VCB(ACTIVATE, activate);
  CB(ONDEACTIVATE, onDeactivate);
  CB(ISACTIVATED, isActive);
  CB(HANDLETRANSPARENCY, handleTransparency);
  CB(HANDLEDESKTOPALPHA, handleDesktopAlpha);
  CB(GETNOTIFYID, getNotifyId);
  VCB(SETENABLED, setEnabled);
  CB(ONENABLE, onEnable);
  CB(ISENABLED, isEnabled);
  CB(GETPAINTALPHA, getPaintingAlpha);
  VCB(SETALPHA, setAlpha);
  VCB(GETALPHA, getAlpha);
  VCB(SETCLICKTHROUGH, setClickThrough);
  VCB(SETTOOLTIP, setTip);
  CB(RUNMODAL, runModal);
  VCB(ENDMODAL, endModal);
  CB(WANTAUTOCONTEXTMENU, wantAutoContextMenu);
  VCB(ONCANCELCAPTURE, onCancelCapture);
  VCB(CANCELCAPTURE, cancelCapture);
  VCB(BRINGTOFRONT, bringToFront);
  VCB(BRINGTOBACK, bringToBack);
  VCB(SETFOCUS, setFocus);
  CB(GOTFOCUS, gotFocus);
  CB(ONGETFOCUS, onGetFocus);
  CB(ONKILLFOCUS, onKillFocus);
  CB(GETNEXTVFOCUS, getNextVirtualFocus);
  VCB(SETVFOCUS, setVirtualChildFocus);
  CB(WANTFOCUS, wantFocus);
  CB(EXCLUDEFROMTABLIST, excludeFromTabList);
  CB(ONACCELERATOREVENT, onAcceleratorEvent);
  CB(ONCHAR, onChar);
  CB(ONKEYDOWN, onKeyDown);
  CB(ONKEYUP, onKeyUp);
  CB(ONSYSKEYDOWN, onSysKeyDown);
  CB(ONSYSKEYUP, onSysKeyUp);
  CB(GETREGIONOP, getRegionOp);
  VCB(SETREGIONOP, setRegionOp);
  VCB(INVALWNDRGN, invalidateWindowRegion);
  CB(GETCOMPOSEDRGN, getComposedRegion);
  CB(GETSUBTRACTORRGN, getSubtractorRegion);
  CB(ISRECTRGN, isRectRgn);
  VCB(SETRECTRGN, setRectRgn);
  CB(GETDEPENDENCYPTR, rootwnd_getDependencyPtr);
  VCB(ADDMINMAXENFORCER, addMinMaxEnforcer);
  VCB(REMOVEMINMAXENFORCER, removeMinMaxEnforcer);
  CB(GETNUMMINMAXENFORCERS, getNumMinMaxEnforcers);
  CB(ENUMMINMAXENFORCER, enumMinMaxEnforcer);
  VCB(SIGNALMINMAXCHANGED, signalMinMaxEnforcerChanged);
  CB(ROOTWNDONACTION, onAction);
  CB(BYPASSMODAL, bypassModal);
  VCB(SETRENDERBASETEXTURE, setRenderBaseTexture);
  CB(GETRENDERBASETEXTURE, getRenderBaseTexture);
  CB(TRIGGEREVENT, triggerEvent);
  CB(WINDOWEVENT, windowEvent);
  CB(GETFLAG, getFlag);
  CB(ALLOWDEACTIVATE, allowDeactivation);
  VCB(SETALLOWDEACTIVATE, setAllowDeactivation);
  CB(FINDWND_BYID,  findWindow);
  CB(FINDWND_BYGUID, findWindowByInterface);
  CB(FINDWND_BYCB, findWindowByCallback);
  CB(FINDWNDCHAIN, findWindowChain);
  VCB(SETTABORDER, setTabOrder);
  CB(GETTABORDER, getTabOrder);
  VCB(SETVIRTUALTABORDER, setVirtualTabOrder);
  CB(GETVIRTUALTABORDER, getVirtualTabOrder);
  VCB(SETAUTOTABORDER, setAutoTabOrder);
  VCB(SETVIRTUALAUTOTABORDER, setVirtualAutoTabOrder);
  CB(GETCURVIRTUALCHILDFOCUS, getCurVirtualChildFocus);
  CB(GETTAB, getTab);
  VCB(FOCUSNEXT, focusNext);
  VCB(FOCUSPREVIOUS, focusPrevious);
  CB(ENUMTAB, enumTab);
  CB(GETNUMTABS, getNumTabs);
  VCB(ONSETROOTFOCUS, onSetRootFocus);
  VCB(SETFOCUSONCLICK, setFocusOnClick);
  CB(GETFOCUSONCLICK, getFocusOnClick);
  VCB(SETNODOUBLECLICKS, setNoDoubleClicks);
  VCB(SETNOLEFTCLICKS, setNoLeftClicks);
  VCB(SETNOMIDDLECLICKS, setNoMiddleClicks);
  VCB(SETNORIGHTCLICKS, setNoRightClicks);
  VCB(SETNOMOUSEMOVES, setNoMouseMoves);
  VCB(SETNOCONTEXTMENUS, setNoContextMenus);
  CB(WANTLEFTCLICKS, wantLeftClicks);
  CB(WANTMIDDLECLICKS, wantMiddleClicks);
  CB(WANTRIGHTCLICKS, wantRightClicks);
  CB(WANTDOUBLECLICKS, wantDoubleClicks);
  CB(WANTMOUSEMOVES, wantMouseMoves);
  CB(WANTCONTEXTMENUS, wantContextMenus);
  CB(WANTACTIVATION, wantActivation);
  VCB(SETDEFAULTCURSOR, setDefaultCursor);
#ifdef WASABI_API_ACCESSIBILITY
  CB(GETACCESSIBLEOBJECT, getAccessibleObject);
  VCB(DETACHACCESSIBLEOBJECT, detachAccessibleObject);
  CB(ACCGETNAME, accessibility_getName);
  CB(ACCGETVALUE, accessibility_getValue);
  CB(ACCGETDESC, accessibility_getDescription);
  CB(ACCGETROLE, accessibility_getRole);
  CB(ACCGETSTATE,accessibility_getState);
  CB(ACCGETHELP, accessibility_getHelp);
  CB(ACCGETACCELERATOR, accessibility_getAccelerator);
  CB(ACCGETDEFACTION, accessibility_getDefaultAction);
  VCB(ACCDODEFACTION, accessibility_doDefaultAction);
  CB(ACCGETNUMITEMS, accessibility_getNumItems);
  CB(ACCGETITEMNAME, accessibility_getItemName);
  CB(ACCGETITEMVALUE,accessibility_getItemValue);
  CB(ACCGETITEMDESC, accessibility_getItemDescription);
  CB(ACCGETITEMROLE, accessibility_getItemRole);
  CB(ACCGETITEMSTATE,accessibility_getItemState);
  CB(ACCGETITEMHELP, accessibility_getItemHelp);
  CB(ACCGETITEMACCELERATOR, accessibility_getItemAccelerator);
  CB(ACCGETITEMDEFACTION, accessibility_getItemDefaultAction);
  VCB(ACCDOITEMDEFACTION, accessibility_doItemDefaultAction);
  VCB(ACCGETITEMRECT, accessibility_getItemRect);
  CB(ACCGETITEMXY, accessibility_getItemXY);
  CB(ACCGETSTDOBJECT, accessibility_getStdObject);
  CB(ACCISLEAF, accessibility_isLeaf);
#endif
  VCB(ENABLEFOCUSEVENTS, enableFocusEvents);
  CB(GETFOCUSEVENTSENABLED, getFocusEventsEnabled);
#ifdef WASABI_PLATFORM_WIN32
#ifndef WA3COMPATIBILITY
  VCB(SETDROPTARGET, setDropTarget);
  CB(GETDROPTARGET, getDropTarget);
#endif
#endif
#ifdef EXPERIMENTAL_INDEPENDENT_AOT
  VCB(SETALWAYSONTOP, setAlwaysOnTop);
  CB(GETALWAYSONTOP, getAlwaysOnTop);
#endif
  CB(GETNCROLE, getNCRole);
  VCB(SETNCROLE, setNCRole);
#ifdef WASABI_COMPILE_SCRIPT
  CB(GETGUISCRIPTOBJECT, getGuiScriptObject);
  CB(ISMINIMIZED, isMinimized);
  CB(ISMAXIMIZED, isMaximized);
#endif
  VCB(SETNONBUFFERED, setNonBuffered);
  CB(ISBUFFERED, isBuffered);
END_DISPATCH;

