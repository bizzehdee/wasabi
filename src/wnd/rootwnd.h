#ifndef _ROOTWND_H
#define _ROOTWND_H

#include <bfc/std.h>

#include <bfc/dispatch.h>
//#include "cursor.h"

class CanvasBase;
class Region;
class TimerClient;
class Dependent;
class GuiObject;
class FindObjectCallback;
class Cursor;
#ifdef WASABI_API_ACCESSIBILITY
class Accessible;
#endif
class Canvas;

// only these methods are safe across dll boundaries
// this is the pointer you find in the GWL_USERDATA of a window

class DragInterface;

enum WndPreferences {
  SUGGESTED_X,
  SUGGESTED_Y,
  SUGGESTED_W,
  SUGGESTED_H,
  MINIMUM_W,
  MINIMUM_H,
  MAXIMUM_W,
  MAXIMUM_H,
};

enum GetTab {
  TAB_GETCURRENT,
  TAB_GETNEXT,
  TAB_GETPREVIOUS,
  TAB_GETFIRST,
  TAB_GETLAST,
};

enum {
  THROUGHTEST_NONE=0,
  THROUGHTEST_NOCLICKTHROUGH=1,
  THROUGHTEST_NODNDTHROUGH=2,
  THROUGHTEST_NOCLICKORDNDTHROUGH=3,
};

class NOVTABLE RootWnd : public Dispatchable {
protected:
  RootWnd() {}	// protect constructor
public:
  static const GUID *depend_getClassGuid() {
    // {11981849-61F7-4158-8283-DA7DD006D732}
    static const GUID ret = 
{ 0x11981849, 0x61f7, 0x4158, { 0x82, 0x83, 0xda, 0x7d, 0xd0, 0x6, 0xd7, 0x32 } };
    return &ret;
  }
  // this passes thru to the windows WndProc, if there is one -- NONPORTABLE
  virtual LRESULT wndProc(OSWINDOWHANDLE hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)=0;

  // get the OSWINDOWHANDLE (if there is one)
  OSWINDOWHANDLE gethWnd() { return getOsWindowHandle(); } // stay back compatibile
  OSWINDOWHANDLE getOsWindowHandle();
  OSMODULEHANDLE getOsModuleHandle();
  void performBatchProcesses(); // this is called after wndProc is called (under win32) to recompute batch operations such as calculating window regions, cascaderepainting, etc. this prevents N children from independently calling repaintTree for the whole gui on overlaping zones of the framebuffer. under OSes other than win32, this should be called after you've executed all your window events for this poll
  TimerClient *getTimerClient();

  const char *getRootWndName();
  const char *getId();

  int init(RootWnd *parent, int nochild=FALSE);
  int init2(OSMODULEHANDLE moduleHandle, OSWINDOWHANDLE parent, int nochild=FALSE);// for the root window
  int isInited();	// are we post init() ? USE THIS INSTEAD OF gethWnd()==1
  int isPostOnInit();	// are we post onInit() ? USE THIS INSTEAD OF ISINITED TO KNOW IF YOU CAN CALL ONRESIZE (if a function can be called by onInit and other parts of the code at the same time)

  int setVirtual(int i);

  int isClickThrough();
  int isDndThrough();
  int onMouseMove(int x, int y);
  int onLeftButtonUp(int x, int y);
  int onMiddleButtonUp(int x, int y);
  int onRightButtonUp(int x, int y);
  int onLeftButtonDown(int x, int y);
  int onMiddleButtonDown(int x, int y);
  int onRightButtonDown(int x, int y);
  int onLeftButtonDblClk(int x, int y);
  int onMiddleButtonDblClk(int x, int y);
  int onRightButtonDblClk(int x, int y);

  // fetch the DragInterface of the RootWnd here, can be NULL
  DragInterface *getDragInterface();

  int getCursorType(int x, int y);
  OSCURSORHANDLE getCustomCursor(int x, int y);

  // returns deepest child for point or yourself if no child there
  RootWnd *rootWndFromPoint(POINT *pt, int throughtest=THROUGHTEST_NONE);

  void getClientRect(RECT *);
  void getNonClientRect(RECT *rect);
  // the onscreen coordinates
  void getWindowRect(RECT *r);

  void setVisible(int show);
  void setCloaked(int cloak);
  void onSetVisible(int show);
  int isVisible(int within=0);
  int getSkinId();

  void *getInterface(GUID interface_guid);
  int onMetricChange(int metricid, int param1=0, int param2=0);

  // painting stuff
  void invalidate();
  void invalidateRect(RECT *r);
  void invalidateRgn(Region *r);
  void invalidateFrom(RootWnd *who);
  void invalidateRectFrom(RECT *r, RootWnd *who);
  void invalidateRgnFrom(Region *r, RootWnd *who);
  void onChildInvalidate(Region *r, RootWnd *who);
  void validate();
  void validateRect(RECT *r);
  void validateRgn(Region *reg);
  int onPaint(CanvasBase *canvas, Region *r);
  int paintTree(CanvasBase *canvas, Region *r);
  int paint(Canvas *canvas=NULL, Region *r=NULL);
  Canvas *getFrameBuffer();
  void commitFrameBuffer(Canvas *canvas, RECT *r, double ratio);
  RootWnd *getParent();
  RootWnd *getRootParent();
  RootWnd *getDesktopParent();
  void setParent(RootWnd *newparent);
  int onSiblingInvalidateRgn(Region *r, RootWnd *who, int who_idx, int my_idx);
  int wantSiblingInvalidations();
  int wantAutoContextMenu();
  int wantActivation();
  void activate();
  int cascadeRepaintFrom(RootWnd *who, int pack=1);
  int cascadeRepaintRgnFrom(Region *reg, RootWnd *who, int pack=1);
  int cascadeRepaintRectFrom(RECT *r, RootWnd *who, int pack=1);
  int cascadeRepaint(int pack=1);
  int cascadeRepaintRgn(Region *reg, int pack=1);
  int cascadeRepaintRect(RECT *r, int pack=1);
  void repaint();
  RootWnd *getBaseTextureWindow();
  void postDeferredCallback (int p1, int p2=0, int mindelay=0);
  int onDeferredCallback(int param1, int param2);
  int childNotify(RootWnd *child, int msg, int p1, int p2);
  void broadcastNotify(RootWnd *wnd, int msg, int p1, int p2);
  int getPreferences(int what);
  void setPreferences(int what, int v);
  Region *getRegion();
  void invalidateWindowRegion();
  Region *getComposedRegion();
  Region *getSubtractorRegion();
  int getRegionOp();
  void setRegionOp(int op);
  int isRectRgn();
  void setRectRgn(int rrgn);
  int getDesktopAlpha();
  void setStartHidden(int sh);
  void setClickThrough(int ct);
  void focusNext();
  void focusPrevious();

  double getRenderRatio();
  void setRenderRatio(double r);
  void setRatioLinked(int l); // 1 = getRenderRatio asks parent, 0 = returns this ratio, default is 1, 0 should only be used for non virtuals
  int handleRatio();
  void resize(int x, int y, int w, int h, int wantcb=1);
  inline void resizeToRect(RECT *r) {
    resize(r->left, r->top, r->right - r->left, r->bottom - r->top);
  }
  void move(int x, int y);
  void getPosition(POINT *pt);
  RootWnd *getForwardWnd();

  // children registration/enumeration
  void registerRootWndChild(RootWnd *child);
  void unregisterRootWndChild(RootWnd *child);
  RootWnd *findRootWndChild(int x, int y, int only_virtuals=0, int throughtest=THROUGHTEST_NONE);
  RootWnd *enumRootWndChildren(int _enum);
  int getNumRootWndChildren();

  // virtual child stuff
  int isVirtual();
  void bringVirtualToFront(RootWnd *w);
  void bringVirtualToBack(RootWnd *w);
  void bringVirtualAbove(RootWnd *w, RootWnd *b);
  void bringVirtualBelow(RootWnd *w, RootWnd *b);
  int checkDoubleClick(int button, int x, int y);

  int beginCapture();
  int endCapture();
  int getCapture();	// returns 1 if this window has mouse/keyb captured
  void onCancelCapture();
  void cancelCapture();
  void setAllowCapture(int allow);
  int isCaptureAllowed();
  void setVirtualChildCapture(RootWnd *child);
  RootWnd *getVirtualChildCapture();
  RootWnd *getCurVirtualChildFocus();
  RootWnd *getTab(int what=TAB_GETCURRENT);

  int ptInRegion(int x, int y);

  void clientToScreen(int *x, int *y); // so rootWndFromPoint can map ratio
  void screenToClient(int *x, int *y); // ..	
  
  int getNotifyId();

  int onActivate();
  int onDeactivate();
  int isActive();
  int handleTransparency();
  int handleDesktopAlpha();
  void setEnabled(int e);
  int onEnable(int e);
  int isEnabled(int within=0);
  int getPaintingAlpha();           
  void getAlpha(int *activealpha=NULL, int *inactivealpha=NULL);
  void setAlpha(int activealpha, int inactivealpha=-1); // -1 means same as activealpha
  void setTip(const char *tip);

  int runModal();
  void endModal(int retcode);

  void bringToFront();
  void bringToBack();
  void setFocus();
  int gotFocus();
  RootWnd *getNextVirtualFocus(RootWnd *w);
  void setVirtualChildFocus(RootWnd *w);
  void setVirtualTabOrder(RootWnd *w, int a);
  int getVirtualTabOrder(RootWnd *w);
  int wantFocus();
  int excludeFromTabList();
  void setTabOrder(int a);
  int getTabOrder();
  void setAutoTabOrder();
  void setVirtualAutoTabOrder(RootWnd *w);

  int onAcceleratorEvent(const char *name);
  int onChar(unsigned int c);
  int onKeyDown(int keycode);
  int onKeyUp(int keycode);
  int onSysKeyDown(int keyCode, int keyData);
  int onSysKeyUp(int keyCode, int keyData);
  int onKillFocus();
  int onGetFocus();
  void onSetRootFocus(RootWnd *w);

  Dependent *getDependencyPtr();
  void addMinMaxEnforcer(RootWnd *w);
  void removeMinMaxEnforcer(RootWnd *w);
  RootWnd *enumMinMaxEnforcer(int n);
  int getNumMinMaxEnforcers();
  void signalMinMaxEnforcerChanged();

  int onAction(const char *action, const char *param=NULL, int x=-1, int y=-1, int p1=0, int p2=0, void *data=NULL, int datalen=0, RootWnd *source=NULL);

  int bypassModal();
  void setRenderBaseTexture(int r);
  int getRenderBaseTexture();

  GuiObject *getGuiObject(); // not guaranteed non null
  int getFlag(int flag);
  // see basewnd.h for codes. this is to force an event
  int triggerEvent(int event, int p1=0, int p2=0);

  // see wndevent.h for codes, this is where OS events are routed
  int windowEvent(int event, unsigned long p1=0, unsigned long p2=0, unsigned long p3=0, unsigned long p4=0);

  int allowDeactivation();
  void setAllowDeactivation(int allow);

  RootWnd *findWindow(const char *id);
  RootWnd *findWindowByInterface(GUID interface_guid);
  RootWnd *findWindowByCallback(FindObjectCallback *cb);
  RootWnd *findWindowChain(FindObjectCallback *cb, RootWnd *wcaller);

  RootWnd *enumTab(int i);
  int getNumTabs();

  int getFocusOnClick();
  void setFocusOnClick(int i);

  void setNoDoubleClicks(int no);
  void setNoLeftClicks(int no);
  void setNoMiddleClicks(int no);
  void setNoRightClicks(int no);
  void setNoMouseMoves(int no);
  void setNoContextMenus(int no);

  int wantDoubleClicks();
  int wantLeftClicks();
  int wantMiddleClicks();
  int wantRightClicks();
  int wantMouseMoves();
  int wantContextMenus();

  void setDefaultCursor(Cursor *c);

#ifdef WASABI_API_ACCESSIBILITY
  Accessible *getAccessibleObject(int createifnotexist=1);
  void detachAccessibleObject();

  const char *accessibility_getName();
  const char *accessibility_getValue();
  const char *accessibility_getDescription();
  int accessibility_getRole();
  int accessibility_getState();
  const char *accessibility_getHelp();
  const char *accessibility_getAccelerator();
  const char *accessibility_getDefaultAction();
  void accessibility_doDefaultAction();
  
  int accessibility_getNumItems();
  const char *accessibility_getItemName(int n);
  const char *accessibility_getItemValue(int n);
  const char *accessibility_getItemDescription(int n);
  int accessibility_getItemRole(int n);
  int accessibility_getItemState(int n);
  const char *accessibility_getItemHelp(int n);
  const char *accessibility_getItemAccelerator(int n);
  const char *accessibility_getItemDefaultAction(int n);
  void accessibility_doItemDefaultAction(int n);
  void accessibility_getItemRect(int n, RECT *r);
  int accessibility_getItemXY(int x, int y);
  RootWnd *accessibility_getStdObject(int objid);
  int accessibility_isLeaf();
#endif

  void enableFocusEvents(int a);
  int getFocusEventsEnabled();

#ifdef WASABI_PLATFORM_WIN32
#ifndef WA3COMPATIBILITY
  void setDropTarget(void *dt);
  void *getDropTarget();
#endif
#endif

#ifdef EXPERIMENTAL_INDEPENDENT_AOT
  void setAlwaysOnTop(int i);
  int getAlwaysOnTop();
#endif

  int getNCRole(); 
  void setNCRole(int ncrole); 

#ifdef WASABI_COMPILE_SCRIPT
  ScriptObject *getGuiScriptObject();
#endif

  int isMinimized();
  int isMaximized();
  void setNonBuffered(int nb);
  int isBuffered();

  enum {
    Event_SETVISIBLE=100,	// param 2 is 1 or 0
    Event_ONPAINT=200, // param 2 is PaintCallback::BEFOREPAINT or PaintCallback::AFTERPAINT
    Event_ONINVALIDATE=300,
  };

  enum {
    BATCHPROCESSES = 50,
    GETOSWINDOWHANDLE	= 100,
    GETOSMODULEHANDLE	= 105,
    GETROOTWNDNAME	= 110,
    GETID = 120,
    GETDRAGINTERFACE	= 200,
    FROMPOINT		= 300,
    GETWINDOWRECT	= 400,
    ISVISIBLE		= 501,
    GETSKINID		= 600,
    ONMETRICCHANGE	= 700,
    ONPAINT		= 800,
    PAINTTREE = 900,
    PAINT = 910,
    GETFRAMEBUFFER = 920,
    COMMITFRAMEBUFFER = 930,
    GETPARENT = 1000,
    GETROOTPARENT = 1001,
    SETPARENT = 1002,
    GETDESKTOPPARENT = 1003,
    ONDEFERREDCALLBACK = 1100,
    POSTDEFERREDCALLBACK = 1101,
    CHILDNOTIFY = 1200,
    BROADCASTNOTIFY = 1210,
    GETPREFERENCES = 1300,
    SETPREFERENCES = 1310,
    BEGINCAPTURE = 1400,
    ENDCAPTURE = 1401,
    GETCAPTURE = 1402,
    SETALLOWCAPTURE = 1403,
    ISCAPTUREALLOWED = 1404,
    DNDTHROUGH = 1499,
    CLICKTHROUGH = 1500,
    GETFORWARDWND = 1501,
    SETCLICKTHROUGH = 1502,
    CLIENTSCREEN = 1600,
    SCREENCLIENT = 1601,
    INIT = 1700,
    ISINITED = 1701,
    ISPOSTONINIT = 1702,
    INIT2 = 1703,
    SETVIRTUAL = 1704,
    GETCURSORTYPE = 1800,
    GETCUSTOMCURSOR = 1801,
    GETINTERFACE = 1850,
    GETCLIENTRECT = 1900,
    GETNONCLIENTRECT = 1901,
    SETVISIBLE = 2000,
    ONSETVISIBLE = 2001,
    SETCLOAKED = 2002,
    INVALIDATE = 2100,
    INVALIDATERECT = 2101,
    INVALIDATERGN = 2102,
    INVALIDATEFROM = 2103,
    INVALIDATERECTFROM = 2104,
    INVALIDATERGNFROM = 2105,
    VALIDATE = 2200,
    VALIDATERECT = 2201,
    VALIDATERGN = 2202,
    ONSIBINVALIDATE = 2300,
    WANTSIBINVALIDATE = 2301,
    ONCHILDINVALIDATE = 2302,
    CASCADEREPAINTFROM = 2400,
    CASCADEREPAINTRECTFROM = 2401,
    CASCADEREPAINTRGNFROM = 2402,
    CASCADEREPAINT = 2403, 
    CASCADEREPAINTRECT = 2405,
    CASCADEREPAINTRGN = 2406,
    REPAINT = 2500,
    GETTEXTUREWND = 2600,
    ONACTIVATE = 2700,
    ACTIVATE = 2710,
    ONDEACTIVATE = 2800,
    ISACTIVATED = 2801,
    GETPOSITION = 2900,
    GETREGION = 3000,
    GETREGIONOP = 3001,
    INVALWNDRGN = 3002,
    GETCOMPOSEDRGN = 3003,
    GETSUBTRACTORRGN = 3010,
    SETREGIONOP = 3004,
    SETRECTRGN = 3005,
    ISRECTRGN = 3006,
    GETDESKTOPALPHA = 3100,
    //SETPARAM = 3200, // deprecated, use the xmlobject interface
    HANDLETRANSPARENCY = 3300,
    HANDLEDESKTOPALPHA = 3400,
    SETSTARTHIDDEN = 3500,
    GETRENDERRATIO = 3600,
    SETRENDERRATIO = 3610,
    SETRATIOLINKED = 3615,
    HANDLERATIO = 3620,
    //_RESIZE = 3700, // deprecated, cut
    _RESIZE = 3701,
    _MOVE = 3710,
    CHECKDBLCLK = 3800,
    REGISTERROOTWNDCHILD = 3960,
    UNREGISTERROOTWNDCHILD = 3965,
    FINDROOTWNDCHILD= 3970,
    ENUMROOTWNDCHILDREN = 3975,
    GETNUMROOTWNDCHILDREN = 3980,
    ISVIRTUAL = 3950,
    BRINGVTOFRONT = 4000,
    BRINGVTOBACK = 4010,
    BRINGVABOVE = 4020,
    BRINGVBELOW = 4030,
    SETVCAPTURE = 4100,
    GETVCAPTURE = 4110,
    SETVTIMER = 4200,
    KILLVTIMER = 4210,
    PTINREGION = 4400,
    ONLBDBLCLK = 4500,
    ONMBDBLCLK = 4502,
    ONRBDBLCLK = 4510,
    ONLBUP = 4520,
    ONMBUP = 4522,
    ONRBUP = 4530,
    ONLBDOWN = 4540,
    ONMBDOWN = 4542,
    ONRBDOWN = 4550,
    ONMOUSEMOVE = 4560,
    CLIENTTOSCREEN = 4600,
    SCREENTOCLIENT = 4610,
    GETNOTIFYID = 4700,
    SETENABLED = 4800,
    ISENABLED = 4810,
    ONENABLE = 4811,
    SETALPHA = 4900,
    GETALPHA = 4910,
    GETPAINTALPHA = 4911,
    SETTOOLTIP = 4920,
    RUNMODAL = 5000,
    ENDMODAL = 5010,
    WANTAUTOCONTEXTMENU = 5100,
    ONCANCELCAPTURE = 5200,
    CANCELCAPTURE = 5210,
    BRINGTOFRONT = 5300,
    BRINGTOBACK = 5310,
    SETFOCUS = 5401,
    GOTFOCUS = 5410,
    GETNEXTVFOCUS = 5420,
    SETVFOCUS = 5430,
    ONKILLFOCUS = 5440,
    ONGETFOCUS = 5450,
    WANTFOCUS = 5460,
    EXCLUDEFROMTABLIST = 5461,
    ONKEYDOWN = 5500,
    ONKEYUP = 5510,
    ONCHAR = 5520,
    ONACCELERATOREVENT = 5530,
    GETTIMERCLIENT = 5600,
    GETDEPENDENCYPTR = 6000,
    ADDMINMAXENFORCER = 6400,
    REMOVEMINMAXENFORCER = 6410,
    GETNUMMINMAXENFORCERS = 6420,
    ENUMMINMAXENFORCER = 6430,
    SIGNALMINMAXCHANGED = 6440,
    ROOTWNDONACTION = 6300,
    BYPASSMODAL = 6500,
    SETRENDERBASETEXTURE = 6600,
    GETRENDERBASETEXTURE = 6610,
    GETGUIOBJECT = 6620,
    ONSYSKEYDOWN = 6630,
    ONSYSKEYUP = 6640,
    GETFLAG = 6650,
    TRIGGEREVENT = 6660,
    WINDOWEVENT = 6661,
    SETALLOWDEACTIVATE = 6700,
    ALLOWDEACTIVATE=6710,
    FINDWND_BYID=6800,
    FINDWND_BYGUID=6810,
    FINDWND_BYCB=6820,
    FINDWNDCHAIN=6830,
    SETTABORDER=6900,
    GETTABORDER=6910,
    SETAUTOTABORDER=6920,
    GETTAB=6940,
    SETVIRTUALTABORDER=7000,
    GETVIRTUALTABORDER=7010,
    SETVIRTUALAUTOTABORDER=7020,
    GETCURVIRTUALCHILDFOCUS=7030,
    FOCUSNEXT=7100,
    FOCUSPREVIOUS=7110,
    GETNUMTABS = 7200,
    ENUMTAB = 7210,
    ONSETROOTFOCUS = 7300,
    GETFOCUSONCLICK = 7400,
    SETFOCUSONCLICK = 7410,
    SETNODOUBLECLICKS = 7500,
    SETNOLEFTCLICKS =  7510,
    SETNOMIDDLECLICKS =  7512,
    SETNORIGHTCLICKS = 7520,
    SETNOMOUSEMOVES = 7530,
    SETNOCONTEXTMENUS = 7540,
    WANTDOUBLECLICKS = 7600,
    WANTLEFTCLICKS = 7610,
    WANTMIDDLECLICKS = 7612,
    WANTRIGHTCLICKS = 7620,
    WANTMOUSEMOVES = 7630,
    WANTCONTEXTMENUS = 7640,
    WANTACTIVATION = 7700,
    SETDEFAULTCURSOR = 7800,

#ifdef WASABI_API_ACCESSIBILITY
    GETACCESSIBLEOBJECT = 8000,
    DETACHACCESSIBLEOBJECT = 8010,

    ACCGETVALUE = 8101,
    ACCGETDESC = 8102,
    ACCGETROLE = 8103,
    ACCGETSTATE = 8104,
    ACCGETHELP = 8105,
    ACCGETACCELERATOR = 8106,
    ACCGETDEFACTION = 8107,
    ACCDODEFACTION = 8108,
    ACCGETNAME = 8109,

    ACCGETNUMITEMS = 8201,
    ACCGETITEMNAME = 8202,
    ACCGETITEMVALUE = 8203,
    ACCGETITEMDESC = 8204,
    ACCGETITEMROLE = 8205,
    ACCGETITEMSTATE = 8206,
    ACCGETITEMHELP = 8207,
    ACCGETITEMACCELERATOR = 8208,
    ACCGETITEMDEFACTION = 8209,
    ACCDOITEMDEFACTION = 8210,
    ACCGETITEMRECT = 8211,
    ACCGETITEMXY = 8212,
    ACCGETSTDOBJECT = 8213,
    ACCISLEAF = 8214,
#endif

    ENABLEFOCUSEVENTS = 8400,
    GETFOCUSEVENTSENABLED = 8401,

#ifdef EXPERIMENTAL_INDEPENDENT_AOT
    SETALWAYSONTOP = 8500,
    GETALWAYSONTOP = 8501,
#endif
    GETNCROLE = 8600,
    SETNCROLE = 8610,
#ifdef WASABI_COMPILE_SCRIPT
    GETGUISCRIPTOBJECT = 8700,
#endif
#ifndef WA3COMPATIBILITY
    GETDROPTARGET = 8800,
    SETDROPTARGET = 8810,
#endif
    ISMINIMIZED = 8900,
    ISMAXIMIZED = 8901,
    SETNONBUFFERED = 9000,
    ISBUFFERED = 9010,
  };
};

class NOVTABLE RootWndI : public RootWnd {
protected:
  RootWndI() {}	// protect constructor

public:
  virtual OSWINDOWHANDLE getOsWindowHandle()=0;
  virtual OSMODULEHANDLE getOsModuleHandle()=0;
  virtual void performBatchProcesses()=0;
  virtual TimerClient *getTimerClient()=0;
  virtual const char *getRootWndName()=0;
  virtual const char *getId()=0;
  virtual int init(RootWnd *parent, int nochild)=0;
  virtual int init2(OSMODULEHANDLE moduleHandle, OSWINDOWHANDLE parent, int nochild=FALSE)=0;
  virtual int isInited()=0;
  virtual int isPostOnInit()=0;

  virtual int setVirtual(int i)=0;

  virtual int isClickThrough()=0;
  virtual int isDndThrough()=0;
  virtual int onMouseMove(int x, int y)=0;
  virtual int onLeftButtonUp(int x, int y)=0;
  virtual int onMiddleButtonUp(int x, int y)=0;
  virtual int onRightButtonUp(int x, int y)=0;
  virtual int onLeftButtonDown(int x, int y)=0;
  virtual int onMiddleButtonDown(int x, int y)=0;  
  virtual int onRightButtonDown(int x, int y)=0;
  virtual int onLeftButtonDblClk(int x, int y)=0;
  virtual int onMiddleButtonDblClk(int x, int y)=0;
  virtual int onRightButtonDblClk(int x, int y)=0;

  virtual DragInterface *getDragInterface()=0;

  virtual int getCursorType(int x, int y)=0;
  virtual OSCURSORHANDLE getCustomCursor(int x, int y)=0;
  virtual RootWnd *rootWndFromPoint(POINT *pt, int throughtest=THROUGHTEST_NONE)=0;

  virtual void getClientRect(RECT *r)=0;
  virtual void getNonClientRect(RECT *rect)=0;
  virtual void getWindowRect(RECT *r)=0;

  virtual void setVisible(int show)=0;
  virtual void setCloaked(int cloak)=0;
  virtual void onSetVisible(int show)=0;
  virtual int isVisible(int within=0)=0;
  virtual int getSkinId()=0;

  virtual void *getInterface(GUID interface_guid)=0;
  virtual int onMetricChange(int metricid, int param1, int param2)=0;

  virtual void invalidate()=0;
  virtual void invalidateRect(RECT *r)=0;
  virtual void invalidateRgn(Region *r)=0;
  virtual void invalidateFrom(RootWnd *who)=0;
  virtual void invalidateRectFrom(RECT *r, RootWnd *who)=0;
  virtual void invalidateRgnFrom(Region *r, RootWnd *who)=0;
  virtual void validate()=0;
  virtual void validateRect(RECT *r)=0;
  virtual void validateRgn(Region *reg)=0;
  virtual void onChildInvalidate(Region *r, RootWnd *who)=0;

  virtual int rootwnd_onPaint(CanvasBase *canvas, Region *r)=0;
  virtual int rootwnd_paintTree(CanvasBase *canvas, Region *r)=0;
  virtual int paint (Canvas *canvas, Region *r)=0;
  virtual Canvas *getFrameBuffer()=0;
  virtual void commitFrameBuffer(Canvas *canvas, RECT *r, double ratio)=0;

  virtual RootWnd *getParent()=0;
  virtual RootWnd *getRootParent()=0;
  virtual RootWnd *getDesktopParent()=0;
  virtual void setParent(RootWnd *parent)=0;

  virtual int onSiblingInvalidateRgn(Region *r, RootWnd *who, int who_idx, int my_idx)=0;
  virtual int wantSiblingInvalidations()=0;

  virtual int cascadeRepaintFrom(RootWnd *who, int pack=1)=0;
  virtual int cascadeRepaintRgnFrom(Region *reg, RootWnd *who, int pack=1)=0;
  virtual int cascadeRepaintRectFrom(RECT *r, RootWnd *who, int pack=1)=0;
  virtual int cascadeRepaint(int pack=1)=0;
  virtual int cascadeRepaintRgn(Region *reg, int pack=1)=0;
  virtual int cascadeRepaintRect(RECT *r, int pack=1)=0;
  virtual void repaint()=0;

  virtual void setClickThrough(int ct)=0;
  virtual RootWnd *getBaseTextureWindow()=0;

  virtual int onDeferredCallback(int param1, int param2)=0;
  virtual void postDeferredCallback(int p1, int p2=0, int mindelay=0)=0;
  virtual int childNotify(RootWnd *child, int msg, int p1, int p2)=0;
  virtual void broadcastNotify(RootWnd *wnd, int msg, int p1, int p2)=0;
  virtual int getPreferences(int what)=0;
  virtual void setPreferences(int what, int v)=0;

  virtual Region *getRegion()=0;
  virtual int getRegionOp()=0;
  virtual void setRegionOp(int op)=0;
  virtual int isRectRgn()=0;
  virtual void setRectRgn(int rrgn)=0;
  virtual void invalidateWindowRegion()=0;
  virtual Region *getComposedRegion()=0;
  virtual Region *getSubtractorRegion()=0;

  virtual int getDesktopAlpha()=0;

  virtual void setStartHidden(int sh)=0;
  virtual double getRenderRatio()=0;
  virtual void setRenderRatio(double r)=0;
  virtual void setRatioLinked(int l)=0;
  virtual int handleRatio()=0;
  virtual void resize(int x, int y, int w, int h, int wantcb=1)=0;
  virtual void move(int x, int y)=0;

  virtual void getPosition(POINT *pt)=0;
  virtual RootWnd *getForwardWnd()=0;

  virtual void registerRootWndChild(RootWnd *child)=0;
  virtual void unregisterRootWndChild(RootWnd *child)=0;
  virtual RootWnd *findRootWndChild(int x, int y, int only_virtuals=0, int throughtest=THROUGHTEST_NONE)=0;
  virtual RootWnd *enumRootWndChildren(int _enum)=0;
  virtual int getNumRootWndChildren()=0;

  virtual int isVirtual()=0;

  virtual void bringVirtualToFront(RootWnd *w)=0;
  virtual void bringVirtualToBack(RootWnd *w)=0;
  virtual void bringVirtualAbove(RootWnd *w, RootWnd *b)=0;
  virtual void bringVirtualBelow(RootWnd *w, RootWnd *b)=0;
  virtual int checkDoubleClick(int button, int x, int y)=0;
  
  virtual int beginCapture()=0;
  virtual int endCapture()=0;
  virtual int getCapture()=0;
  virtual void onCancelCapture()=0;
  virtual void cancelCapture()=0;
  virtual void setAllowCapture(int allow)=0;
  virtual int isCaptureAllowed()=0;

  virtual void clientToScreen(int *x, int *y)=0; 
  virtual void screenToClient(int *x, int *y)=0; 

  virtual void setVirtualChildCapture(RootWnd *child)=0;
  virtual RootWnd *getVirtualChildCapture()=0;

  virtual int ptInRegion(int x, int y)=0;

  virtual int onActivate()=0;
  virtual void activate()=0;
  virtual int onDeactivate()=0;
  virtual int isActive()=0;
  virtual int handleTransparency()=0;
  virtual int handleDesktopAlpha()=0;

  virtual int getNotifyId()=0;
  virtual void setEnabled(int en)=0;
  virtual int isEnabled(int within=0)=0;
  virtual int onEnable(int e)=0;

  virtual int getPaintingAlpha()=0; 
  virtual void getAlpha(int *active=NULL, int *inactive=NULL)=0; 
  virtual void setAlpha(int activealpha, int inactivealpha=-1)=0; 

  virtual void setTip(const char *tip)=0;
  virtual int runModal()=0;
  virtual void endModal(int retcode)=0;
  virtual int wantAutoContextMenu()=0;
  virtual int wantActivation()=0;

  virtual void bringToFront()=0;
  virtual void bringToBack()=0;
  virtual void setFocus()=0;
  virtual int gotFocus()=0;
  virtual RootWnd *getNextVirtualFocus(RootWnd *w)=0;
  virtual void setVirtualChildFocus(RootWnd *w)=0;
  virtual void setVirtualTabOrder(RootWnd *w, int a)=0;
  virtual int getVirtualTabOrder(RootWnd *w)=0;
  virtual int wantFocus()=0;
  virtual int excludeFromTabList()=0;
  virtual void setTabOrder(int a)=0;
  virtual int getTabOrder()=0;
  virtual RootWnd *getTab(int what=TAB_GETCURRENT)=0;
  virtual void setAutoTabOrder()=0;
  virtual void setVirtualAutoTabOrder(RootWnd *w)=0;
  virtual RootWnd *getCurVirtualChildFocus()=0;
  virtual void onSetRootFocus(RootWnd *w)=0;

  virtual int onAcceleratorEvent(const char *name)=0;

  virtual int onChar(unsigned int c)=0;
  virtual int onKeyDown(int keycode)=0;
  virtual int onKeyUp(int keycode)=0;
  virtual int onSysKeyDown(int keyCode, int keyData)=0;
  virtual int onSysKeyUp(int keyCode, int keyData)=0;
  virtual int onKillFocus()=0;
  virtual int onGetFocus()=0;
  virtual Dependent *rootwnd_getDependencyPtr()=0;
  virtual void addMinMaxEnforcer(RootWnd *w)=0;
  virtual void removeMinMaxEnforcer(RootWnd *w)=0;
  virtual RootWnd *enumMinMaxEnforcer(int n)=0;
  virtual int getNumMinMaxEnforcers()=0;
  virtual void signalMinMaxEnforcerChanged()=0;

  virtual int onAction(const char *action, const char *param=NULL, int x=-1, int y=-1, int p1=0, int p2=0, void *data=NULL, int datalen=0, RootWnd *source=NULL)=0;

  virtual int bypassModal()=0;

  virtual void setRenderBaseTexture(int r)=0;
  virtual int getRenderBaseTexture()=0;
  virtual GuiObject *getGuiObject()=0; // not guaranteed non null

  virtual int getFlag(int flag)=0;
  virtual int triggerEvent(int event, int p1, int p2)=0;

  virtual int windowEvent(int event, unsigned long p1, unsigned long p2, unsigned long p3=0, unsigned long p4=0)=0;

  virtual int allowDeactivation()=0;
  virtual void setAllowDeactivation(int allow)=0;

  virtual RootWnd *findWindow(const char *id)=0;
  virtual RootWnd *findWindowByInterface(GUID interface_guid)=0;
  virtual RootWnd *findWindowByCallback(FindObjectCallback *cb)=0;
  virtual RootWnd *findWindowChain(FindObjectCallback *cb, RootWnd *wcaller)=0;

  virtual void focusNext()=0;
  virtual void focusPrevious()=0;

  virtual RootWnd *enumTab(int i)=0;
  virtual int getNumTabs()=0;

  virtual int getFocusOnClick()=0;
  virtual void setFocusOnClick(int i)=0;

  virtual void setNoDoubleClicks(int no)=0;
  virtual void setNoLeftClicks(int no)=0;
  virtual void setNoMiddleClicks(int no)=0;
  virtual void setNoRightClicks(int no)=0;
  virtual void setNoMouseMoves(int no)=0;
  virtual void setNoContextMenus(int no)=0;

  virtual int wantDoubleClicks()=0;
  virtual int wantLeftClicks()=0;
  virtual int wantMiddleClicks()=0;
  virtual int wantRightClicks()=0;
  virtual int wantMouseMoves()=0;
  virtual int wantContextMenus()=0;

  virtual void setDefaultCursor(Cursor *c)=0;

#ifdef WASABI_API_ACCESSIBILITY
  virtual Accessible *getAccessibleObject(int createifnotexist=1)=0;
  virtual void detachAccessibleObject()=0;

  virtual const char *accessibility_getName()=0;
  virtual const char *accessibility_getValue()=0;
  virtual const char *accessibility_getDescription()=0;
  virtual int accessibility_getRole()=0;
  virtual int accessibility_getState()=0;
  virtual const char *accessibility_getHelp()=0;
  virtual const char *accessibility_getAccelerator()=0;
  virtual const char *accessibility_getDefaultAction()=0;
  virtual void accessibility_doDefaultAction()=0;
  
  virtual int accessibility_getNumItems()=0;
  virtual const char *accessibility_getItemName(int n)=0;
  virtual const char *accessibility_getItemValue(int n)=0;
  virtual const char *accessibility_getItemDescription(int n)=0;
  virtual int accessibility_getItemRole(int n)=0;
  virtual int accessibility_getItemState(int n)=0;
  virtual const char *accessibility_getItemHelp(int n)=0;
  virtual const char *accessibility_getItemAccelerator(int n)=0;
  virtual const char *accessibility_getItemDefaultAction(int n)=0;
  virtual void accessibility_doItemDefaultAction(int n)=0;
  virtual void accessibility_getItemRect(int n, RECT *r)=0;
  virtual int accessibility_getItemXY(int x, int y)=0;
  virtual RootWnd *accessibility_getStdObject(int objid)=0;
  virtual int accessibility_isLeaf()=0;
#endif

  virtual void enableFocusEvents(int a)=0;
  virtual int getFocusEventsEnabled()=0;

#ifdef EXPERIMENTAL_INDEPENDENT_AOT
  virtual void setAlwaysOnTop(int i)=0;
  virtual int getAlwaysOnTop()=0;
#endif

  virtual int getNCRole()=0; 
  virtual void setNCRole(int ncrole)=0; 

#ifdef WASABI_COMPILE_SCRIPT
  virtual ScriptObject *getGuiScriptObject()=0;
#endif

#ifdef WASABI_PLATFORM_WIN32
#ifndef WA3COMPATIBILITY
  virtual void setDropTarget(void *dt)=0;
  virtual void *getDropTarget()=0;
#endif
#endif

  virtual int isMinimized()=0;
  virtual int isMaximized()=0;

  virtual void setNonBuffered(int nb)=0;
  virtual int isBuffered()=0;

protected:
  RECVS_DISPATCH;
};

#endif
