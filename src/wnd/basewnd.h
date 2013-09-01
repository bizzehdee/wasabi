#ifndef _BASEWND_H
#define _BASEWND_H

//#include "api/api_wnd.h"
#include <bfc/std.h>
#include <bfc/named.h>
#include <bfc/tlist.h>
#include <bfc/ptrlist.h>
#include <api/timer/api_timer.h>
#include <api/timer/timerclient.h>
#include "rootwnd.h"
#include "drag.h"

#ifdef MULTITHREADED_INVALIDATIONS
#include <bfc/critsec.h>
#endif

#include "findobjectcb.h"

class BaseWnd;
class BltCanvas;
class FilenameI;
class Canvas;
class CanvasBase;
class RegionI;
class DragSet;
class VirtualWnd;
class Tooltip;
class svc_toolTipsRenderer;
#ifdef WASABI_API_ACCESSIBILITY
class Accessible;
#endif

using namespace wasabi;	// GAY CUT ME

#define ROOTSTRING "RootWnd"
#define BASEWNDCLASSNAME "BaseWindow_" ROOTSTRING

// for triggerEvent()
#define TRIGGER_ONRESIZE	1000
#define TRIGGER_INVALIDATE	2000

// it is safe to use anything higher than this for your own funky messages
#define BASEWND_NOTIFY_USER	NUM_NOTIFY_MESSAGES

#define BASEWND_CURSOR_USERSET                  -1// window will do own setting
#define BASEWND_CURSOR_POINTER                  1
#define BASEWND_CURSOR_NORTHSOUTH               2
#define BASEWND_CURSOR_EASTWEST                 3
#ifdef WIN32 // don't use these. use _NORTHWEST, _SOUTHEAST, _NORTHEAST, _SOUTHWEST
# define BASEWND_CURSOR_NORTHWEST_SOUTHEAST      4
# define BASEWND_CURSOR_NORTHEAST_SOUTHWEST      5
#endif
#define BASEWND_CURSOR_4WAY                     6
#define BASEWND_CURSOR_EDIT                     7
#define BASEWND_CURSOR_NORTHWEST                8
#define BASEWND_CURSOR_SOUTHEAST                9
#define BASEWND_CURSOR_NORTHEAST                10
#define BASEWND_CURSOR_SOUTHWEST                11

// Our own defined window messages

#define WM_WA_CONTAINER_TOGGLED WM_USER+0x1338
#define WM_WA_COMPONENT_TOGGLED WM_USER+0x1339
#define WM_WA_RELOAD            WM_USER+0x133A
#define WM_WA_GETFBSIZE         WM_USER+0x133B

#define DEFERREDCB_FLUSHPAINT   0x200

#define REGIONOP_NONE (0)
#define REGIONOP_OR   (1)
#define REGIONOP_AND  (2)
#define REGIONOP_SUB  (-1)
#define REGIONOP_SUB2 (-2)

#define TABORDER_K (0.0001f)

#define AUTOWH 0xFFFE
#define NOCHANGE 0xFFFD

class virtualChildTimer {
public:
  virtualChildTimer(int _id, RootWnd *_child) : id(_id), child(_child) { }
  virtual ~virtualChildTimer() {}

  int id;
  RootWnd *child;
};

class RootWndFinder : public FindObjectCallbackI {
public:
  RootWndFinder() {  findobject_guid = INVALID_GUID; }
  virtual ~RootWndFinder() {  }

  void reset() { findobject_id = ""; findobject_guid = INVALID_GUID; }
  void setFindId(const char *id) { findobject_id = id; }
  void setFindGuid(GUID guid) { findobject_guid = guid; }

  virtual int findobjectcb_matchObject(RootWnd *w) {
    if (!findobject_id.isempty()) {
      if (w != NULL) {
        const char *id = w->getId();
        return STRCASEEQLSAFE(id, findobject_id);
      }
    } else if (findobject_guid != INVALID_GUID) {
      return (w->getInterface(findobject_guid) != NULL);
    }
    return 0;
  }

private:
  String findobject_id;
  GUID findobject_guid;
};

typedef struct {
  OSWINDOWHANDLE owner;
  OSWINDOWHANDLE hthis;
  PtrList<RootWnd> *hlist;
} enumownedstruct;

class WndWatcher: public DependentViewerI {
  public:
    WndWatcher(BaseWnd *b) : watched(NULL), dep(NULL), watcher(b) { ASSERT(watcher != NULL); }
    WndWatcher() : watched(NULL), dep(NULL), watcher(NULL) { }
    virtual ~WndWatcher() {}

    virtual void setWatcher(BaseWnd *w) { 
      if (watcher != NULL)
        watchWindow(NULL);
      watcher = w; 
    }

    virtual void watchWindow(RootWnd *w) { 
      ASSERT(watcher != NULL);
      if (dep != NULL) {
        viewer_delViewItem(dep);
        dep = NULL;
        watched = NULL;
      }
      if (w != NULL) {
        watched = w; 
        dep = w->getDependencyPtr(); 
        viewer_addViewItem(dep);
      }
    }
    
    virtual int viewer_onItemDeleted(Dependent *item);

  private:
    RootWnd *watched;
    Dependent *dep;
    BaseWnd *watcher;
};

BOOL CALLBACK EnumOwnedTopMostWindows(OSWINDOWHANDLE hwnd, LPARAM lParam);

class TabOrderEntry {
  public:
    RootWnd *wnd;
    float order; // yes, float. if a wnd tries to set order n and n is already set for another wnd, that other wnd will be pushed to n+k
                 // with k = 0.0001 . recursively, if n+k is taken, it'll push that wnd to n+2k, which if taken has its wnd pushed to n+3k, etc
                 // if n+xk >= n+1 (when x = 10000), the 10000th entry is discarded (if you manage to make a dialog with 10000 keyboard fields inside
                 // a single group, you're nuts anyway, and you should die a painful death)
};

class TabOrderSort {
public:
  static int compareItem(TabOrderEntry *p1, TabOrderEntry *p2) {
    if (p1->order < p2->order) return -1;
    if (p1->order > p2->order) return 1;
    return 0; 
  }
};

// base class
#define BASEWND_PARENT RootWndI
class NOVTABLE BaseWnd :
  public RootWndI,
  public DragInterface,
  public Named ,
  public DependentI, 
  public TimerClientI {

  friend class VirtualWnd;

protected:
  // override constructor to init your data, but don't create anything yet
  BaseWnd();	// this is protected because of NOVTABLE
public:
  virtual ~BaseWnd();

//INITIALIZATION
  // these actually create the window
  // try REALLY hard to not have to override these, and if you do,
  // override the second one
  virtual int init(RootWnd *parent, int nochild=FALSE);
  virtual int init(OSMODULEHANDLE hInstance, OSWINDOWHANDLE parent, int nochild=FALSE);
  virtual int init2(OSMODULEHANDLE hInstance, OSWINDOWHANDLE parent, int nochild=FALSE) { return init(hInstance, parent, nochild); }	//FUCKO
  virtual int isInited();	// are we post init() ?
  virtual int isPostOnInit() { return postoninit; }//FUCKO

  virtual int setVirtual(int i) { return 0; }//FUCKO

  // if at all possible put your init stuff in this one, and call up the
  // heirarchy BEFORE your code
  virtual int onInit();
  virtual int onPostOnInit(); // this one is called after onInit so you get a chance to do something after your inheritor override

  // use this to return the cursor type to show
  virtual int getCursorType(int x, int y);

// WINDOW SIZING/MOVEMENT/CONTROL
  virtual int getFontSize() { return 0; }
  virtual int setFontSize(int size) { return -1; }

  // if you override it, be sure to call up the heirarchy
  virtual void resize(int x, int y, int w, int h, int wantcb=1);	// resize yourself
  void resize(RECT *r, int wantcb=1);
  void resizeToRect(RECT *r) { resize(r); }//helper

  // called after resize happens, return TRUE if you override
  virtual int onResize();
  void resizeWndToClient(BaseWnd *wnd);	// resize a window to match you
  virtual int onPostedMove(); // whenever WM_WINDOWPOSCHANGED happens, use mainly to record positions when moved by the window tracker, avoid using for another prupose, not portable

  // move only, no resize
  virtual void move(int x, int y);

  // puts window on top of its siblings
  virtual void bringToFront();
  // puts window behind its siblings
  virtual void bringToBack();

  // fired when a sibbling invalidates a region. you can change the region before returning, use with care, can fuck up everything if not used well
  virtual int onSiblingInvalidateRgn(Region *r, RootWnd *who, int who_idx, int my_idx) { return 0; }

  // set window's visibility
  virtual void setVisible(int show);
  virtual void setCloaked(int cloak);
  virtual void onSetVisible(int show);	// override this one
  virtual void onChildInvalidate(Region *r, RootWnd *w) {}

  // enable/disable window for input
  virtual void setEnabled(int en);
  // grab the keyboard focus
  virtual void setFocus();
  virtual void setFocusOnClick(int f);
  virtual int getFocusOnClick() { return focus_on_click; }

  virtual int pointInWnd(POINT *p);

  // repaint yourself
  virtual void invalidate();	// mark entire window for repainting
  virtual void invalidateRect(RECT *r);// mark specific rect for repainting
  virtual void invalidateRgn(Region *r);// mark specific rgn for repainting
  virtual void invalidateFrom(RootWnd *who);	// mark entire window for repainting
  virtual void invalidateRectFrom(RECT *r, RootWnd *who);// mark specific rect for repainting
  virtual void invalidateRgnFrom(Region *r, RootWnd *who);// mark specific rgn for repainting
  virtual void validate(); // unmark the entire window from repainting
  virtual void validateRect(RECT *r); // unmark specific rect from repainting
  virtual void validateRgn(Region *reg); // unmark specific region from repainting

  // no virtual on these please
  void deferedInvalidateRgn(Region *h);
  void deferedInvalidateRect(RECT *r);
  void deferedInvalidate();
  void deferedValidateRgn(Region *h); 
  void deferedValidateRect(RECT *r);
  void deferedValidate();
  Region *getDeferedInvalidRgn();
  int hasVirtualChildren();
  // setVirtualChildFocus should not be called with NULL unless it is the current virtual child focus 
  // being deleted that's calling to remove its pointer from the basewnd parent, NULL will not generate 
  // an onKillFocus event in the child
  virtual void setVirtualChildFocus(RootWnd *w);
  virtual RootWnd *getNextVirtualFocus(RootWnd *w);
  void setVirtualTabOrder(RootWnd *w, int a);
  int getVirtualTabOrder(RootWnd *w);
  virtual void setTabOrder(int a);
  virtual int getTabOrder();
  virtual void setAutoTabOrder();
  virtual void setVirtualAutoTabOrder(RootWnd *w);
  virtual void focusNext();
  virtual void focusPrevious();
  virtual RootWnd *getCurVirtualChildFocus() { return curVirtualChildFocus; }
  virtual RootWnd *getTab(int what=TAB_GETNEXT);
 
private: 
  //virtual int focusVirtualChild(RootWnd *child);
  virtual void physicalInvalidateRgn(Region *r); 
  void autoFocus(RootWnd *w);
protected:
  virtual int ensureVirtualCanvasOk();
  virtual void setVirtualCanvas(Canvas *c);
  virtual void setRSize(int x, int y, int w, int h);

public:

  virtual void repaint();	// repaint right now!

  // override this to add decorations
  virtual void getClientRect(RECT *);
  RECT clientRect();	// helper
  virtual void getNonClientRect(RECT *);
  RECT nonClientRect();	// helper
  virtual void getWindowRect(RECT *);	// windows coords in screen system
  RECT windowRect();	// helper
  virtual void getPosition(POINT *pt); // window coord relative to os window (instead of rootparent)
  virtual void getBRPosition(POINT *pt); // window coord from bottom/right relative to os window (instead of rootparent)

  virtual void *getInterface(GUID interface_guid) { return NULL; }
  virtual void *dependent_getInterface(const GUID *classguid);

  virtual void clientToScreen(int *x, int *y);		// convenience fn
  virtual void screenToClient(int *x, int *y);		// convenience fn
  virtual void clientToScreen(POINT *pt);		// convenience fn
  virtual void screenToClient(POINT *pt);		// convenience fn
  virtual void clientToScreen(RECT *r);		// convenience fn
  virtual void screenToClient(RECT *r);		// convenience fn

  void setIcon(OSICONHANDLE icon, int _small);
  virtual void setSkinId(int id);

  virtual int getPreferences(int what);
  virtual void setPreferences(int what, int v);

  virtual void setStartHidden(int wtf);

  virtual void setDefaultCursor(Cursor *c);
  virtual Canvas *getFrameBuffer();

// from RootWnd
protected:
  virtual DragInterface *getDragInterface();
  virtual RootWnd *rootWndFromPoint(POINT *pt, int throughtest=THROUGHTEST_NONE);
  virtual int getSkinId();
  virtual int onMetricChange(int metricid, int param1, int param2);
  virtual int rootwnd_onPaint(CanvasBase *canvas, Region *r);
  virtual int rootwnd_paintTree(CanvasBase *canvas, Region *r);

  void assignRootFocus(RootWnd *w);

public:
  // override for custom processing (NONPORTABLE!)
  virtual LRESULT wndProc(OSWINDOWHANDLE hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void performBatchProcesses(); // this is called after wndProc is called (under win32) to recompute batch operations such as calculating window regions, cascaderepainting, etc. this prevents N children from independently calling repaintTree for the whole gui on overlaping zones of the framebuffer. under OSes other than win32, this should be called after you've executed all your window events for this poll, try not to use it, or use a dirty bit to cut down on your job
  virtual const char *getRootWndName();
  virtual const char *getId();
// end from RootWnd

  // or override these, they're more portable
  virtual int onContextMenu(int x, int y);	// always return 1 if accept msg
  virtual int onChildContextMenu(int x, int y);	// always return 1 if accept msg

  // called on WM_PAINT by onPaint(Canvas, Region *), if the canvas is null, create your own, if not, it will have update regions clipped for you
  virtual int onPaint(Canvas *canvas);
  void setTransparency(int amount);	// 0..255
  int getTransparency();

  // override those two
  virtual void timerCallback(int id);
  virtual int onDeferredCallback(int p1, int p2);

  int setTimer(int id, int ms);
  int killTimer(int id);
  virtual void postDeferredCallback(int p1, int p2=0, int mindelay=0);

  // from timerclient
  virtual int timerclient_onDeferredCallback(int p1, int p2);
  virtual void timerclient_timerCallback(int id);
  virtual TimerClient *timerclient_getMasterClient();
  virtual void timerclient_onMasterClientMultiplex();
  virtual TimerClient *getTimerClient();
  virtual const char *timerclient_getName();

private:
  virtual int onPaint(Canvas *canvas, Region *r);

public:  
  // click-drag. FYI, the drag-drop handling overrides these
  virtual int onLeftButtonDown(int x, int y); 
  virtual int onMiddleButtonDown(int x, int y) { return 0; }
  virtual int onRightButtonDown(int x, int y) { return 0; }

  virtual int onMouseMove(int x, int y);	// only called when mouse captured
  virtual int onLeftButtonUp(int x, int y);
  virtual int onMiddleButtonUp(int x, int y) { return 0; }
  virtual int onRightButtonUp(int x, int y) { return 0; }

  virtual int onMouseWheelUp(int click, int lines);
  virtual int onMouseWheelDown(int click, int lines);

  virtual int beginCapture();
  virtual int endCapture();
  virtual int getCapture();	// returns 1 if this window has mouse/keyb captured
  virtual void onCancelCapture(); // called when someone steals the capture from you
  virtual void cancelCapture();
  virtual void setAllowCapture(int allow);
  virtual int isCaptureAllowed();

  // these will not be called in the middle of a drag operation
  virtual int onLeftButtonDblClk(int x, int y) { return 0; }
  virtual int onMiddleButtonDblClk(int x, int y) { return 0; }
  virtual int onRightButtonDblClk(int x, int y) { return 0; }

  virtual int onGetFocus();	// you have the keyboard focus
  virtual int onKillFocus();	// you lost the keyboard focus
  virtual int gotFocus();
  virtual int isActive();
  virtual int onActivate();
  virtual void activate();
  virtual int onDeactivate();
  virtual int onEnable(int en);
  virtual int isEnabled(int within=0);

  virtual void registerAcceleratorSection(const char *name, int global=0);
  virtual int onAcceleratorEvent(const char *name);

  virtual int onChar(unsigned int c);
  virtual int onKeyDown(int keyCode) { return 0; }
  virtual int onKeyUp(int keyCode) { return 0; }
  virtual int onSysKeyDown(int keyCode, int keyData) { return 0; }
  virtual int onSysKeyUp(int keyCode, int keyData) { return 0; }

  virtual int onEraseBkgnd(HDC dc);// override and return 0 to authorize bkg painting, 1 to avoid it (default to 1)
  virtual int onUserMessage(int msg, int w, int l, int *r);

//CHILD->PARENT MESSAGES
  // feel free to override for your own messages, but be sure to call up the
  // chain for unhandled msgs
  // children should not call this directly if they don't have to; use
  // notifyParent on yourself instead
  // message ids should be put in notifmsg.h to avoid conflicts
  virtual int childNotify(RootWnd *child, int msg,
                          int param1=0, int param2=0);

  // call this to send a notification to all the children (and grandchildren (and grand-grand-children, (and... you get it)))
  virtual void broadcastNotify(RootWnd *wnd, int msg, int p1, int p2);

  // don't try to override these
  void setParent(RootWnd *newparent);
  RootWnd *getParent();
  virtual RootWnd *getRootParent(); 
  virtual RootWnd *getDesktopParent();
  
  // avoid overriding this one if you can
  virtual int reparent(RootWnd *newparent);

  // override this one
  virtual void onSetParent(RootWnd *newparent) {}

  virtual Region *getRegion(); // override to return your client region
  virtual void setRegionOp(int op); // 0 = none, 1 == or, 2=and, 3=xor, -1 = sub, -2 = sub thru groups
  virtual void setRectRgn(int i); // set to 1 if you don't want your region to clip your clicks

  virtual void invalidateWindowRegion(); // call this when your region has changed

  Region *getComposedRegion(); // returns the result of and/or/subing your children regions with your region
  Region *getSubtractorRegion(); // returns the composed subtracting region, that region is automatically subtracted from the desktop root parent's region
  int ptInRegion(int x, int y); // handled automatically if you override getRegion and isRectRgn, but you can still override it if you need
  virtual int getRegionOp(); //
  virtual int isRectRgn();

  // call this to notify your parent via its childNotify method
  virtual int notifyParent(int msg, int param1=0, int param2=0);

  // call this when you have received a childNotify and wish to
  // defer the notification to your own notify object.
  virtual int passNotifyUp(RootWnd *child, int msg,
                          int param1=0, int param2=0);

  // This allows you to set a custom integer ID for any object you control,
  // such that you can use its ID in a switch statement by calling getNotifyId()
  // which is dispatched through RootWnd.
  void setNotifyId(int id);
  virtual int getNotifyId();

  // from class Named
  virtual void onSetName();
  // non-virtuals only: sets the exported name of the OS window WITHOUT changing the Named member (getRootWndName() will not return this string)
  virtual void setOSWndName(const char *name);
  // non-virtuals only: retreive the exported name of the OS window. This is NOT the same as getRootWndName().
  virtual const char *getOSWndName();

  virtual const char *getTip();
  virtual void setTip(const char *tooltip);
  virtual int getStartHidden();
  virtual void abortTip();
  virtual int isVisible(int within=0);

	// Virtual windows functions
  virtual Canvas *createFrameBuffer(int w, int h);
  virtual void prepareFrameBuffer(Canvas *canvas, int w, int h);
  virtual void deleteFrameBuffer(Canvas *canvas);

  virtual void registerRootWndChild(RootWnd *child);
  virtual void unregisterRootWndChild(RootWnd *child);
  virtual RootWnd *findRootWndChild(int x, int y, int only_virtuals=0, int throughtest=THROUGHTEST_NONE);
  virtual RootWnd *enumRootWndChildren(int _enum);
  virtual int getNumRootWndChildren();

  virtual int isVirtual();
  virtual RootWnd *enumVirtualChild(int _enum);
  virtual int getNumVirtuals();

  virtual int handleVirtualChildMsg(UINT uMsg,int x, int y, void *p=NULL, void *d=NULL);
  virtual void setVirtualChildCapture(RootWnd *child);
  virtual RootWnd *getVirtualChildCapture();

  virtual int cascadeRepaintFrom(RootWnd *who, int pack=1);
  virtual int cascadeRepaintRgnFrom(Region *reg, RootWnd *who, int pack=1);
  virtual int cascadeRepaintRectFrom(RECT *r, RootWnd *who, int pack=1);
  virtual int cascadeRepaint(int pack=1);
  virtual int cascadeRepaintRgn(Region *reg, int pack=1);
  virtual int cascadeRepaintRect(RECT *r, int pack=1);
  virtual void flushPaint();

  virtual int allowMinimize() { return 1; } // override and return 0 to disable
  virtual int allowRestore() { return 1; }  // override and return 0 to disable
  virtual int allowMaximize() { return 1; } // override and return 0 to disable

  virtual void onMinimize();	// call ancestor
  virtual void onRestore();	// call ancestor
  virtual void onMaximize();	// call ancestor

  virtual int isMinimized();
  virtual int isMaximized();

  virtual void freeResources();
  virtual void reloadResources();
  virtual int getDesktopAlpha();
  virtual int handleDesktopAlpha() { return isVirtual(); }
  virtual int getPaintingAlpha();           // this one is a hint for painting, it returns either activealpha or inactivealpha
  virtual void setAlpha(int activealpha, int inactivealpha=-1); // -1 means same as activealpha
  virtual void getAlpha(int *activealpha=NULL, int *inactivealpha=NULL);
  virtual int getFlag(int flag);
  virtual int triggerEvent(int event, int p1, int p2);

  virtual int windowEvent(int event, unsigned long p1, unsigned long p2, unsigned long p3=0, unsigned long p4=0);
  virtual void commitFrameBuffer(Canvas *canvas, RECT *r, double ratio);

  virtual int paint(Canvas *canvas, Region *r);

protected:
  virtual void do_flushPaint();
  virtual int paintTree(Canvas *canvas, Region *r);
  virtual int virtualBeforePaint(Region *r);
  virtual int virtualAfterPaint(Region *r);
  virtual int virtualOnPaint();
  virtual void setDesktopAlpha(int do_alpha);
  virtual void onSetDesktopAlpha(int a);

public:
	
  virtual OSWINDOWHANDLE getOsWindowHandle();
  virtual OSMODULEHANDLE getOsModuleHandle();

public:
	
  BOOL getNoCopyBits(void);
  void setNoCopyBits(BOOL ncb);
  BltCanvas *scalecanvas;

protected:
  virtual int checkDoubleClick(int button, int x, int y);

//MISC
public:
  virtual int isDestroying();	// in the middle of dying

//DRAGGING AND DROPPING -- (derived from DragInterface)

  // derived windows should call this if they detect a drag beginning
  // call once per datum per type of data being exposed. order is maintained
  int addDragItem(const char *droptype, void *item);
  // returns TRUE if drop was accepted
  int handleDrag();
  int resetDragSet();	// you don't need to call this

  // (called on dest) when dragged item enters the winder
  virtual int dragEnter(RootWnd *sourceWnd);
  // (called on dest) during the winder
  // FG> x/y are in screen corrdinates because target is a rootwnd
  virtual int dragOver(int x, int y, RootWnd *sourceWnd) { return 0; }
  // (called on src)
  virtual int dragSetSticky(RootWnd *wnd, int left, int right, int up, int down);
  // (called on dest) when dragged item leaves the winder
  virtual int dragLeave(RootWnd *sourceWnd) { return 0; }

  // when it finally is dropped:
 
  // called on destination window
  // FG> x/y are in screen corrdinates because target is a rootwnd
  virtual int dragDrop(RootWnd *sourceWnd, int x, int y) { return 0; }
  // called on source window
  virtual int dragComplete(int success) { return 0; }
  // in that order
  // must be called right before handleDrag();		(sender)
  void setSuggestedDropTitle(const char *title);

  // must be called from dragDrop();			(receiver)
  virtual const char *dragGetSuggestedDropTitle(void);
  virtual int dragCheckData(const char *type, int *nitems=NULL);
  virtual void *dragGetData(int slot, int itemnum);
  virtual const char *dragGetType(int slot);
  virtual int dragGetNumTypes();
  virtual int dragCheckOption(int option) { return 0; }

  // return TRUE if you support any of the datatypes this window is exposing
  virtual int checkDragTypes(RootWnd *sourceWnd) { return 0; }

// external drops 
  // override this and return 1 to receive drops from the OS
  virtual int acceptExternalDrops() { return 0; }

  virtual void onExternalDropBegin() {}
  virtual void onExternalDropDirScan(const char *dirname) {}
  virtual void onExternalDropEnd() {}

  virtual int bypassModal();

  virtual RootWnd *findWindow(const char *id);
  virtual RootWnd *findWindowByInterface(GUID interface_guid);
  virtual RootWnd *findWindowByCallback(FindObjectCallback *cb);
  virtual RootWnd *findWindowChain(FindObjectCallback *cb, RootWnd *wcaller=NULL);
  // pt in screen coords
  static RootWnd *rootWndFromScreenPoint(const POINT *pt, int throughtest=THROUGHTEST_NONE);

protected:
  virtual int callDefProc(OSWINDOWHANDLE wnd, UINT msg, int wparam, int lparam);
  virtual int wantEraseBackground() { return 0; }

private:
  void addDroppedFile(const char *filename, PtrList<FilenameI> *plist); // recursive
  void setLayeredWindow(int i);
#ifdef WASABI_API_ACCESSIBILITY
  Accessible *createNewAccObj();
#endif

public:
//FG> alternate notify window
  virtual void setNotifyWindow(RootWnd *newnotify);
  virtual RootWnd *getNotifyWindow();

  virtual double getRenderRatio();
  virtual void setRenderRatio(double r);
  virtual void setRatioLinked(int l);
  virtual int handleRatio();
  int renderRatioActive();
  void multRatio(int *x, int *y=NULL);
  void multRatio(RECT *r);
  void divRatio(int *x, int *y=NULL);
  void divRatio(RECT *r);
  virtual int isClickThrough();
  virtual int isDndThrough() { return FALSE; }
  virtual void setClickThrough(int ct);
  virtual RootWnd *getForwardWnd() { return this; }

  virtual void setNoLeftClicks(int no);
  virtual void setNoMiddleClicks(int no);
  virtual void setNoRightClicks(int no);
  virtual void setNoDoubleClicks(int no);
  virtual void setNoMouseMoves(int no);
  virtual void setNoContextMenus(int no);

  // these functions are override that can be changed via XML. They are not intended to describe how your wnd should receive its messages, they are here rather
  // to allow a skinner to disable part of the functionality of an object (ie: removing the context menu via nocontextmenu="1").
  virtual int wantDoubleClicks() { return !nodoubleclick; }
  virtual int wantLeftClicks() { return !noleftclick; }
  virtual int wantMiddleClicks() { return !nomiddleclick; }
  virtual int wantRightClicks() { return !norightclick; }
  virtual int wantMouseMoves() { return !nomousemove; }
  virtual int wantContextMenus() { return !nocontextmnu; } 


// DERIVED WINDOW BEHAVIORAL PREFERENCES
protected:
  // return 1 to get onMouseOver even if mouse isn't captured
  virtual int wantSiblingInvalidations();

  virtual int wantFocus();
  virtual int excludeFromTabList() { return 0; }
  virtual int wantAutoContextMenu(); // return 1 if you want to auto popup the main app context menu

protected: 

  void onTipMouseMove();
  void renderBaseTexture(CanvasBase *c, const RECT &r, int alpha=255);

  int getTabOrderEntry(RootWnd *w);
  void delTabOrderEntry(int i);
  int getTabOrderEntry(float order);
  void delTabOrderEntry(RootWnd *w);

  virtual OSCURSORHANDLE getCustomCursor(int x, int y);

public:
  virtual RootWnd *getBaseTextureWindow();
  void setBaseTextureWindow(RootWnd *w);
  virtual int isMouseOver(int x, int y);

  virtual void bringVirtualToFront(RootWnd *w);
  virtual void bringVirtualToBack(RootWnd *w);
  virtual void bringVirtualAbove(RootWnd *w, RootWnd *b);
  virtual void bringVirtualBelow(RootWnd *w, RootWnd *b);
  void changeChildZorder(RootWnd *w, int newpos);

  virtual int handleTransparency(); // return 0 if you use overlay mode to render your stuff
  virtual int runModal();
  virtual int exec() { return runModal(); }
  virtual void endModal(int ret);

  Dependent *rootwnd_getDependencyPtr();
  Dependent *timerclient_getDependencyPtr();

  virtual void signalMinMaxEnforcerChanged(void);
  virtual void onMinMaxEnforcerChanged(void) {}
  virtual void addMinMaxEnforcer(RootWnd *w);
  virtual void removeMinMaxEnforcer(RootWnd *w);
  virtual RootWnd *enumMinMaxEnforcer(int n);
  virtual int getNumMinMaxEnforcers();
  virtual int onAction(const char *action, const char *param=NULL, int x=-1, int y=-1, int p1=0, int p2=0, void *data=NULL, int datalen=0, RootWnd *source=NULL);
  virtual int sendAction(RootWnd *target, const char *action, const char *param=NULL, int x=0, int y=0, int p1=0, int p2=0, void *data=NULL, int datalen=0);

  virtual void setRenderBaseTexture(int r);
  virtual int getRenderBaseTexture();

  virtual GuiObject *getGuiObject();

  void setAutoResizeAfterInit(int tf) { want_autoresize_after_init = tf; }
  virtual void setAllowDeactivation(int allow) { allow_deactivate = allow; }
  virtual int allowDeactivation();

  int getNumTabs();
  RootWnd *enumTab(int i);
  virtual void onSetRootFocus(RootWnd *w);

  virtual int wantActivation() { return 1; } // return 0 if you don't want activation upon click

#ifdef WASABI_API_ACCESSIBILITY
  virtual Accessible *getAccessibleObject(int createifnotexists=1);
  virtual void detachAccessibleObject();

  virtual const char *accessibility_getName();
  virtual const char *accessibility_getValue();
  virtual const char *accessibility_getDescription();
  virtual int accessibility_getRole();
  virtual int accessibility_getState();
  virtual const char *accessibility_getHelp();
  virtual const char *accessibility_getAccelerator();
  virtual const char *accessibility_getDefaultAction();
  virtual void accessibility_doDefaultAction();
  
  virtual int accessibility_getNumItems();
  virtual const char *accessibility_getItemName(int n);
  virtual const char *accessibility_getItemValue(int n);
  virtual const char *accessibility_getItemDescription(int n);
  virtual int accessibility_getItemRole(int n);
  virtual int accessibility_getItemState(int n);
  virtual const char *accessibility_getItemHelp(int n);
  virtual const char *accessibility_getItemAccelerator(int n);
  virtual const char *accessibility_getItemDefaultAction(int n);
  virtual void accessibility_doItemDefaultAction(int n);
  virtual void accessibility_getItemRect(int n, RECT *r);
  virtual int accessibility_getItemXY(int x, int y);
  virtual RootWnd *accessibility_getStdObject(int objid);
  virtual int accessibility_isLeaf();
#endif

  virtual void enableFocusEvents(int a) { focusEventsEnabled = a; }
  virtual int getFocusEventsEnabled() { return focusEventsEnabled; }
#ifdef EXPERIMENTAL_INDEPENDENT_AOT
  virtual void setAlwaysOnTop(int i);
  virtual int getAlwaysOnTop();
#endif
  virtual void wndwatcher_onDeleteWindow(RootWnd *w);

  virtual int getNCRole();
  virtual void setNCRole(int ncrole);
  virtual void setOSModuleHandle(OSMODULEHANDLE module) { hinstance = module; }

#ifdef WASABI_COMPILE_SCRIPT
  virtual ScriptObject *getGuiScriptObject();
#endif

#ifdef WASABI_PLATFORM_WIN32
#ifndef WA3COMPATIBILITY
  virtual void setDropTarget(void *dt);
  virtual void *getDropTarget();
#endif
#endif

public:
  void setMargins(int left, int top, int right, int bottom);
  // most of the time you would not need to call this, since clientRect already applies the margins,
  // however, for things like getPreferences(SUGGESTED_W/H), you might.
  void getMargins(int *left, int *top=NULL, int *right=NULL, int *bottom=NULL);


protected:

  void setForeignWnd(int i); // set to 1 if this basewnd was wrapped around an OSWINDOWHANDLE 
                             // this means mainly that the destructor will not destroy the window handle.

  void updateCursor();

protected:
  // ATTENTION: note the capitalization on these -- so as not to mix up with
  // wndProc()'s hWnd
  OSMODULEHANDLE hinstance;
  OSWINDOWHANDLE hwnd;

private:
  RootWnd *parentWnd;

  int inputCaptured;
  int allowCapture;

  void onTab();
  void recursive_setVirtualTabOrder(RootWnd *w, float a, float lambda=TABORDER_K);
  void recursive_buildTabList(RootWnd *from, PtrList<RootWnd> *list);

protected:

  void dropVirtualCanvas();
  int bufferizeLockedUIMsg(int uMsg, int wParam, int lParam);
  void checkLockedUI();
  int checkModal();
  void hintDestroying() { destroying=TRUE; } // call that in your destructor if you'd otherwise generate virtual calls after your destructor
  virtual int forceTransparencyFlag();

  int dragging;

  Canvas *virtualCanvas;

  virtual void updateWindowRegion();
  int isWindowRegionValid() { return !wndregioninvalid; }
  virtual int wantRedrawOnResize() { return 1; }
  virtual int ensureWindowRegionValid();
  int disable_tooltip_til_recapture;

  void setNonBuffered(int nb);
  int isBuffered() { return buffered; }
  virtual const char *getWindowClass() { return BASEWNDCLASSNAME; }

protected:
  void handleDragOver(int x, int y);
  void handleDragDrop(int x, int y);

private:
#ifdef EXPERIMENTAL_INDEPENDENT_AOT
  void saveTopMosts();
  void restoreTopMosts();
#endif
  virtual void onBeforePaintTree(Canvas *c, Region *r);
  void _cascadeRepaintRgn(Region *rg);
  void packCascadeRepaintRgn(Region *rg);
  int createTip();       
  void destroyTip();     // called in destructor, do not override
  PtrList<DragSet> dragsets;
  RootWnd *prevtarg;
  int prevcandrop;
  String suggestedTitle;

  typedef struct {
    int msg;
    int wparam;
    int lparam;
  } bufferedMsgStruct;
  
  TList<bufferedMsgStruct> bufferedmsgs;
  int uiwaslocked;

  //FG
  int start_hidden;
  svc_toolTipsRenderer *tipsvc;
  String tip;
  int tipid;
  BOOL tip_done;
  BOOL tipawaytimer;
  BOOL tipshowtimer;
  BOOL tipbeenchecked;
  BOOL tipdestroytimer;

  //FG
  BOOL ncb;
  DWORD lastClick[2];
  POINT lastClickP[2];

  //FG
  RootWnd *btexture;

  //FG
  RootWnd *notifyWindow;
  BOOL destroying;

  //CT:Virtual children
  PtrList<RootWnd> virtualChildren;
  PtrList<RootWnd> rootwndchildren; //FG
  int virtualCanvasHandled;
  int virtualCanvasH,virtualCanvasW;
  int rwidth, rheight, rx, ry;
  RootWnd *curVirtualChildCaptured;

	//FG>
  RegionI *deferedInvalidRgn;

  OSWINDOWHANDLE oldCapture;
  
  int hasfocus;
  RootWnd *curVirtualChildFocus;

  double ratio;
  int skin_id;
  int wndalpha;
  int w2k_alpha;
  int curframerate;

  int notifyid;
  int activealpha;
  int inactivealpha;
  int clickthrough;
  int triedtipsvc;
  int mustquit;
  int returnvalue;
  int postoninit;
  int inited;
  int skipnextfocus;
  RegionI *subtractorrgn, *composedrgn;
  void setWindowRegion(Region *r);
  void computeComposedRegion();
  virtual void assignWindowRegion(Region *wr);
  int wndregioninvalid;
  int rectrgn, regionop;
  RegionI *deferedCascadeRepaintRgn;
  int need_flush_cascaderepaint;
  Tooltip *tooltip;
  PtrList<RootWnd> minmaxEnforcers;
  int this_visible;
  int renderbasetexture;
  GuiObject *my_guiobject;
  int want_autoresize_after_init;
  int resizecount;
  double lastratio;
  int suggested_w, suggested_h;
  int default_x;
  int default_y;
  int maximum_w, maximum_h;
  int minimum_w, minimum_h;
  int allow_deactivate;
  int minimized, maximized;
  int wasmaximized;
  int deleting;
  int preventcancelcapture;
  int inonresize;
  String tcname;
  int focus_on_click;
  PtrListQuickSorted<TabOrderEntry, TabOrderSort> childtabs;
  RootWnd *rootfocus;
  int ratiolinked;
  int nodoubleclick, noleftclick, nomiddleclick, norightclick, nomousemove, nocontextmnu;
  Cursor *customdefaultcursor;
#ifdef WASABI_API_ACCESSIBILITY
  Accessible *accessible;
#endif
  String osname;
  int focusEventsEnabled;
  PtrList<RootWnd> ontoplist;
  int alwaysontop;
  WndWatcher rootfocuswatcher;
  int ncrole;
  int cloaked;
  int m_takenOver;
  int this_enabled;
  int has_alpha_flag;
  RECT *commitfb_rect;
  int buffered;
  LRESULT (*m_oldWndProc)(OSWINDOWHANDLE hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#ifdef MULTITHREADED_INVALIDATIONS
  static CriticalSection s_invalidation_cs;
#endif
#ifndef WA3COMPATIBILITY
  void *m_target;
#endif
  int margin[4];
};

#ifdef _WIN64
#define GETHINSTANCE GWLP_HINSTANCE
#else
#define GETHINSTANCE GWL_HINSTANCE
#endif

#ifdef WIN32
__inline HINSTANCE HINSTANCEfromHWND(OSWINDOWHANDLE wnd) {
  if (wnd == NULL) return NULL;
  return (OSMODULEHANDLE)GetWindowLong(wnd, GETHINSTANCE);
}
#endif

#endif
