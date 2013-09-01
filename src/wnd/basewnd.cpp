#include "precomp.h"

//#define USE_TOOLTIPS

#include "basewnd.h"

#ifdef WASABI_API_ACCESSIBILITY
#include "accessible.h"
#endif

#include "canvas.h"
#include "bltcanvas.h"
#include "cursor.h"
#include <bfc/file/filename.h>
#ifdef XWINDOW
#include <parse/pathparse.h>
#endif
#include "metricscb.h"
#include "notifmsg.h"
#include "paintcb.h"
#include "region.h"
#ifdef USE_TOOLTIPS
#include "tooltip.h"
#endif
#include "usermsg.h"
#include "wndevent.h"

#include <bfc/std.h>
#include <bfc/std_wnd.h>

#include <bfc/assert.h>
#ifdef WASABI_API_ACCESSIBILITY
#include <api/service/svcs/svc_accessibility.h>
#endif
#include <api/service/svcenum.h>

//?#include <common/guiobjwnd.h>
#ifdef WASABI_COMPILE_SKIN
#include <api/script/scriptguid.h>
#endif
//?#include <studio/api.h>

#if UTF8
# include <bfc/string/encodedstr.h>
# include <bfc/file/readdir.h>
#endif

//CUT? #include <studio/gc.h>
#include <api/syscb/callbacks/gccb.h>

#ifdef WIN32
#include <shellapi.h>	// for HDROP
  #ifndef WM_MOUSEWHEEL
  #define WM_MOUSEWHEEL 0x20A
  #endif
#endif

#ifdef WASABI_DRAW_FOCUS_RECT
#include <studio/paintsets.h>
#endif

#define DESKTOPALPHA
#define REFRESH_RATE 25
#define DRAWTIMERID 125

#define TIP_TIMER_ID        1601
#define TIP_DESTROYTIMER_ID 1602
#define TIP_AWAY_ID         1603
#define TIP_AWAY_DELAY       100

#define TIP_TIMER_THRESHOLD  350
#define TIP_LENGTH          3000

#define VCHILD_TIMER_ID_MIN	2000
#define VCHILD_TIMER_ID_MAX	2100

#define BUFFEREDMSG_TIMER_ID 1604

#define DEFERREDCB_INVALIDATE   0x201 // move to .h
#define DEFERREDCB_FOCUSFIRST   0x202 // move to .h

class DragSet : public PtrList<void>, public Named {};

//CUT? #define ROOTSTRING "RootWnd"

//CUT? #define BASEWNDCLASSNAME "BaseWindow_" ROOTSTRING

static RootWnd *stickyWnd;
static RECT sticky;
#ifdef MULTITHREADED_INVALIDATIONS
CriticalSection BaseWnd::s_invalidation_cs;
#endif

/*RootWnd *RootWnd::rootwndFromPoint(POINT &point, int level) {
  RootWnd *wnd;
  wnd = WASABI_API_WND->rootWndFromPoint(&point);
  return RootWnd::rootwndFromRootWnd(wnd, level, &point);
}

RootWnd *RootWnd::rootwndFromRootWnd(RootWnd *wnd, int level, POINT *point) {

  for (;;) {
    if (wnd == NULL || level < 0) return NULL;
    if (point) {
      RECT r;
      wnd->getWindowRect(&r);
      if (!PtInRect(&r, *point)) return NULL; // PORT ME
    }
    if (level == 0) return wnd; 
    wnd = wnd->getRootWndParent();
    level--;
  }
  // should never get here
}*/

int WndWatcher::viewer_onItemDeleted(Dependent *item) {
  if (item == dep) {
    dep = NULL;
    watcher->wndwatcher_onDeleteWindow(watched);
    watched = NULL;
  }
  return 1;
}

// this is the static wndProc. it calls the RootWnd::wndProc() it gets
// from the RootWnd* in the HWND userdata
LRESULT CALLBACK staticWndProc(OSWINDOWHANDLE hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  // fetch out the RootWnd *
#ifdef WIN32
  if (uMsg == WM_CREATE) {
    CREATESTRUCT *cs = (CREATESTRUCT *)lParam;
    ASSERT(cs->lpCreateParams != NULL);
    // stash pointer to self
    SetWindowLong(hWnd, GWL_USERDATA, (LONG)cs->lpCreateParams);
  }
#endif

  // pass the messages into the BaseWnd
  RootWnd *rootwnd = reinterpret_cast<RootWnd*>(GetWindowLong(hWnd, GWL_USERDATA));
  if (rootwnd == NULL || rootwnd->getOsWindowHandle() != hWnd) {
#ifdef WASABI_PLATFORM_WIN32
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
#else
    return 0;
#endif
  } else {
    int r = rootwnd->wndProc(hWnd, uMsg, wParam, lParam);
    if (StdWnd::isValidWnd(hWnd)) // wndProc may have switched skin and killed this window
      rootwnd->performBatchProcesses();
    return r;
  }
}

BaseWnd::BaseWnd() {
  m_oldWndProc = NULL;
  buffered = 1;
  uiwaslocked = 0;
  m_takenOver = 0;
  rootfocus = NULL;
  rootfocuswatcher.setWatcher(this);
  alwaysontop = 0;
  customdefaultcursor = NULL;
  preventcancelcapture = 0;
  ratiolinked = 1;
  deleting = 0;
  hinstance = INVALIDOSMODULEHANDLE;
  hwnd = INVALIDOSWINDOWHANDLE;
  parentWnd = NULL;
  dragging = 0;
  prevtarg = NULL;
  prevcandrop = 0;
  inputCaptured = 0;
  allowCapture = 1;
  btexture = NULL;
  postoninit = 0;
  inited = 0;
  skipnextfocus = 0;
  ncb=FALSE;
#ifdef WASABI_API_ACCESSIBILITY
  accessible = NULL;
#endif

  tooltip = NULL;
  tip_done = FALSE;
  tipshowtimer = FALSE;
  tipawaytimer = FALSE;
  tipdestroytimer = FALSE;
  start_hidden = 0;
  notifyWindow = NULL;
  lastClick[0] = 0;
  lastClick[1] = 0;
  lastClickP[0].x = 0;
  lastClickP[0].y = 0;
  lastClickP[1].x = 0;           
  lastClickP[1].y = 0;
  destroying = FALSE;

  curVirtualChildCaptured = NULL;
  curVirtualChildFocus = NULL;

  virtualCanvas = NULL; virtualCanvasH = virtualCanvasW = 0;
  deferedInvalidRgn = NULL; 

  hasfocus = 0;
  focus_on_click = 1;

  ratio = 1;
  lastratio = 1;
  rwidth=rheight=0;
  skin_id = -1;
  wndalpha = 255;
  activealpha = 255;
  inactivealpha = 255;
  w2k_alpha = 0;//FUCKO
  scalecanvas = NULL;
  clickthrough = 0;

  mustquit = 0;
  returnvalue = 0;
  notifyid = 0;
  cloaked = 0;
  disable_tooltip_til_recapture = 0;

  subtractorrgn = NULL;
  composedrgn = NULL;
  wndregioninvalid = 1;
  regionop = REGIONOP_NONE;
  rectrgn = 1;
  need_flush_cascaderepaint = 0;
  deferedCascadeRepaintRgn = NULL;
  this_visible = 0;
  this_enabled = 1;
  renderbasetexture = 0;
  oldCapture = INVALIDOSWINDOWHANDLE;
  my_guiobject = NULL;
  want_autoresize_after_init = 0;
  resizecount=0;
  suggested_w=AUTOWH;
  suggested_h=AUTOWH;
  maximum_w=maximum_h=AUTOWH;
  minimum_w=minimum_h=AUTOWH;
  rx = 0;
  ry = 0;
  rwidth = 0;
  rheight = 0;            
  allow_deactivate = 1;
  minimized = maximized = 0;
  wasmaximized = 0;
  inonresize = 0;
#ifndef WA3COMPATIBILITY
  m_target = NULL;
#endif

  nodoubleclick = noleftclick = nomiddleclick = norightclick = nomousemove = nocontextmnu = 0;
  focusEventsEnabled = 1;
#ifdef WIN32
  ncrole = HTNOWHERE;
#endif
  margin[0] = margin[1] = margin[2] = margin[3] = 0;
}

BaseWnd::~BaseWnd() {
  ASSERT(deleting == 0);
  deleting = 1;
  //ASSERT(virtualChildren.getNumItems() == 0);
  childtabs.deleteAll();
#ifdef WASABI_COMPILE_WND
  if (WASABI_API_WND->getModalWnd() == this) WASABI_API_WND->popModalWnd(this);
#endif
  destroying=TRUE;
  curVirtualChildFocus = NULL;
  if (inputCaptured && StdWnd::getCapture() == getOsWindowHandle()) StdWnd::releaseCapture();

  if (hwnd != INVALIDOSWINDOWHANDLE && !m_takenOver) {
#ifdef URLDROPS
    if (acceptExternalDrops()) StdWnd::revokeDragNDrop(hwnd/*, &m_target*/);
#else
#ifndef WA3COMPATIBILITY
    if (m_target != NULL) {
      StdWnd::revokeDragNDrop(hwnd);
    }
#endif
#endif

#ifdef WASABI_COMPILE_WND
    int popact=!wantActivation();
    if (popact) WASABI_API_WND->appdeactivation_push_disallow(this);
#endif

    StdWnd::destroyWnd(hwnd);

#ifdef WASABI_COMPILE_WND
    if (popact) WASABI_API_WND->appdeactivation_pop_disallow(this);
#endif
  }

  deleteFrameBuffer(virtualCanvas);
  virtualCanvas=NULL;
  delete scalecanvas;
  scalecanvas = NULL;

  resetDragSet();

  notifyParent(ChildNotify::DELETED);
  if (tipdestroytimer)
    killTimer(TIP_DESTROYTIMER_ID);
  if (tipshowtimer)
    killTimer(TIP_TIMER_ID);
  if (tipawaytimer)
    killTimer(TIP_AWAY_ID);

  destroyTip();

//?  delete tooltip;

  if (uiwaslocked)
    killTimer(BUFFEREDMSG_TIMER_ID);

  if (deferedInvalidRgn)
    delete deferedInvalidRgn;

  delete composedrgn;
  delete subtractorrgn;
  delete deferedCascadeRepaintRgn;

  if (parentWnd != NULL)
    parentWnd->unregisterRootWndChild(this);

#ifdef WASABI_COMPILE_WND
  if (!m_takenOver) WASABI_API_WND->unregisterRootWnd(this);
#endif
  hwnd = INVALIDOSWINDOWHANDLE;
}

int BaseWnd::init(RootWnd *parWnd, int nochild) {
  if (parWnd == NULL) {
    return init(WASABI_API_APP->main_gethInstance(), NULL, TRUE);
  }
  if (parWnd == NULL) return 0;
  OSWINDOWHANDLE phwnd = parWnd->getOsWindowHandle();
  ASSERT(phwnd != INVALIDOSWINDOWHANDLE);
  int ret;
  parentWnd = parWnd;	// set default parent wnd
  ret = init(parWnd->getOsModuleHandle(), phwnd, nochild);
  if (!ret) parentWnd = NULL;	// abort
  return ret;
}

int BaseWnd::init(OSMODULEHANDLE moduleHandle, OSWINDOWHANDLE parent, int nochild) {
  RECT r;
  int w, h;

  ASSERTPR(getOsWindowHandle() == INVALIDOSWINDOWHANDLE, "don't you double init you gaybag");

  hinstance = moduleHandle;

  //ASSERT(hinstance != INVALIDOSMODULEHANDLE);

  if (parent != INVALIDOSWINDOWHANDLE) {
    StdWnd::getClientRect(parent, &r);
  } else {
    r.left = default_x;
    int w = getPreferences(SUGGESTED_W);
    if (w == AUTOWH) w = 320;
    r.right = default_x + w;
    r.top = default_y;
    int h = getPreferences(SUGGESTED_H);
    if (h == AUTOWH) h = 200;
    r.bottom = default_y + h;
  }

  w = (r.right - r.left);
  h = (r.bottom - r.top);

  rwidth = w;
  rheight = h;
  rx = r.left;
  ry = r.top;

#ifdef WASABI_COMPILE_WND
  int popact=!wantActivation();
  if (popact) WASABI_API_WND->appdeactivation_push_disallow(this);
#endif

  const char *wndclass = getWindowClass();
#if defined(WASABI_PLATFORM_WIN32)
  int subclasswndproc = !STRCASEEQL(wndclass, BASEWNDCLASSNAME);

  hwnd = StdWnd::createWnd(r.left, r.top, w, h, nochild, acceptExternalDrops(),
    parent, hinstance, wndclass, ::staticWndProc,
    (unsigned long)static_cast<RootWnd*>(this), FALSE, wndalpha);
#elif defined(WASABI_PLATFORM_XWINDOW)
  hwnd = StdWnd::createWnd(r.left, r.top, w, h, nochild, acceptExternalDrops(),
    parent, hinstance, wndclass, (unsigned long)static_cast<RootWnd*>(this));
#else
#error port me!
#endif // platform

#ifdef WASABI_PLATFORM_WIN32
  if (subclasswndproc) {
    m_oldWndProc = (LRESULT (*)(HWND, UINT, WPARAM, LPARAM))SetWindowLong(gethWnd(), GWL_WNDPROC, (LONG)staticWndProc);
    if ((LONG)m_oldWndProc == (LONG)staticWndProc) m_oldWndProc = NULL;
    else SetWindowLong(gethWnd(), GWL_USERDATA, (LONG)this);
  }
#endif

#ifdef WASABI_COMPILE_WND
  if (popact) WASABI_API_WND->appdeactivation_pop_disallow(this);
#endif

  //ASSERT(hwnd != NULL); // lets fail nicely, this could happen for some win32 reason, we don't want to fail the whole app for it, so lets just fail the wnd
  if (hwnd == INVALIDOSWINDOWHANDLE) return 0;

  if (wantActivation()) bringToFront();

#ifdef WASABI_COMPILE_WND //FUCKO
#ifdef URLDROPS
  if (acceptExternalDrops()) RegisterDragDrop(hwnd, &m_target);
#else
#ifndef WA3COMPATIBILITY
  if (!m_target && WASABI_API_WND != NULL) 
    m_target = WASABI_API_WND->getDefaultDropTarget();
  if (m_target != NULL) {
    RegisterDragDrop(hwnd, (IDropTarget *)m_target);
  }
#endif
#endif
#endif

  this_visible = 0;

  onInit();

  this_visible = !start_hidden;

  onPostOnInit();

  return 1;
}

#ifdef WASABI_PLATFORM_WIN32
#ifndef WA3COMPATIBILITY
void BaseWnd::setDropTarget(void *dt) {
  if (isVirtual()) return;
  if (isInited() && m_target != NULL) {
    StdWnd::revokeDragNDrop(getOsWindowHandle());
    m_target = NULL;
  }
  m_target = dt;
  if (m_target != NULL && isInited()) {
    RegisterDragDrop(gethWnd(), (IDropTarget *)m_target);
  }
}

void *BaseWnd::getDropTarget() {
  return m_target;
}
#endif
#endif

int BaseWnd::onInit() {

  const char *s = getName();
  if (s != NULL)
    setOSWndName(s);

  ASSERTPR(!inited, "onInit() called twice");
  ASSERT(!postoninit);
  inited = 1;

  if (getParent())
    getParent()->registerRootWndChild(this);

#ifdef WASABI_COMPILE_WND 
  if (WASABI_API_WND != NULL) 
    WASABI_API_WND->registerRootWnd(this);
#endif

  if (!StdWnd::isDesktopAlphaAvailable()) 
    w2k_alpha = 0; //FUCKO

  if (w2k_alpha) {
    setLayeredWindow(1);
  }

  return 0;
}

int BaseWnd::onPostOnInit() {
  ASSERT(inited);
  ASSERTPR(!postoninit, "onPostOnInit() called twice");
  postoninit=1; // from now on, isInited() returns 1;
  if (want_autoresize_after_init) onResize(); 
  else invalidateWindowRegion();
  if (isVisible()) onSetVisible(1); 
  if (getTabOrder() == -1) setAutoTabOrder();
  RootWnd *dp = getDesktopParent();
#ifdef WASABI_API_TIMER  
  if ((dp == NULL || dp == this) && WASABI_API_TIMER != NULL) 
    postDeferredCallback(DEFERREDCB_FOCUSFIRST, 0, 500);
#endif
  return 0;
}

void BaseWnd::setLayeredWindow(int i) {
  if (!StdWnd::isValidWnd(getOsWindowHandle())) return;
  if (!isInited()) return;
  StdWnd::setLayeredWnd(getOsWindowHandle(), i);
  setTransparency(-1);
}

int BaseWnd::getCursorType(int x, int y) {
  if (!customdefaultcursor)
    return BASEWND_CURSOR_POINTER;
  return BASEWND_CURSOR_USERSET;
}

void BaseWnd::onSetName() {
  if (isInited() && !isVirtual())
    StdWnd::setWndName(getOsWindowHandle(), getNameSafe());
  notifyParent(ChildNotify::NAMECHANGED);
#ifdef WASABI_API_ACCESSIBILITY
  if (accessible) accessible->onSetName(getName());
#endif
}

OSWINDOWHANDLE BaseWnd::getOsWindowHandle() {
  return isVirtual() ? getParent()->getOsWindowHandle() : hwnd;
}

OSMODULEHANDLE BaseWnd::getOsModuleHandle() {
  return hinstance;
}

void BaseWnd::timerCallback(int id) { 
  switch (id) {
    case BUFFEREDMSG_TIMER_ID:
      checkLockedUI();
      break;
    case TIP_TIMER_ID:

      tipshowtimer=FALSE;
      tip_done = TRUE;
      killTimer(TIP_TIMER_ID);
  
      POINT p;
      StdWnd::getMousePos(&p.x, &p.y);
#ifdef WASABI_COMPILE_WND 
      if (WASABI_API_WND->rootWndFromPoint(&p) == (RootWnd *)this) {
        createTip();
        setTimer(TIP_DESTROYTIMER_ID, TIP_LENGTH);
        tipdestroytimer=TRUE;
      }
#endif
      setTimer(TIP_AWAY_ID, TIP_AWAY_DELAY);
      tipawaytimer=TRUE;
      break;
    case TIP_DESTROYTIMER_ID:
      killTimer(TIP_DESTROYTIMER_ID);
      killTimer(TIP_AWAY_ID);
      tipawaytimer=FALSE;
      tipdestroytimer=FALSE;
      destroyTip();
      break;
    case TIP_AWAY_ID:
      onTipMouseMove();
      break;
  }
}

int BaseWnd::isInited() {
  return inited;
}

int BaseWnd::isDestroying() {
  return destroying;
}

int BaseWnd::wantSiblingInvalidations() {
  return FALSE;
}

void BaseWnd::setRSize(int x, int y, int w, int h) {
  rwidth = w;
  rheight = h;
  rx = x;
  ry = y;
}

void BaseWnd::resize(int x, int y, int w, int h, int wantcb) {
  if (!isInited()) {
    if (w != NOCHANGE)
      suggested_w = w;
    if (h != NOCHANGE)
      suggested_h = h;
    if (x != NOCHANGE)
      default_x = x;
    if (y != NOCHANGE)
      default_y = y;
    return;
  }

  inonresize = 1;

  if (getNumMinMaxEnforcers() > 0) {
    int min_w = getPreferences(MINIMUM_W);
    int min_h = getPreferences(MINIMUM_H);
    int max_w = getPreferences(MAXIMUM_W);
    int max_h = getPreferences(MAXIMUM_H);
    if (min_w != AUTOWH && w < min_w) w = min_w;
    if (max_w != AUTOWH && w > max_w) w = max_w;
    if (min_h != AUTOWH && h < min_h) h = min_h;
    if (max_h != AUTOWH && h > max_h) h = max_h;
  }

  int noresize = (w == NOCHANGE && h == NOCHANGE);
  int nomove = (x == NOCHANGE && y == NOCHANGE);
  if (x == NOCHANGE) x = rx;
  if (y == NOCHANGE) y = ry;
  if (w == NOCHANGE) w = rwidth;
  if (h == NOCHANGE) h = rheight;

  double thisratio = getRenderRatio();
  int updwndregion = 0;
  int different_ratio = (lastratio != thisratio);
  lastratio = thisratio;

  int noevent = (resizecount > 1 && w == rwidth && h == rheight);
  if (different_ratio == 1 && noevent == 1) {
    if (StdWnd::getTopmostChild(getOsWindowHandle()) != INVALIDOSWINDOWHANDLE)
      noevent = 0;
    invalidateWindowRegion();
  }

  RECT oldsize, newsize={x,y,w,h};
  if (hwnd != INVALIDOSWINDOWHANDLE) 
    BaseWnd::getNonClientRect(&oldsize);

  setRSize(x, y, w, h);
  
  int enlarge=0;
  if (handleRatio() && renderRatioActive()) {
    enlarge=1;
    multRatio(&w, &h);
    if (getParent() != NULL) {
      multRatio(&x, &y);
    }
  }

  if (getOsWindowHandle() != INVALIDOSWINDOWHANDLE) {

    RECT oldsizescaled;
    getWindowRect(&oldsizescaled);
    RECT newsizescaled={x,y,x+w,y+h};
    if (!Std::rectEqual(newsizescaled, oldsizescaled)) {
//CUT      SetWindowPos(getOsWindowHandle(), NULL, x, y, w, h,
//CUT      SWP_NOZORDER |
//CUT      SWP_NOACTIVATE |
//CUT      (!wantRedrawOnResize() ? SWP_NOCOPYBITS: 0) |
//CUT      (ncb ? SWP_NOCOPYBITS : 0) | 
//CUT      ( nomove ? SWP_NOMOVE : 0) |
//CUT      ( noresize ? SWP_NOSIZE : 0) |
//CUT      0);
      StdWnd::setWndPos(getOsWindowHandle(), INVALIDOSWINDOWHANDLE, x, y, w, h,
        TRUE, TRUE, !wantRedrawOnResize() || ncb, nomove, noresize);
    } else {
      //DebugString("BaseWnd::resize optimized\n");
    }

    if (ncb) {
      invalidate();
    } else {
      RECT r;
      if (hwnd != INVALIDOSWINDOWHANDLE) {
        if (newsize.left == oldsize.left && newsize.top == oldsize.top) {
          if (newsize.right > oldsize.right) { // growing in width
            r.left = oldsize.right;
            r.right = newsize.right;
            r.top = newsize.top;
            r.bottom = newsize.bottom;
            invalidateRect(&r);
            if (newsize.bottom > oldsize.bottom) { // growing in width & height
              r.left = oldsize.left;
              r.right = newsize.right;
              r.top = oldsize.bottom;
              r.bottom = newsize.bottom;
              invalidateRect(&r);
            }
          } else if (newsize.bottom > oldsize.bottom) {
            if (newsize.bottom > oldsize.bottom) { // growing in height
              r.left = oldsize.left;
              r.right = newsize.right;
              r.top = oldsize.bottom;
              r.bottom = newsize.bottom;
              invalidateRect(&r);
            }
          }
        }
      }
    }
  }

  if (!noevent) {
    if (wantcb && isPostOnInit()) {
      resizecount = MIN(5, ++resizecount);
      if (!isVirtual())
        invalidateWindowRegion();
      onResize();
      if (ensureWindowRegionValid())
        updateWindowRegion();
    }
  }

  inonresize = 0;
}

int BaseWnd::onResize() {
  if (!isVirtual() || (getRegionOp() != REGIONOP_NONE))
    invalidateWindowRegion();
  // you are not supposed to call onResize until after onInit has returned. If what you wanted was to generate
  // an onResize event to do some custom client coordinates recalculations (ie: to apply on your children)
  // then you don't need to do anything since onResize is going to be called after onInit() is done. If you still want to
  // trigger it because your code might be called by onInit and after onInit, use isPostOnInit() as a test.
  // if what you wanted was to signal a object that you just resized it, then you don't need to do anything beside
  // resize(...), it will generate the event on its own if the window is inited, and will defer to until after onInit
  // if it is not.
  // shortly put: do not call onResize before or inside onInit()
  // if you have any valid reason for doing that, i'd like to know about it so i can make it possible. -FG
#ifdef _DEBUG
  if (!isPostOnInit()) {
    ASSERTPR(isPostOnInit(), "do not call onResize before or inside onInit()");
  }
#endif
  return FALSE;
}

void BaseWnd::resizeWndToClient(BaseWnd *wnd) {
  if (wnd != NULL)
    wnd->resize(&clientRect());
}

int BaseWnd::onPostedMove() {
  return FALSE;
}

void BaseWnd::resize(RECT *r, int wantcb) {
  resize(r->left, r->top, r->right-r->left, r->bottom-r->top, wantcb);
}

void BaseWnd::move(int x, int y) {
  setRSize(x, y, rwidth, rheight);
  StdWnd::setWndPos(getOsWindowHandle(), INVALIDOSWINDOWHANDLE, x, y, 0, 0,
    TRUE, TRUE, ncb, FALSE, TRUE);
//CUT  if (!ncb)
//CUT    SetWindowPos(getOsWindowHandle(), NULL, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_DEFERERASE); 
//CUT  else
//CUT    SetWindowPos(getOsWindowHandle(), NULL, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOACTIVATE|SWP_DEFERERASE);
}

#ifdef EXPERIMENTAL_INDEPENDENT_AOT
BOOL CALLBACK EnumOwnedTopMostWindows(OSWINDOWHANDLE hwnd, LPARAM lParam) {
  enumownedstruct *st = (enumownedstruct *)lParam;
  if (hwnd != st->hthis && GetWindow(hwnd, GW_OWNER) == st->owner) {
    RootWnd *w = (RootWnd*)GetWindowLong(hwnd, GWL_USERDATA);
    if (w != NULL && w->getAlwaysOnTop())
      st->hlist->addItem(w);
  }
  return TRUE;
}

void BaseWnd::saveTopMosts() {
  OSWINDOWHANDLE owner = GetWindow(getOsWindowHandle(), GW_OWNER);
  enumownedstruct st;
  ontoplist.removeAll();
  if (owner != NULL) {
    st.owner = owner;
    st.hlist = &ontoplist;
    st.hthis = getOsWindowHandle();
    EnumWindows(EnumOwnedTopMostWindows, (long)&st);
  }
}

void BaseWnd::restoreTopMosts() {
  OSWINDOWHANDLE owner = GetWindow(getOsWindowHandle(), GW_OWNER);
  if (owner != NULL) {
    for (int i=0;i<ontoplist.getNumItems();i++) {
      ontoplist.enumItem(i)->setAlwaysOnTop(1);
    }
  }
}
#endif

void BaseWnd::bringToFront() {
  // when we set a window to the top of the zorder (not topmost), win32 finds the owner and removes any topmost flag its children may
  // have because it assumes we want this window over these, which we definitly don't. so we need to first go thru all the owner's children,
  // make a list of the ones with a topmost flag, set this window on top, and set the topmost flags back. yay 
  ASSERT(!isVirtual());
#ifdef EXPERIMENTAL_INDEPENDENT_AOT
  saveTopMosts();
#endif
//CUT  SetWindowPos(getOsWindowHandle(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE|SWP_DEFERERASE|SWP_NOOWNERZORDER);
  StdWnd::bringToFront(getOsWindowHandle());
#ifdef EXPERIMENTAL_INDEPENDENT_AOT
  restoreTopMosts();
#endif
}

void BaseWnd::bringToBack() {
  ASSERT(!isVirtual());
#ifdef EXPERIMENTAL_INDEPENDENT_AOT
  saveTopMosts(); 
#endif
//CUT  SetWindowPos(getOsWindowHandle(), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE|SWP_DEFERERASE|SWP_NOOWNERZORDER);
  StdWnd::sendToBack(getOsWindowHandle());
#ifdef EXPERIMENTAL_INDEPENDENT_AOT
  restoreTopMosts();
#endif
}

void BaseWnd::setVisible(int show) {
  int visible = isVisible(1);
  if (!!visible == !!show) return;
  invalidate();
  this_visible = !!show;
  /*if (!getParent() || getParent() == WASABI_API_WND->main_getRootWnd() && IsWindow(getOsWindowHandle())) {
    if (!show) {
      setLayeredWindow(0);
      if (setLayeredWindowAttributes)
        setLayeredWindowAttributes(hwnd, RGB(0,0,0), 255, LWA_ALPHA);
    } else {
      setLayeredWindow(w2k_alpha);
    }
  }*/
  if (!getParent() ||
#ifdef WASABI_COMPILE_WND
      getParent() == WASABI_API_WND->main_getRootWnd() ||
#endif
      getParent()->isVisible()) {
    onSetVisible(show);
  }
}

void BaseWnd::setCloaked(int cloak) {
  if (cloaked == cloak) return;
  cloaked = cloak;
  if (isVirtual()) return;
  if (cloaked) {
    //CUTif (IsWindowVisible(getOsWindowHandle()))
    //CUT  ShowWindow(getOsWindowHandle(), SW_HIDE);
    if (StdWnd::isWndVisible(getOsWindowHandle()))
      StdWnd::hideWnd(getOsWindowHandle());
  } else {
    if (isVisible(1))
      //CUTShowWindow(getOsWindowHandle(), SW_NORMAL);
      StdWnd::showWnd(getOsWindowHandle());
  }
}


void BaseWnd::onSetVisible(int show) {
/* for debug purposes - don't delete please  
  #include "../../../studio/container.h"
  #include "../../../studio/layout.h"
  if (!show && getGuiObject() && STRCASEEQLSAFE(getGuiObject()->guiobject_getId(), "normal")) {
    Layout *l = (Layout *)getInterface(layoutGuid);
    if (l) {
      if (l->getParentContainer() && STRCASEEQLSAFE(l->getParentContainer()->getId(), "main")) {
        DebugString("Hiding main player\n");
      }
    }
  }*/
  if (!isVirtual()) 
    if (hwnd != INVALIDOSWINDOWHANDLE) 
      if (!cloaked) {
//CUT //      SetWindowPos(getOsWindowHandle(),NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER|SWP_SHOWWINDOW);
//CUT        ShowWindow(getOsWindowHandle(), show ? SW_SHOWNA : SW_HIDE);
        if (show) StdWnd::showWnd(getOsWindowHandle(), TRUE);
        else StdWnd::hideWnd(getOsWindowHandle());
      }
/*  if (!show)
    postDeferredCallback(0x7849);
  else {*/
    foreach(rootwndchildren)
      RootWnd *w = rootwndchildren.getfor();
      if (w->isVisible(1)) // check internal flag only
        w->onSetVisible(show);
    endfor;
    dependent_sendEvent(RootWnd::depend_getClassGuid(), Event_SETVISIBLE, show);
  //}
/*  if (getDesktopParent() == this) {
    cascadeRepaint(0);
  }*/

/*#ifdef WIN32 // os-specific non virtual child wnd support
  if (!isVirtual()) {
    HWND w = GetWindow(getOsWindowHandle(), GW_CHILD);
    while (w != NULL) {
      RootWnd *rootwnd = (RootWnd*)GetWindowLong(w, GWL_USERDATA);
      if (rootwnd && rootwnd != this)
        if (rootwnd->isInited())
          rootwnd->onSetVisible(show);
      w = GetWindow(w, GW_HWNDNEXT);
    }
  }
#endif*/
  if (!isVirtual()) {
    if (!show) {
      deferedInvalidate();
      delete virtualCanvas;
      virtualCanvas = NULL;
    }
  }
  invalidateWindowRegion();
}

void BaseWnd::setEnabled(int en) {
  int enabled = isEnabled(1);
  if (!!enabled == !!en) return;
  invalidate();
  this_enabled = !!en;
  if (!getParent() ||
#ifdef WASABI_COMPILE_WND
      getParent() == WASABI_API_WND->main_getRootWnd() ||
#endif
      getParent()->isEnabled()) {
    onEnable(en);
  }
}

int BaseWnd::isEnabled(int within) {
  if (!isVirtual() && !getOsWindowHandle()) return 0;
  if (!this_enabled) return 0; 

  if (within) return this_enabled; // whatever, local

  if (isVirtual()) // virtual, global
    if (getParent())
      return getParent()->isEnabled();
    else
      return 0;
  
  // non virtual, global
//CUT  if (GetWindowLong(getOsWindowHandle(), GWL_STYLE) & WS_POPUP) return this_enabled;
  if (StdWnd::isPopup(getOsWindowHandle())) return this_enabled;
//CUT  if (!StdWnd::isValidWnd(GetParent(gethWnd()))) return this_enabled;
  if (!StdWnd::isValidWnd(StdWnd::getParent(getOsWindowHandle()))) return this_enabled;
  if (getParent()) return getParent()->isEnabled(); // not a popup, check its parent or fail
  return this_enabled;
}

int BaseWnd::onEnable(int en) {
  if (!isVirtual()) {
    if (hwnd != INVALIDOSWINDOWHANDLE) 
//CUT      EnableWindow(getOsWindowHandle(), en);
      StdWnd::setEnabled(getOsWindowHandle(), en);
    foreach(rootwndchildren)
      RootWnd *w = rootwndchildren.getfor();
      if (w->isEnabled(1)) // check internal flag only
        w->onEnable(en);
    endfor;
  }
  return 1;
}

void BaseWnd::setFocus() {
  if (curVirtualChildFocus != NULL) {
    curVirtualChildFocus->onKillFocus();
    curVirtualChildFocus = NULL;
  }
  onSetRootFocus(this);
//CUT  SetFocus(getOsWindowHandle());
  StdWnd::setFocus(getOsWindowHandle());
}

void BaseWnd::setFocusOnClick(int f) {
  focus_on_click = f;
}

Region *BaseWnd::getDeferedInvalidRgn() {
  return deferedInvalidRgn;
}

void BaseWnd::deferedInvalidate() {
  if (!hasVirtualChildren() || !isVisible(1)) return; 
  RECT r={0,0,0,0};
  getNonClientRect(&r);
  deferedInvalidateRect(&r);
}

void BaseWnd::deferedInvalidateRect(RECT *r) {
  if (!hasVirtualChildren()) return; 
  RegionI h(r);
  deferedInvalidateRgn(&h);
}

void BaseWnd::deferedInvalidateRgn(Region *h) {
  if (!hasVirtualChildren()) return; 
#ifdef MULTITHREADED_INVALIDATIONS
  INCRITICALSECTION(s_invalidation_cs);
#endif
  if (!deferedInvalidRgn) {
    deferedInvalidRgn = new RegionI();
  }

  deferedInvalidRgn->addRegion(h);
}

void BaseWnd::deferedValidate() {
  if (!hasVirtualChildren() || !isVisible(1)) return; 
  RECT r={0,0,0,0};
  getNonClientRect(&r);
  deferedValidateRect(&r);
}

void BaseWnd::deferedValidateRect(RECT *r) {
  if (!hasVirtualChildren()) return; 
  RegionI h(r);
  deferedValidateRgn(&h);
}

void BaseWnd::deferedValidateRgn(Region *h) {
  if (!hasVirtualChildren()) return; 
  if (!deferedInvalidRgn) return;

#ifdef MULTITHREADED_INVALIDATIONS
  INCRITICALSECTION(s_invalidation_cs);
#endif
  deferedInvalidRgn->subtractRgn(h);
}

int BaseWnd::hasVirtualChildren() {
  return 1; //virtualChildren.getNumItems() > 0;
}

void BaseWnd::invalidate() {
  invalidateFrom(this);
}

void BaseWnd::invalidateFrom(RootWnd *who) {
  if (hasVirtualChildren()) deferedInvalidate();
//CUT  if (hwnd != NULL && isVisible(1)) InvalidateRect(getOsWindowHandle(), NULL, FALSE);
  if (hwnd != INVALIDOSWINDOWHANDLE && isVisible(1)) StdWnd::invalidateRect(getOsWindowHandle());
}

void BaseWnd::invalidateRectFrom(RECT *r, RootWnd *who) {
  if (hasVirtualChildren()) deferedInvalidateRect(r);
  RegionI rg(r);
  invalidateRgnFrom(&rg, who);
}

void BaseWnd::invalidateRgn(Region *r) {
  invalidateRgnFrom(r, this);
}

void BaseWnd::invalidateRect(RECT *r) {
  invalidateRectFrom(r, this);
}

void BaseWnd::invalidateRgnFrom(Region *r, RootWnd *who) {
#ifdef MULTITHREADED_INVALIDATIONS
  INCRITICALSECTION(s_invalidation_cs);
#endif
  if (parentWnd) parentWnd->onChildInvalidate(r, who);
  PaintCallbackInfoI pc(NULL, r);
  dependent_sendEvent(RootWnd::depend_getClassGuid(), Event_ONINVALIDATE, 0, &pc);
  static int inhere=0;
  if (hwnd != INVALIDOSWINDOWHANDLE && isVisible(1)) {
    if (hasVirtualChildren()/* && !inhere*/) {
      inhere=1;
      Region *_r = r->clone();
      int j = virtualChildren.searchItem(who);
      for (int i=0;i<virtualChildren.getNumItems();i++) {
        RootWnd *w = virtualChildren[i];
        if (w != who && w->wantSiblingInvalidations())
          w->onSiblingInvalidateRgn(_r, who, j, i);
      }

      inhere=0;
 
      if (buffered) deferedInvalidateRgn(_r);
      physicalInvalidateRgn(_r);
      r->disposeClone(_r);
    } else {
      if (buffered) deferedInvalidateRgn(r);
      physicalInvalidateRgn(r);
    }
  }
}

void BaseWnd::physicalInvalidateRgn(Region *r) {
  if (hwnd != INVALIDOSWINDOWHANDLE && isVisible(1)) {
    if (renderRatioActive()) {
      Region *clone = r->clone();
      clone->scale(getRenderRatio(), getRenderRatio(), TRUE);
//CUT      InvalidateRgn(getOsWindowHandle(), clone->getOSHandle(), FALSE);
      StdWnd::invalidateRegion(getOsWindowHandle(), clone->getOSHandle());
      r->disposeClone(clone);
    } else
//CUT      InvalidateRgn(getOsWindowHandle(), r->getOSHandle(), FALSE);
      StdWnd::invalidateRegion(getOsWindowHandle(), r->getOSHandle());
  }
}

void BaseWnd::validate() {
//CUT  if (hwnd != NULL) ValidateRect(getOsWindowHandle(), NULL);
  if (hwnd != INVALIDOSWINDOWHANDLE) StdWnd::validateRect(getOsWindowHandle());
}

void BaseWnd::validateRect(RECT *r) {
  if (hwnd != INVALIDOSWINDOWHANDLE) {
    if (renderRatioActive()) {
      RECT r2 = *r;
      r2.left = (int)((double)r2.left * getRenderRatio());
      r2.top = (int)((double)r2.top * getRenderRatio());
      r2.right = (int)((double)r2.right * getRenderRatio());
      r2.bottom = (int)((double)r2.bottom * getRenderRatio());
//CUT      ValidateRect(getOsWindowHandle(), &r2);
      StdWnd::validateRect(getOsWindowHandle(), &r2);
    } else
//CUT    ValidateRect(getOsWindowHandle(), r);
      StdWnd::validateRect(getOsWindowHandle(), r);
  }
}

void BaseWnd::validateRgn(Region *reg) {
  if (hwnd != INVALIDOSWINDOWHANDLE) {
    if (renderRatioActive()) {
      Region *clone = reg->clone();
      clone->scale(getRenderRatio(), getRenderRatio(), TRUE);
//CUT      ValidateRgn(getOsWindowHandle(), clone->getOSHandle());
      StdWnd::validateRegion(getOsWindowHandle(), clone->getOSHandle());
      reg->disposeClone(clone);
    } else
//CUT      ValidateRgn(getOsWindowHandle(), reg->getOSHandle());
      StdWnd::validateRegion(getOsWindowHandle(), reg->getOSHandle());
  }
}

void BaseWnd::repaint() {
/*	if (hasVirtualChildren())	{
	  Region *h = new Region();
	  int s = GetUpdateRgn(getOsWindowHandle(), h->getHRGN(), FALSE);
	  if (s != NULLREGION && s != ERROR) {
  		virtualDrawRgn(h);
	  }
	  delete h;
	}*/
  //CUTif (hwnd != NULL) UpdateWindow(getOsWindowHandle());
  if (hwnd != INVALIDOSWINDOWHANDLE) StdWnd::update(getOsWindowHandle());
}

void BaseWnd::getClientRect(RECT *rect) {
  if (hwnd == INVALIDOSWINDOWHANDLE) {
    MEMSET(rect, 0, sizeof(RECT));
    return;
  }
  StdWnd::getClientRect(getOsWindowHandle(), rect);
  rect->left += margin[0];
  rect->top += margin[1];
  rect->right -= margin[2];
  rect->bottom -= margin[3];
  if (renderRatioActive()) {
    rect->right = rect->left + rwidth;
    rect->bottom = rect->left + rheight;
  }
}

RECT BaseWnd::clientRect() {
  RECT ret;
  getClientRect(&ret);
  return ret;
}

void BaseWnd::getNonClientRect(RECT *rect) {
  if (hwnd == INVALIDOSWINDOWHANDLE) {
    MEMSET(rect, 0, sizeof(RECT));
    return;
  } 

  StdWnd::getWindowRect(getOsWindowHandle(), rect);
  rect->right -= rect->left; rect->left = 0;
  rect->bottom -= rect->top; rect->top = 0;
  if (getRenderRatio() != 1.0) {
    rect->right = rect->left + rwidth;
    rect->bottom = rect->left + rheight;
  }
}

RECT BaseWnd::nonClientRect() {
  RECT ret;
  getNonClientRect(&ret);
  return ret;
}

void BaseWnd::getWindowRect(RECT *rect) {
//CUT#ifdef WIN32
//CUT  ASSERT(hwnd != NULL);
//CUT  GetWindowRect(getOsWindowHandle(), rect);
//CUT#else
//CUT#error port me
//CUT#endif
  StdWnd::getWindowRect(getOsWindowHandle(), rect);
}

// get position relative to parent (same coordinate system for basewnd & virtualwnd)
void BaseWnd::getPosition(POINT *pt) {
  if (!isVirtual() && getParent()) {
    RECT wr;
    getWindowRect(&wr);
    RECT pcr;
    StdWnd::getClientRect(getParent()->gethWnd(), &pcr); // don't go thru rootwnd->getClientRect!
    getParent()->clientToScreen((int *)&pcr.left, (int *)&pcr.top);
    RECT pr;
    getParent()->getWindowRect(&pr);
    pt->x = wr.left - pr.left - (pcr.left-pr.left);
    pt->y = wr.top - pr.top - (pcr.top-pr.top);
    return;
  }
  pt->x = rx;
  pt->y = ry;
}

// get position relative to parent from bottom/right 
void BaseWnd::getBRPosition(POINT *pt) {
  if (!isVirtual() && getParent()) {
    RECT wr;
    getWindowRect(&wr);
    RECT pcr;
    StdWnd::getClientRect(getParent()->gethWnd(), &pcr); // don't go thru rootwnd->getClientRect!
    getParent()->clientToScreen((int *)&pcr.right, (int *)&pcr.bottom);
    RECT pr;
    getParent()->getWindowRect(&pr);
    pt->x = -(wr.right - pr.right - (pcr.right-pr.right));
    pt->y = -(wr.bottom - pr.bottom - (pcr.bottom-pr.bottom));
    return;
  }
  RECT r;
  getParent()->getWindowRect(&r);
  RECT mr;
  getWindowRect(&mr);
  pt->x = r.right - mr.right;
  pt->y = r.bottom - mr.bottom;

  RECT wr;
  getParent()->getWindowRect(&wr);
  RECT pcr;
  StdWnd::getClientRect(getParent()->gethWnd(), &pcr); // don't go thru rootwnd->getClientRect!
  getParent()->clientToScreen((int *)&pcr.right, (int *)&pcr.bottom);
  pt->x -= wr.right-pcr.right;
  pt->y -= wr.bottom-pcr.bottom;

  multRatio((int *)&pt->x, (int *)&pt->y);
}

void *BaseWnd::dependent_getInterface(const GUID *classguid) {
  HANDLEGETINTERFACE(RootWnd);
//CUT  HANDLEGETINTERFACE(RootWnd);
  return NULL;
}

RECT BaseWnd::windowRect() {
  RECT ret;
  getWindowRect(&ret);
  return ret;
}


void BaseWnd::clientToScreen(int *x, int *y) {
  int _x = x ? *x : 0;
  int _y = y ? *y : 0;
  if (renderRatioActive()) {
    _x = (int)((double)_x * getRenderRatio());
    _y = (int)((double)_y * getRenderRatio());
  }
  StdWnd::clientToScreen(getOsWindowHandle(), &_x, &_y);
  if (x) *x = _x;
  if (y) *y = _y;
}

void BaseWnd::clientToScreen(RECT *r) {
  clientToScreen((int*)&r->left, (int*)&r->top);
  clientToScreen((int*)&r->right, (int*)&r->bottom);
}

void BaseWnd::clientToScreen(POINT *p) {
  clientToScreen((int *)&p->x, (int *)&p->y);
}

void BaseWnd::screenToClient(int *x, int *y) {
//CUT  POINT p;
  int _x = x ? *x : 0;
  int _y = y ? *y : 0;
//CUT  ScreenToClient(getOsWindowHandle(), &p);
  StdWnd::screenToClient(getOsWindowHandle(), &_x, &_y);
  if (renderRatioActive()) {
    _x = (int)((double)_x / getRenderRatio());
    _y = (int)((double)_y / getRenderRatio());
  }
  if (x) *x = _x;
  if (y) *y = _y;
}

void BaseWnd::screenToClient(RECT *r) {
  screenToClient((int*)&r->left, (int*)&r->top);
  screenToClient((int*)&r->right, (int*)&r->bottom);
}

void BaseWnd::screenToClient(POINT *p) {
  screenToClient((int *)&p->x, (int *)&p->y);
}


void BaseWnd::setParent(RootWnd *newparent) {
  ASSERTPR(newparent != NULL, "quit being a weeny");
  //ASSERTPR(parentWnd == NULL || newparent == parentWnd, "can't reset parent");
  parentWnd = newparent;
  if (isInited()) {
    OSWINDOWHANDLE w1 = getOsWindowHandle();
    OSWINDOWHANDLE w2 = newparent->getOsWindowHandle();
    if (w1 != w2)
      StdWnd::setParent(w1, w2);
  }
  if (!newparent->isBuffered()) setVirtual(0);
  onSetParent(newparent);
}

//FUCKO
int BaseWnd::reparent(RootWnd *newparent) {
  if (!isVirtual()) {
    if (isInited()) {
      RootWnd *old = getParent();
      if (!old && newparent) {
        StdWnd::setParent(getOsWindowHandle(), newparent->getOsWindowHandle());
#  ifdef WIN32
        SetWindowLong(getOsWindowHandle() , GWL_STYLE, GetWindowLong(getOsWindowHandle(), GWL_STYLE) & ~WS_POPUP);
        SetWindowLong(getOsWindowHandle() , GWL_STYLE, GetWindowLong(getOsWindowHandle(), GWL_STYLE) | WS_CHILD);
#  endif
      } else if (old && !newparent) {
#  ifdef WIN32
        SetWindowLong(getOsWindowHandle() , GWL_STYLE, GetWindowLong(getOsWindowHandle(), GWL_STYLE) & ~WS_CHILD);
        SetWindowLong(getOsWindowHandle() , GWL_STYLE, GetWindowLong(getOsWindowHandle(), GWL_STYLE) | WS_POPUP);
#  endif
        StdWnd::setParent(getOsWindowHandle(), INVALIDOSWINDOWHANDLE);
      } else {
        StdWnd::setParent(getOsWindowHandle(), newparent ? newparent->getOsWindowHandle() : INVALIDOSWINDOWHANDLE);
      }
    }
  }

  parentWnd = newparent;
  onSetParent(newparent);
  return 1;
}

RootWnd *BaseWnd::getParent() {
  return parentWnd;
}

RootWnd *BaseWnd::getRootParent() {
  return this;
}

//PORTME
RootWnd *BaseWnd::getDesktopParent() {
  OSWINDOWHANDLE w = getOsWindowHandle();
  OSWINDOWHANDLE last = w;
  if (w == INVALIDOSWINDOWHANDLE) return NULL;
  OSWINDOWHANDLE p = w;
#if defined(WIN32)
  // NONPORTABLE
  char cn[256];
  while (p && !(GetWindowLong(p, GWL_STYLE) & WS_POPUP)) {
    GetClassName(p, cn, 255); cn[255]=0;
    if (STRCASEEQL(cn, BASEWNDCLASSNAME))
      last = p;
    p = StdWnd::getParent(p);
  }
  if (p) 
#elif defined(XWINDOW)
  OSWINDOWHANDLE root = XWindow::RootWin(w);
  char cn[256];
  while (p != root) {
    XPointer xp;
    if(!XFindContext(XWindow::getDisplay(), p, XWindow::getContext(), &xp)) {
      GetClassName(p, cn, 255); cn[255]=0;
      if (STRCASEEQL(cn, BASEWNDCLASSNAME))
        last = p;
    }
    p = StdWnd::getParent(p);
  }
  if (p != root) 
#else
#error port me!
#endif //platform
  {
    GetClassName(p, cn, 255); cn[255]=0;
    if (STRCASEEQL(cn, BASEWNDCLASSNAME))
      return (RootWnd*)GetWindowLong(p, GWL_USERDATA);
    else if (last != INVALIDOSWINDOWHANDLE)
      return (RootWnd*)GetWindowLong(last, GWL_USERDATA);
  }
  return NULL;
}

int BaseWnd::notifyParent(int msg, int param1, int param2) {
  RootWnd *notifywnd = getNotifyWindow();
  if (getParent() == NULL && notifywnd == NULL) return 0;
  if (notifywnd == NULL) notifywnd = getParent();
  ASSERT(notifywnd != NULL);
  return notifywnd->childNotify(this, msg, param1, param2);
}

int BaseWnd::passNotifyUp(RootWnd *child, int msg, int param1, int param2) {
  // Same code as above to decide for whom we should notify.
  RootWnd *notifywnd = getNotifyWindow();
  if (getParent() == NULL && notifywnd == NULL) return 0;
  if (notifywnd == NULL) notifywnd = getParent();
  ASSERT(notifywnd != NULL);
  // And here we just change the RootWnd pointer.
  return notifywnd->childNotify(child, msg, param1, param2);
}

void BaseWnd::broadcastNotify(RootWnd *wnd, int msg, int p1, int p2) {
  int i;

  for (i=0;i<getNumRootWndChildren();i++)
    enumRootWndChildren(i)->childNotify(wnd, msg, p1, p2);

  for (i=0;i<getNumRootWndChildren();i++)
    enumRootWndChildren(i)->broadcastNotify(wnd, msg, p1, p2);
}


void BaseWnd::setNotifyId(int id) {
  notifyid = id;
}

int BaseWnd::getNotifyId() {
  return notifyid;
}

DragInterface *BaseWnd::getDragInterface() {
  return this;
}

RootWnd *BaseWnd::rootWndFromPoint(POINT *pt, int throughtest) {
  POINT pos = *pt;
  screenToClient(&pos);
  RootWnd *ret = findRootWndChild(pos.x, pos.y, 0, throughtest);
  if (ret == NULL) ret = this;
  return ret;
}

int BaseWnd::getSkinId() {
  return skin_id;
}

int BaseWnd::onMetricChange(int metricid, int param1, int param2) {
  switch (metricid) {
    case Metric::TEXTDELTA:
      return setFontSize(-1);
  }
  return 0;
}

int BaseWnd::rootwnd_onPaint(CanvasBase *canvas, Region *r) {
  BaseCloneCanvas c(canvas);
  return onPaint(&c, r);
}

int BaseWnd::rootwnd_paintTree(CanvasBase *canvas, Region *r) {
  BaseCloneCanvas c(canvas);
  return paintTree(&c, r);
}

const char *BaseWnd::getRootWndName() {
  return getName();
}

const char *BaseWnd::getId() {
  return NULL;
}

void BaseWnd::setSkinId(int id) {
  skin_id = id;
}

void BaseWnd::setPreferences(int what, int v) {
  switch (what) {
    case MAXIMUM_W: maximum_w = v; break;
    case MAXIMUM_H: maximum_h = v; break;
    case MINIMUM_W: minimum_w = v; break;
    case MINIMUM_H: minimum_h = v; break;
    case SUGGESTED_W: suggested_w = v; break;
    case SUGGESTED_H: suggested_h = v; break;
  }
}

int BaseWnd::getPreferences(int what) {
  if (getNumMinMaxEnforcers() > 0) {

    int min_x=minimum_w, min_y=minimum_h, max_x=maximum_w, max_y=maximum_h, sug_x=suggested_w, sug_y=suggested_h;
  
    for (int i=0;i<getNumMinMaxEnforcers();i++) {
  
      int tmin_x, tmin_y, tmax_x, tmax_y, tsug_x, tsug_y;
  
      RootWnd *w = enumMinMaxEnforcer(i);
  
      if (w) {

        tmin_x = w->getPreferences(MINIMUM_W);
        tmin_y = w->getPreferences(MINIMUM_H);
        tmax_x = w->getPreferences(MAXIMUM_W);
        tmax_y = w->getPreferences(MAXIMUM_H);
        tsug_x = w->getPreferences(SUGGESTED_W);
        tsug_y = w->getPreferences(SUGGESTED_H);

        if (tmin_x == -1) tmin_x = AUTOWH;
        if (tmin_y == -1) tmin_y = AUTOWH;
        if (tmax_x == -1) tmax_x = AUTOWH;
        if (tmax_y == -1) tmax_y = AUTOWH;
        if (tsug_x == -1) tsug_x = AUTOWH;
        if (tsug_y == -1) tsug_y = AUTOWH;

#if 0//FUCKO feex me
#ifndef DISABLE_SYSFONTSCALE
        GuiObject *o = static_cast<GuiObject *>(getInterface(guiObjectGuid));
        if (o != NULL) {
          if (o->guiobject_getAutoSysMetricsW()) {
            if (tmin_x != AUTOWH) tmin_x = (int)((float)tmin_x*Canvas::getSystemFontScale());
            if (tmax_x != AUTOWH) tmax_x = (int)((float)tmax_x*Canvas::getSystemFontScale());
            if (tsug_x != AUTOWH) tsug_x = (int)((float)tsug_x*Canvas::getSystemFontScale());
          }
          if (o->guiobject_getAutoSysMetricsH()) {
            if (tmin_y != AUTOWH) tmin_y = (int)((float)tmin_y*Canvas::getSystemFontScale());
            if (tmax_y != AUTOWH) tmax_y = (int)((float)tmax_y*Canvas::getSystemFontScale());
            if (tsug_y != AUTOWH) tsug_y = (int)((float)tsug_y*Canvas::getSystemFontScale());
          }
        }
#endif
#endif

        RECT cor;
        w->getNonClientRect(&cor);
        RECT wr;
        getNonClientRect(&wr);

        int xdif = (wr.right-wr.left)-(cor.right-cor.left);
        int ydif = (wr.bottom-wr.top)-(cor.bottom-cor.top);
        if (tmin_x != AUTOWH) tmin_x += xdif;
        if (tmin_y != AUTOWH) tmin_y += ydif;
        if (tmax_x != AUTOWH) tmax_x += xdif;
        if (tmax_y != AUTOWH) tmax_y += ydif;
        if (tsug_x != AUTOWH) tsug_x += xdif;
        if (tsug_y != AUTOWH) tsug_y += ydif;
      }

      if (min_x != AUTOWH) min_x = (tmin_x != AUTOWH) ? MAX(min_x, tmin_x) : min_x; else min_x = tmin_x;
      if (max_x != AUTOWH) max_x = (tmax_x != AUTOWH) ? MAX(max_x, tmax_x) : max_x; else max_x = tmax_x;
      if (min_y != AUTOWH) min_y = (tmin_y != AUTOWH) ? MAX(min_y, tmin_y) : min_y; else min_y = tmin_y;
      if (max_y != AUTOWH) max_y = (tmax_y != AUTOWH) ? MAX(max_y, tmax_y) : max_y; else max_y = tmax_y;
      if (sug_x != AUTOWH) sug_x = (tsug_x != AUTOWH) ? MAX(sug_x, tsug_x) : sug_x; else sug_x = tsug_x;
      if (sug_y != AUTOWH) sug_y = (tsug_y != AUTOWH) ? MAX(sug_y, tsug_y) : sug_y; else sug_y = tsug_y;
    }

    if (min_x != AUTOWH && min_x == max_x) sug_x = min_x;
    if (min_y != AUTOWH && min_y == max_y) sug_y = min_y;

    switch (what) {
      case MINIMUM_W: return min_x;
      case MINIMUM_H: return min_y;
      case MAXIMUM_W: return max_x;
      case MAXIMUM_H: return max_y;
      case SUGGESTED_W: return sug_x;
      case SUGGESTED_H: return sug_y;
    }
  }

  switch (what) {
    case SUGGESTED_W: return suggested_w;
    case SUGGESTED_H: return suggested_h;
    case MAXIMUM_W: return maximum_w;
    case MAXIMUM_H: return maximum_h;
    case MINIMUM_W: return minimum_w;
    case MINIMUM_H: return minimum_h;
  }

  return AUTOWH;
}

void BaseWnd::setStartHidden(int wtf) {
  start_hidden = wtf;
}

void BaseWnd::handleDragOver(int xPos, int yPos) {
  POINT pt={xPos, yPos};
  clientToScreen(&pt);
  RootWnd *targ;
  int candrop = 0;
  // find the window the mouse is over

  targ = NULL;
  if (stickyWnd) {
    RECT wr;
    StdWnd::getWindowRect(stickyWnd->getOsWindowHandle(), &wr);
    if (pt.x >= wr.left-sticky.left &&
        pt.x <= wr.right + sticky.right &&
        pt.y >= wr.top - sticky.top &&
        pt.y <= wr.bottom + sticky.bottom) targ = stickyWnd;
    else stickyWnd = NULL;
  }

#ifdef WASABI_COMPILE_WND
  if (targ == NULL && WASABI_API_WND) targ = WASABI_API_WND->rootWndFromPoint(&pt);
#else
  targ = rootWndFromScreenPoint(&pt, THROUGHTEST_NODNDTHROUGH);
#endif

  DI prevtargdi(prevtarg);
  DI targdi(targ);

  if (prevtarg != targ) { // window switch
    if (prevtarg != NULL) prevtargdi.dragLeave(this);
    if (targ != NULL) 
      targdi.dragEnter(this);
  }
  if (targ != NULL)
    candrop = targdi.dragOver(pt.x, pt.y, this);
#ifdef WIN32
  if (targ == NULL || !candrop)
    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_NO)));
  else
    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_APPSTARTING)));
#elif defined(XWINDOW)
  if ( prevtarg != NULL && prevtarg != targ )
    if ( targ == NULL || prevtarg->gethWnd() != targ->gethWnd() )
      XWindow::setCursor( prevtarg->gethWnd(), None );

  if ( targ != NULL )
    XWindow::setCursor( targ->gethWnd(), candrop?XC_hand2:XC_X_cursor );
#else
#error port me!
#endif // platform
  prevcandrop = candrop;
  prevtarg = targ;
}

void BaseWnd::handleDragDrop(int xPos, int yPos) {
  clientToScreen(&xPos, &yPos);
  // reset dragging immediately, so we avoid reentry if the dragDrop causes a cancelCapture
  dragging = 0;
  int res = 0;
  if (prevtarg != NULL && prevcandrop) {
    res = DI(prevtarg).dragDrop(this, xPos, yPos);
  }

  // inform source what happened
  dragComplete(res);

  resetDragSet();
  prevtarg = NULL;
  prevcandrop = 0;
  stickyWnd = NULL;
  suggestedTitle = NULL;
#ifdef WIN32
  SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
  endCapture();
#elif defined(XWINDOW)
  XWindow::setCursor( gethWnd(), None );
#else
#error port me!
#endif
}

//PORTME
LRESULT BaseWnd::wndProc(OSWINDOWHANDLE hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  int xPos, yPos;

  switch (uMsg) {
    case WM_CLOSE: return 0;
    case WM_NCPAINT: return 0;
    case WM_SYNCPAINT: return 0;
  }
  if (!isDestroying()) switch (uMsg) {
#ifdef WIN32
    case WM_SYSCOMMAND: {
      if ((wParam & 0xfff0) == SC_SCREENSAVE || (wParam & 0xfff0) == SC_MONITORPOWER) {
#ifdef WASABI_COMPILE_WND
        RootWnd *main = WASABI_API_WND->main_getRootWnd();
        if (main && main != this)
          return SendMessage(main->gethWnd(),uMsg,wParam,lParam);
#endif
      }
      switch (wParam & 0xFFF0) {
        case SC_MAXIMIZE:
          if (!allowMaximize()) return 0;
          break;
        case SC_MINIMIZE:
          if (!allowMinimize()) return 0;
          break;
        case SC_RESTORE:
          if (!allowRestore()) return 0;
          break;
      }
      int r = callDefProc(hWnd, uMsg, wParam, lParam);
      switch (wParam & 0xFFF0) {
        case SC_MAXIMIZE:
          onMaximize();
          break;
        case SC_MINIMIZE:
          onMinimize();
          break;
        case SC_RESTORE:
          onRestore();
          break;
      }
      return r;
    }
#endif // WIN32
//CUT    case WM_CREATE:
//CUT      hwnd = hWnd;
//CUT    break;

    case WM_PAINT: {
#ifdef WIN32
      if (inonresize && !wantRedrawOnResize()) return 1;
#endif
      ASSERT(hwnd != INVALIDOSWINDOWHANDLE);
#ifdef WIN32
      if (!isVisible(1) || IsIconic(gethWnd())) break;
#else
      if (!isVisible(1)) break;
#endif
      RECT r;
      if (StdWnd::getUpdateRect(hWnd, &r)) {
        if (virtualOnPaint()) { return 0; }
      }
    }
    break;

    case WM_SETCURSOR:
      if (checkModal()) return TRUE;
      if (hWnd == (OSWINDOWHANDLE)wParam) {
        updateCursor();
      }
      return TRUE;

    case WM_TIMER:
      timerCallback(wParam);
      return 0;

#ifdef WASABI_API_ACCESSIBILITY
    case WM_GETOBJECT:
      if (lParam == OBJID_CLIENT) {
        Accessible *acc = getAccessibleObject();
        if (acc != NULL) {
          LRESULT lAcc = acc->getOSHandle(wParam);
          return lAcc;
        }
      }
      break;  // Fall through to DefWindowProc
#endif


    case WM_SETFOCUS:
#if defined(XWINDOW)
      if (isInited()) onGetFocus();
#elif defined(WIN32)
      if (!focusEventsEnabled) break;
      if (isInited()) {
        if (rootfocus != NULL && rootfocus != this) {
          if (rootfocus != curVirtualChildFocus)
            rootfocus->setFocus();
          break;
        } else {
          if (wantFocus()) {
            onGetFocus();
            break;
          } else {
            RootWnd *w = getTab(TAB_GETFIRST);
            if (w != NULL) {
              w->setFocus();
            }
          }
        }
      }
#endif // platform
    break;

    case WM_KILLFOCUS: {
      RootWnd *rp = getRootParent();
#ifdef WASABI_COMPILE_WND
      if (!WASABI_API_WND->rootwndIsValid(rp) || !StdWnd::isValidWnd(rp->getOsWindowHandle())) break;
#endif
      if (!focusEventsEnabled) break;
#ifdef WASABI_COMPILE_WND
      if (WASABI_API_WND) WASABI_API_WND->forwardOnKillFocus(); // resets the keyboard active keys buffer
      if (!WASABI_API_WND->rootwndIsValid(curVirtualChildFocus)) curVirtualChildFocus = NULL;
#endif
      if (curVirtualChildFocus) {
  curVirtualChildFocus->onKillFocus();
  curVirtualChildFocus = NULL;
      } else {
        if (hasfocus) onKillFocus();
      }
    }
    break;

    // dragging and dropping

    case WM_LBUTTONDOWN: {
      if (lParam == 0xdeadc0de) return 1;
      if (bufferizeLockedUIMsg(uMsg, wParam, lParam)) return 0;
#ifdef WASABI_COMPILE_WND
      WASABI_API_WND->popupexit_check(this);
#endif
      if (checkModal()) return 0;
      abortTip();
      xPos = (signed short)LOWORD(lParam);
      yPos = (signed short)HIWORD(lParam);
      xPos = (int)((float)xPos / getRenderRatio());
      yPos = (int)((float)yPos / getRenderRatio());
      if (!getCapture() && hasVirtualChildren())
        if (handleVirtualChildMsg(uMsg,xPos,yPos)) 
          return 0;
      if (isEnabled() && !dragging) {
        autoFocus(this);
        int r = 0;
        if (wantLeftClicks())
          r = onLeftButtonDown(xPos, yPos);
        if (checkDoubleClick(uMsg, xPos, yPos) && wantDoubleClicks())
          if (onLeftButtonDblClk(xPos, yPos))
            return 0;
        return r;
      }
    }
    break;

    case WM_MBUTTONDOWN: {
      if (lParam == 0xdeadc0de) return 1;
      if (bufferizeLockedUIMsg(uMsg, wParam, lParam)) return 0;
#ifdef WASABI_COMPILE_WND 
      WASABI_API_WND->popupexit_check(this);
#endif
      if (checkModal()) return 0;
      abortTip();
      xPos = (signed short)LOWORD(lParam);
      yPos = (signed short)HIWORD(lParam);
      xPos = (int)((float)xPos / getRenderRatio());
      yPos = (int)((float)yPos / getRenderRatio());
      if (!getCapture() && hasVirtualChildren())
        if (handleVirtualChildMsg(uMsg,xPos,yPos)) 
          return 0;
      if (isEnabled() && !dragging) {
        autoFocus(this);
        int r = 0;
        if (wantMiddleClicks()) 
          r = onMiddleButtonDown(xPos, yPos);
        if (checkDoubleClick(uMsg, xPos, yPos) && wantDoubleClicks())
          if (onMiddleButtonDblClk(xPos, yPos))
            return 0;
        return r;
      }
    }
    break;

    case WM_RBUTTONDOWN: {
      if (lParam == 0xdeadc0de) return 1;
      if (bufferizeLockedUIMsg(uMsg, wParam, lParam)) return 0;
#ifdef WASABI_COMPILE_WND
      WASABI_API_WND->popupexit_check(this);
#endif
      if (checkModal()) return 0;
      abortTip();
      xPos = (signed short)LOWORD(lParam);
      yPos = (signed short)HIWORD(lParam);
      xPos = (int)((float)xPos / getRenderRatio());
      yPos = (int)((float)yPos / getRenderRatio());
      if (!getCapture() && hasVirtualChildren())
        if (handleVirtualChildMsg(uMsg,xPos,yPos)) 
          return 0;
      if (isEnabled() && !dragging) {
        autoFocus(this);
        int r = 0;
        if (wantRightClicks())
          r = onRightButtonDown(xPos, yPos);
        if (checkDoubleClick(uMsg, xPos, yPos) && wantDoubleClicks()) if (onRightButtonDblClk(xPos, yPos)) return 0;
        return r;
      }
    }
    break;

    case WM_MOUSEMOVE: {
      if (checkModal()) return 0;
      xPos = (signed short)LOWORD(lParam);
      yPos = (signed short)HIWORD(lParam);
      xPos = (int)((float)xPos / getRenderRatio());
      yPos = (int)((float)yPos / getRenderRatio());
      if (dragging) {
        handleDragOver(xPos, yPos);
      } else if (isEnabled()) {
        tipbeenchecked=FALSE;
        if (!getCapture() && hasVirtualChildren()) {
          if (handleVirtualChildMsg(WM_MOUSEMOVE, xPos, yPos))
            return 0;
        }
        if (getCapture()) {
          int r = 0;
          if (wantMouseMoves())
            if (onMouseMove(xPos, yPos))
              return 0;
        }
        if (!tipbeenchecked) onTipMouseMove();
        return 0;
      }
    }
    break;

    case WM_LBUTTONUP: {
      if (lParam == 0xdeadc0de) return 1;
      if (bufferizeLockedUIMsg(uMsg, wParam, lParam)) return 0;
      if (checkModal()) return 0;
      xPos = (signed short)LOWORD(lParam);
      yPos = (signed short)HIWORD(lParam);
      xPos = (int)((float)xPos / getRenderRatio());
      yPos = (int)((float)yPos / getRenderRatio());
      abortTip();
      if (!dragging && !getCapture() && hasVirtualChildren()) {
        if (handleVirtualChildMsg(uMsg,xPos,yPos)) 
          return 0;
      }
      if (dragging) {
        handleDragDrop(xPos, yPos);
      } else if (isEnabled()) {
        if (wantLeftClicks())
          if (onLeftButtonUp(xPos, yPos)) return 0;
      }
    }
    break;

    case WM_MBUTTONUP: {
      if (lParam == 0xdeadc0de) return 1;
      if (bufferizeLockedUIMsg(uMsg, wParam, lParam)) return 0;
      if (checkModal()) return 0;
      int xPos = (signed short)LOWORD(lParam);
      int yPos = (signed short)HIWORD(lParam);        
      xPos = (int)((float)xPos / getRenderRatio());
      yPos = (int)((float)yPos / getRenderRatio());
      abortTip();
      if (!dragging && !getCapture() && hasVirtualChildren()) {
        if (handleVirtualChildMsg(uMsg,xPos,yPos)) 
          return 0;
      }
      if (dragging) {
        handleDragDrop(xPos, yPos);
      } else if (isEnabled()) {
        if (wantMiddleClicks())
          if (onMiddleButtonUp(xPos, yPos)) return 0;
      }
    }
    break;

    case WM_RBUTTONUP: {
      if (lParam == 0xdeadc0de) return 1;
      if (bufferizeLockedUIMsg(uMsg, wParam, lParam)) return 0;
      if (checkModal()) return 0;
      abortTip();
      xPos = (signed short)LOWORD(lParam);
      yPos = (signed short)HIWORD(lParam);
      xPos = (int)((float)xPos / getRenderRatio());
      yPos = (int)((float)yPos / getRenderRatio());
      if (!getCapture() && hasVirtualChildren()) {
        if (handleVirtualChildMsg(uMsg,xPos,yPos))
          return 0;
      }
      if (isEnabled() && !dragging) {
        if (wantRightClicks())
          if (onRightButtonUp(xPos, yPos)) return 0;
      }
    }
    break;

    case WM_CONTEXTMENU: {
      if (bufferizeLockedUIMsg(uMsg, wParam, lParam)) return 0;
      if (checkModal()) return 0;
      ASSERT(hWnd != INVALIDOSWINDOWHANDLE);
      int xPos = (signed short)LOWORD(lParam);
      int yPos = (signed short)HIWORD(lParam);
      if (hWnd == getOsWindowHandle()) {
        if (wantContextMenus())
          if (onContextMenu(xPos, yPos)) return 0;
#ifdef WIN32
      } else if (GetParent(hWnd) == getOsWindowHandle()) {
  if (wantContextMenus())
    if (onChildContextMenu(xPos, yPos)) return 0;
#endif
      }
    }
    break;

    case WM_ERASEBKGND:
      return (onEraseBkgnd((HDC)wParam));

      case WM_MOUSEWHEEL: {
        abortTip();

//jeez could this be less commented
        int l=(short)HIWORD(wParam)/120;
        int a=(short)HIWORD(wParam);
        int t=LOWORD(wParam);;
        if (!l)
          if (a > 0) l=1;
        else if (a < 0)l=0;
        a=l>=0?l:-l;
#ifdef WIN32
      if (GetAsyncKeyState(VK_MBUTTON)&0x8000) {
  if (l>=0) l=0; // Fast Forward 5s
  else l=1; // Rewind 5s
      } else {
#endif
        if (l>=0) l=2; // Volume up
        else l=3; // Volume down
#ifdef WIN32
      }
#endif

      int r = 0;

      if (l & 1)
        r = onMouseWheelDown(!(BOOL)(l&2), a);
      else
        r = onMouseWheelUp(!(BOOL)(l&2), a);
#ifdef WASABI_COMPILE_WND
      if (r == 0) {
  r = WASABI_API_WND->forwardOnMouseWheel(l, a);
      }
#endif
      // if it wasn't handled by this wnd, nor by the api, send it to the main wnd, unless we're it

#ifdef WASABI_COMPILE_WND
      if (r == 0) {
  if (WASABI_API_WND->main_getRootWnd() != this)
    r = SendMessage(WASABI_API_WND->main_getRootWnd()->gethWnd(), uMsg, wParam, lParam);
      }
#endif


      return r;
      }

      case WM_WA_RELOAD: {
        if (wParam == 0)
          freeResources();
        else
          reloadResources();
        return 0;
      }

      case WM_WA_GETFBSIZE: {
        SIZE *s = (SIZE *)wParam;
        s->cx = rwidth;
        s->cy = rheight;
        return 0;
      }

      case WM_USER+8976: // wheel in tip, delete tip
        abortTip();
        return 0;

    case WM_CHAR:
      if (bufferizeLockedUIMsg(uMsg, wParam, lParam)) return 0;
#ifdef WASABI_COMPILE_WND
      if (WASABI_API_WND->interceptOnChar((TCHAR) wParam)) return 0;
#endif
      if (curVirtualChildFocus == NULL) {
  if (onChar(((TCHAR) wParam))) return 0;
      } else {
        if (curVirtualChildFocus->onChar(((TCHAR) wParam))) return 0;
      }
#ifdef WASABI_COMPILE_WND
      if (WASABI_API_WND && WASABI_API_WND->forwardOnChar(this, (TCHAR) wParam, lParam)) return 0;
#endif
      break;

    case WM_KEYDOWN:
      if (bufferizeLockedUIMsg(uMsg, wParam, lParam)) return 0;
#ifdef WASABI_COMPILE_WND
      if (WASABI_API_WND->interceptOnKeyDown((int) wParam)) return 0;
#endif
      if (curVirtualChildFocus == NULL) {
  if (onKeyDown((int) wParam)) return 0;
      } else {
        if (curVirtualChildFocus->onKeyDown((int)wParam)) return 0;
      }
#ifdef WASABI_COMPILE_WND
      if (WASABI_API_WND && WASABI_API_WND->forwardOnKeyDown(this, (int) wParam, lParam)) return 0;
#endif
      break;

    case WM_KEYUP:
      if (bufferizeLockedUIMsg(uMsg, wParam, lParam)) return 0;
#ifdef WASABI_COMPILE_WND
      if (WASABI_API_WND->interceptOnKeyUp((int) wParam)) return 0;
#endif
      if (curVirtualChildFocus == NULL) {
  if (onKeyUp((int) wParam)) return 0;
      } else {
        if (curVirtualChildFocus->onKeyUp((int)wParam)) return 0;
      }
#ifdef WASABI_COMPILE_WND
      if (WASABI_API_WND && WASABI_API_WND->forwardOnKeyUp(this, (int) wParam, lParam)) return 0;
#endif
      break;

    case WM_SYSKEYDOWN:
      if (bufferizeLockedUIMsg(uMsg, wParam, lParam)) return 0;
#ifdef WASABI_COMPILE_WND
      if (WASABI_API_WND->interceptOnSysKeyDown((int) wParam, lParam)) return 0;
#endif
      if (curVirtualChildFocus == NULL) {
  if (onSysKeyDown((int) wParam, lParam)) return 0;
      } else {
        if (curVirtualChildFocus->onSysKeyDown((int)wParam, lParam)) return 0;
      }
#ifdef WASABI_COMPILE_WND
      if (WASABI_API_WND && WASABI_API_WND->forwardOnSysKeyDown(this, (int) wParam, lParam)) return 0;
#endif
      break;

    case WM_SYSKEYUP:
      if (bufferizeLockedUIMsg(uMsg, wParam, lParam)) return 0;
#ifdef WASABI_COMPILE_WND
      if (WASABI_API_WND->interceptOnSysKeyUp((int) wParam, lParam)) return 0;
#endif
      if (curVirtualChildFocus == NULL) {
  if (onSysKeyUp((int) wParam, lParam)) return 0;
      } else {
        if (curVirtualChildFocus->onSysKeyUp((int)wParam, lParam)) return 0;
      }
#ifdef WASABI_COMPILE_WND
      if (WASABI_API_WND && WASABI_API_WND->forwardOnSysKeyUp(this, (int) wParam, lParam)) return 0;
#endif
      break;

    case WM_MOUSEACTIVATE: {
#ifdef WIN32
      if (checkModal() || !wantActivation())
        return MA_NOACTIVATE;
      //SetFocus(getOsWindowHandle());
      return MA_ACTIVATE;
#else
      return 0;
#endif
    }

    case WM_ACTIVATEAPP:
      if (wParam == FALSE) {
#ifdef WASABI_COMPILE_WND
      if (WASABI_API_WND != NULL) {
        WASABI_API_WND->popupexit_signal();
        if (!wParam) {
          WASABI_API_SYSCB->syscb_issueCallback(SysCallback::GC, GarbageCollectCallback::GARBAGECOLLECT);
        }
        WASABI_API_WND->kbdReset();
        return 0;
      }
#endif
  }

  case WM_ACTIVATE: {
    int fActive = LOWORD(wParam);
    if (fActive == WA_ACTIVE || fActive == WA_CLICKACTIVE)  {
#ifdef WASABI_COMPILE_WND
        if (WASABI_API_WND != NULL)
          WASABI_API_WND->popupexit_check(this);
#endif
        onActivate();
    } else {
      onDeactivate();
    }
  }
  return 0;

  case WM_NCACTIVATE:
    if (allowDeactivation())
      return TRUE;
    return FALSE;

    case WM_WINDOWPOSCHANGED: {

#ifdef WIN32
      WINDOWPOS *lpwp = (WINDOWPOS *)lParam; // points to size and position data

      if (!inonresize) {
        int w = rwidth;
        int h = rheight;
        multRatio(&w, &h);
        if (lpwp->cx != w || lpwp->cy != h) {
                //DebugString("external onResize\n");
          w = lpwp->cx;
          h = lpwp->cy;
          divRatio(&w, &h);
          setRSize(rx, ry, w, h);
          if (isPostOnInit())
            onResize();
        }
      }
#endif

      onPostedMove();
      return 0;
    }

    case WM_DROPFILES: {
      if (checkModal()) break;
#ifdef WASABI_COMPILE_WND
      WASABI_API_WND->pushModalWnd();
#endif
      onExternalDropBegin();
#ifdef WIN32
      HDROP h = (HDROP)wParam;
      POINT dp;
      DragQueryPoint(h, &dp);
      clientToScreen(&dp);
      // build a file list
      char buf[WA_MAX_PATH];
      PtrList<FilenameI> keep;

      SetCursor(LoadCursor(NULL, IDC_WAIT));

//CUT #if UTF8
//CUT       // doesn't really need UTF8, the "buf" is never written to.
//CUT       // made to be NULL to enforce this concept.
      int nfiles = DragQueryFile(h, 0xffffffff, NULL, 0);
//CUT #else
//CUT       int nfiles = DragQueryFile(h, 0xffffffff, buf, sizeof(buf));
//CUT #endif

      // convert them all to PlayItem *'s
      String buf8;
      for (int i = 0; i < nfiles; i++) {
#if UTF8
        if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
          // CODE FOR NT COMPATIBLE OS'S
          wchar_t bufW[WA_MAX_PATH];
          // bufW contains the next file we want to process.
          DragQueryFileW(h, i, bufW, sizeof(bufW));
          // And convert so buf8 is carrying the package.
          EncodedStr fname16enc(SvcStrCnv::UTF16, bufW, (WSTRLEN(bufW)+1)*2, 0/*no delete*/);
          int retcode = fname16enc.convertToUTF8(buf8);
          if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
            DragQueryFile(h, i, buf, sizeof(buf));
            buf8 = buf;
          }
        } else {
          // CODE FOR 9x COMPATIBLE OS'S
          DragQueryFile(h, i, buf, sizeof(buf));
          // And convert so buf8 is carrying the package.
          EncodedStr fnameOSenc(SvcStrCnv::OSNATIVE, buf, (STRLEN(buf)+1), 0/*no delete*/);
          int retcode = fnameOSenc.convertToUTF8(buf8);
          if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
            DragQueryFile(h, i, buf, sizeof(buf));
            buf8 = buf;
          }
        }
        addDroppedFile(buf8, &keep);    // recursive
#else
        DragQueryFile(h, i, buf, sizeof(buf));
        addDroppedFile(buf, &keep);     // recursive
#endif
      }
      SetCursor(LoadCursor(NULL, IDC_ARROW));

      DI dragger( this );
#elif defined(XWINDOW)
      POINT dp = { wParam >> 16, wParam & 0xffff };
      PtrList<FilenameI> keep;
      char *data = (char *)lParam;

      PathParser pp( data, "\n" );
      int nfiles = pp.getNumStrings();

      for ( int i = 0; i < nfiles; i++ ) {
        addDroppedFile( pp.enumString( i ), &keep );
      }

      XFree( data );

      // Stupid Xdnd doesn't do child windows
#ifdef WASABI_COMPILE_WND
      RootWnd *targ = WASABI_API_WND->rootWndFromPoint(&dp);
#else
      RootWnd *targ = NULL;
      {
        OSWINDOWHANDLE targw = StdWnd::windowFromPoint(dp);
        if(targw != INVALIDOSWINDOWHANDLE) {
          XPointer xp;
          if(!XFindContext(XWindow::getDisplay(), targw, XWindow::getContext(), &xp))
            targ = ((RootWnd *)GetWindowLong(targw, GWL_USERDATA))->rootWndFromPoint(&dp);
        }
        if(targ == NULL) targ = static_cast<RootWnd *>(this);
      }
#endif

      DI dragger( targ );
#else
#error port me!
#endif
      dragging = 1;
      if (dragEnter(this)) {
        if (dragOver(dp.x, dp.y, this)) dragDrop(this, dp.x, dp.y);
      } else {
        dragLeave(this);
#ifdef FORWARD_DRAGNDROP
#ifdef WASABI_COMPILE_WND
        OSWINDOWHANDLE w = WASABI_API_WND->main_getRootWnd()->gethWnd();
        SendMessage(w, WM_DROPFILES, wParam, lParam);
#endif
#endif
      }
      dragging = 0;

      // remove data
      keep.deleteAll();
      resetDragSet();

      onExternalDropEnd();
#ifdef WASABI_COMPILE_WND
      WASABI_API_WND->popModalWnd();
#endif
    }
    return 0;   // dropfiles

  case WM_CAPTURECHANGED:
    if (preventcancelcapture) break;
    inputCaptured = 0;
    if (curVirtualChildCaptured != NULL) {
      RootWnd *w = curVirtualChildCaptured;
      curVirtualChildCaptured = NULL;
      w->onCancelCapture();
    } else {
      onCancelCapture();
    }
    return 0;

}//switch

  if (uMsg >= WM_USER) {
    int ret;
    if (onUserMessage(uMsg, wParam, lParam, &ret))
      return ret;
  }

  return callDefProc(hWnd, uMsg, wParam, lParam);
}

int BaseWnd::onUserMessage(int msg, int w, int l, int *r) { return 0; }

int BaseWnd::checkDoubleClick(int b, int x, int y) {
  DWORD now = Std::getTickCount();

//BU> why is this switch there if we don't keep track of which button?
  switch (b) {
    case WM_LBUTTONDOWN:
      if (lastClick[0] > now - StdWnd::getDoubleClickDelay()) {
        lastClick[0] = 0;
        if (ABS(lastClickP[0].x - x) > StdWnd::getDoubleClickX() ||
            ABS(lastClickP[0].y - y) > StdWnd::getDoubleClickY())
          return 0;
        return 1;
      }
      lastClick[0] = now;
      lastClickP[0].x = x;
      lastClickP[0].y = y;
      break;

    case WM_MBUTTONDOWN:
      if (lastClick[0] > now - StdWnd::getDoubleClickDelay()) {
        lastClick[0] = 0;
        if (ABS(lastClickP[0].x - x) > StdWnd::getDoubleClickX() ||
            ABS(lastClickP[0].y - y) > StdWnd::getDoubleClickY())
          return 0;
        return 1;
      }
      lastClick[0] = now;
      lastClickP[0].x = x;
      lastClickP[0].y = y;
      break;

    case WM_RBUTTONDOWN:
      if (lastClick[1] > now - StdWnd::getDoubleClickDelay()) {
        lastClick[1] = 0;
        if (ABS(lastClickP[1].x - x) > StdWnd::getDoubleClickX() || ABS(lastClickP[1].y - y) > StdWnd::getDoubleClickY()) return 0;
        return 1;
      }
      lastClick[1] = now;
      lastClickP[1].x = x;
      lastClickP[1].y = y;
      break;
    }

  return 0;
}

int BaseWnd::onMouseWheelUp(int click, int lines) {
  return 0;
}

int BaseWnd::onMouseWheelDown(int click, int lines) {
  return 0;
}

int BaseWnd::onContextMenu(int x, int y) {
  return 0;
}

int BaseWnd::onChildContextMenu(int x, int y) {
  return 0;
}

int BaseWnd::onDeferredCallback(int param1, int param2) {
  switch (param1) {
    case DEFERREDCB_FLUSHPAINT:
      do_flushPaint();
      break;
    case DEFERREDCB_INVALIDATE:
      if (isPostOnInit())
        invalidate();
      break;
    case DEFERREDCB_FOCUSFIRST:
      assignRootFocus(NULL);
      if (StdWnd::getFocus() == getOsWindowHandle()) {
        focusNext();
      }
      break;
    case 0x7849/*DEFERREDCB_ONHIDE*/: {
      foreach(rootwndchildren)
        RootWnd *w = rootwndchildren.getfor();
        if (w->isVisible(1)) // check internal flag only
          w->onSetVisible(0);
      endfor;
      dependent_sendEvent(RootWnd::depend_getClassGuid(), Event_SETVISIBLE, 0);
      break;
    }
  }
  return 0;
}

int BaseWnd::onPaint(Canvas *canvas) {
#if 0
  // example:
  PaintCanvas c;
  if (!c.beginPaint(this)) return 0;
  (do some painting)
  c will self-destruct on return
#endif
  if (renderbasetexture) {
    PaintCanvas paintcanvas;
    if (canvas == NULL) {
      if (!paintcanvas.beginPaint(this)) return 0;
      canvas = &paintcanvas;
    }
    RECT r;
    getNonClientRect(&r);
    renderBaseTexture(canvas, r);
  }
  return 0;
}

int BaseWnd::onPaint(Canvas *canvas, Region *h) {

  if (!canvas) return onPaint(canvas);

#if defined(WIN32)
  int sdc = SaveDC(canvas->getHDC());	//PORTME
#elif defined(XWINDOW)
  RegionI oldrgn;
  canvas->getClipRgn( &oldrgn );
#else
#error port me!
#endif //platform

  canvas->selectClipRgn(h);

  int rs = onPaint(canvas);

#if defined(WIN32)
  RestoreDC(canvas->getHDC(), sdc);	//PORTME
#elif defined(XWINDOW)
  canvas->selectClipRgn( &oldrgn );
#else
#error port me!
#endif //platform

  return rs;
    
}

int BaseWnd::getTransparency() {
  return wndalpha;
}

void BaseWnd::setTransparency(int amount) {
  //if (wndalpha == amount) return;
  if (amount == 254) amount = 255;
  if (amount == 1) amount = 0;

  if (amount != -1) wndalpha = amount; else amount = wndalpha;

  if (!StdWnd::isDesktopAlphaAvailable()) {
    wndalpha = 255;
    return;
  } 

  if (w2k_alpha) {
    invalidate(); 
    return;
  }

#ifdef WIN32

  if (!isInited() || isVirtual()) return;
  if (!StdWnd::isValidWnd(getOsWindowHandle())) return;

  if (amount < -1) amount = 0;
  else if (amount > 255) amount = 255;

//CUT  DWORD dwLong = GetWindowLong(hwnd, GWL_EXSTYLE);
  if (amount==255 && !forceTransparencyFlag()) {
    StdWnd::setLayeredWnd(hwnd, FALSE);
//CUT    if (dwLong & WS_EX_LAYERED)
//CUT      SetWindowLong(hwnd, GWL_EXSTYLE, dwLong & ~WS_EX_LAYERED);
    has_alpha_flag = 0;
  } else {
     if (!StdWnd::isLayeredWnd(hwnd))
       StdWnd::setLayeredWnd(hwnd, TRUE);
//CUT    if (!(dwLong & WS_EX_LAYERED))
//CUT      SetWindowLong(hwnd, GWL_EXSTYLE, dwLong | WS_EX_LAYERED);
    StdWnd::setLayeredAlpha(hwnd, amount);
//CUT    setLayeredWindowAttributes(hwnd, RGB(0,0,0), amount, LWA_ALPHA);
    has_alpha_flag = 1;
  }
#endif
}

int BaseWnd::forceTransparencyFlag() { 
  return 0;
}

int BaseWnd::beginCapture() {
  if (!allowCapture) return 0;
  if (!getCapture()) {
    disable_tooltip_til_recapture = 0;
    curVirtualChildCaptured = NULL;
/*    oldCapture = */StdWnd::setCapture(getOsWindowHandle());
/*    if (oldCapture) {
      DebugString("Stolen capture detected, this may be ok, but try to avoid it if possible. Saving old capture\n");
    }*/
    inputCaptured = 1;
  }
  return 1;
}

int BaseWnd::endCapture() {
  if (!allowCapture) return 0;
  preventcancelcapture = 1;
  if (!inputCaptured) return 0;
  StdWnd::releaseCapture();
/*  if (oldCapture) { 
    DebugString("Restoring old capture\n");
    SetCapture(oldCapture); 
    oldCapture = NULL; 
  }*/
  inputCaptured = 0;
  preventcancelcapture = 0;
  return 1;
}

void BaseWnd::setAllowCapture(int allow) {
  allowCapture = allow;
}

int BaseWnd::isCaptureAllowed() {
  return allowCapture;
}

int BaseWnd::getCapture() {
#if defined(WIN32)
  if (inputCaptured && StdWnd::getCapture() == getOsWindowHandle() && curVirtualChildCaptured == NULL) return 1;
  return 0;
#elif defined(XWINDOW)
  return inputCaptured && curVirtualChildCaptured == NULL;
#else
#error port me!
#endif //platform
}

void BaseWnd::cancelCapture() {
  if (curVirtualChildCaptured != NULL) {
    curVirtualChildCaptured->cancelCapture();
    return;
  } 
  if (getCapture()) endCapture();
  onCancelCapture();
}

int BaseWnd::onMouseMove(int x, int y) { 
  onTipMouseMove();
  return 0;
}

void BaseWnd::onTipMouseMove() {
#ifdef WASABI_COMPILE_WND 
  POINT p;

  if (dragging) return;
  if (disable_tooltip_til_recapture) return;

  tipbeenchecked = TRUE;

  StdWnd::getMousePos(&p.x, &p.y);

  if (WASABI_API_WND->rootWndFromPoint(&p) != (RootWnd *)this) { // leaving area
    tip_done = FALSE;
    if (tipawaytimer)
      killTimer(TIP_AWAY_ID);
    tipawaytimer = FALSE;
    if (tipshowtimer) 
      killTimer(TIP_TIMER_ID);
    tipshowtimer = FALSE;
    destroyTip();
  } else { // moving in area
    const char *t = getTip();
    if (!disable_tooltip_til_recapture && !tipshowtimer && !tip_done && t != NULL && *t != 0) { //entering area & need tip
      setTimer(TIP_TIMER_ID, TIP_TIMER_THRESHOLD);
      tipshowtimer=TRUE;
    } else if (tipshowtimer) {
      killTimer(TIP_TIMER_ID);
      setTimer(TIP_TIMER_ID, TIP_TIMER_THRESHOLD);
    }
  }
#endif
}

int BaseWnd::onGetFocus() {	// return TRUE if you override this
  hasfocus = 1;
  notifyParent(ChildNotify::GOTFOCUS);
  getRootParent()->onSetRootFocus(this);
  invalidate();
#ifdef WASABI_API_ACCESSIBILITY
  Accessible *a = getAccessibleObject();
  if (a != NULL) 
    a->onGetFocus();
#endif
  return 1;
}

int BaseWnd::onKillFocus() {	// return TRUE if you override this
  hasfocus = 0;
  notifyParent(ChildNotify::KILLFOCUS);
  invalidate();
  return 1;
}

int BaseWnd::childNotify(RootWnd *child, int msg, int p1, int p2) {
  return 0;
}

int BaseWnd::addDragItem(const char *droptype, void *item) {
  ASSERT(droptype != NULL);
  if (item == NULL) return -1;
  DragSet *set;
  int pos = dragCheckData(droptype);
  if (pos == -1) {
    set = new DragSet();
    set->setName(droptype);
    dragsets.addItem(set);
    pos = dragsets.getNumItems() - 1;
  } else set = dragsets[pos];
  set->addItem(item);
  return pos;
}

int BaseWnd::handleDrag() {
  abortTip();
  if (dragsets.getNumItems() == 0) {
    dragComplete(0);
    return 0;
  }

  beginCapture();
  //StdWnd::setCapture(hwnd);
#if defined(WIN32)
  SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_APPSTARTING)));
#elif defined(XWINDOW)
  XWindow::setCursor( gethWnd(), XC_hand2 );
#else
#error port me!
#endif //platform

  dragging = 1;
  stickyWnd = NULL;

  return 1;
}

int BaseWnd::resetDragSet() {
  dragsets.deleteAll();
  return 1;
}

int BaseWnd::dragEnter(RootWnd *sourceWnd) {
  RootWnd *rw = getParent(); //RootWnd::rootwndFromRootWnd(getParent()); //FG> note to self, check!
  if (rw) return DI(rw).dragEnter(sourceWnd);
  return 0;
}

int BaseWnd::dragSetSticky(RootWnd *wnd, int left, int right, int up, int down){
  ASSERT(dragging);
  stickyWnd = wnd;

  if (left < 0) left = 0;
  if (right < 0) right = 0;
  if (up < 0) up = 0;
  if (down < 0) down = 0;

  Std::setRect(&sticky, left, up, right, down);

  return 1;
}

void BaseWnd::setSuggestedDropTitle(const char *title) {
  ASSERT(title != NULL);
  suggestedTitle = title;
}

const char *BaseWnd::dragGetSuggestedDropTitle() {
  return suggestedTitle;	// can be NULL
}

int BaseWnd::dragCheckData(const char *type, int *nitems) {
  for (int i = 0; i < dragsets.getNumItems(); i++) {
    if (STRCASEEQL(type, dragsets[i]->getName())) {
      if (nitems != NULL) *nitems = dragsets[i]->getNumItems();
      return i;
    }
  }
  return -1;
}

void *BaseWnd::dragGetData(int slot, int itemnum) {
  if (slot < 0 || slot >= dragsets.getNumItems()) return 0;
  if (itemnum < 0 || itemnum >= dragsets[slot]->getNumItems()) return 0;
  return dragsets[slot]->enumItem(itemnum);
}

const char *BaseWnd::dragGetType(int slot) {
  DragSet *set = dragsets[slot];
  if (set) return set->getName();
  return NULL;
}

int BaseWnd::dragGetNumTypes() {
  return dragsets.getNumItems();
}

void BaseWnd::addDroppedFile(const char *filename, PtrList<FilenameI> *plist) {
#ifdef RECURSE_SUBDIRS_ON_DROP
  const char *slash = filename + STRLEN(filename) - 1;
  for (; slash > filename; slash--) if (*slash == '/' || *slash == '\\') break;
  if (STREQL(slash+1, ".") || STREQL(slash+1, "..")) return;

  char buf[WA_MAX_PATH];
  STRCPY(buf, filename);
  // try to resolve shortcuts
  char *ext = buf + STRLEN(buf) - 1;
  for (; ext > buf; ext--) if (*ext == '.' || *ext == '\\' || *ext == '/') break;
#ifdef WIN32
  if (!STRICMP(ext, ".lnk")) {
    char buf2[MAX_PATH];
    if (StdFile::resolveShortcut(buf, buf2)) STRCPY(buf, buf2);
  }
#endif

  int isdir = 0;

  // handle root dir specially?
#if UTF8
  ReadDir testForDir(buf, NULL, 0/*don't skip dots*/); // using ReadDir to test for dir?
  if (testForDir.next()) {
    // the first file you get if you don't skip dots is your own self if you're a dir.
    isdir = testForDir.isDir();
  }
#else  
  WIN32_FIND_DATA data;
  HANDLE r = FindFirstFile(buf, &data);
  if (!r) return;
  FindClose(r);
  if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) isdir = 1;
#endif

  if (isdir) {
    onExternalDropDirScan(buf);
#if UTF8
    ReadDir dirFiles(buf); // default params means do the whole directory, recursively
    while (dirFiles.next()) {
      addDroppedFile(StringPrintf("%s%s%s", buf, DIRCHARSTR, dirFiles.getFilename()), plist);
    }
#else
    // enumerate that dir
    char search[WA_MAX_PATH];
    wsprintf(search, "%s\\*.*", buf);
    HANDLE files = FindFirstFile(search, &data);
    if (files == INVALID_HANDLE_VALUE) return;	// nothin' in it
    for (;;) {
      char obuf[WA_MAX_PATH];
      wsprintf(obuf, "%s\\%s", buf, data.cFileName);
      addDroppedFile(obuf, plist);
      if (!FindNextFile(files, &data)) {
        FindClose(files);
        return;
      }
    }
#endif
    // should never get here
  } else {
    addDragItem(DD_FILENAME, plist->addItem(new FilenameI(StringPrintf("file:%s", buf))));
  }
#else
    addDragItem(DD_FILENAME, plist->addItem(new FilenameI(StringPrintf("file:%s", filename))));
#endif
}

BOOL BaseWnd::getNoCopyBits(void) {
  return ncb;
}

void BaseWnd::setNoCopyBits(BOOL newncb) {
  ncb = newncb;
}

int BaseWnd::onEraseBkgnd(HDC dc) {
  if (wantEraseBackground()) {
#ifdef WIN32
    RECT r;
    BaseWnd::getClientRect(&r);
    if (virtualCanvas) {
      StretchBlt(dc, r.left, r.top, r.right-r.left, r.bottom-r.top, virtualCanvas->getHDC(), 0, 0, virtualCanvasW, virtualCanvasH, SRCCOPY); 
    } else {
      FillRect(dc, &r, (HBRUSH)GetStockObject(BLACK_BRUSH)); // port me
    }
#endif
  }
  return 1; 
}

void BaseWnd::setIcon(OSICONHANDLE icon, int _small) {
  StdWnd::setIcon(getOsWindowHandle(), icon, !_small);
//CUT  SendMessage(getOsWindowHandle(), WM_SETICON, _small ? ICON_SMALL : ICON_BIG, (int)icon);
}

const char *BaseWnd::getTip() {
  return tip;
}

void BaseWnd::setTip(const char *_tooltip) {
  tip = _tooltip;
  abortTip();
}

int BaseWnd::getStartHidden() {
  return start_hidden;
}

void BaseWnd::abortTip() {
  if (tipshowtimer)
    killTimer(TIP_TIMER_ID);
  tipshowtimer=FALSE;
  if (tipawaytimer)
    killTimer(TIP_AWAY_ID);
  tipawaytimer=FALSE;
  if (tipdestroytimer)
    killTimer(TIP_DESTROYTIMER_ID);
  tipdestroytimer=FALSE;
  destroyTip();
  tip_done=FALSE;
  RECT r;
  if (getOsWindowHandle() && StdWnd::getUpdateRect(getOsWindowHandle(), &r) != 0) // FG> avoids xoring over disapearing tip
    repaint();
}

int BaseWnd::isVisible(int within) {
  if (!isVirtual() && !getOsWindowHandle()) return 0;
  if (!this_visible) return 0; 

  if (within) return this_visible; // whatever, local

  if (isVirtual()) // virtual, global
    if (getParent())
      return getParent()->isVisible();
    else
      return 0;
  
  // non virtual, global
  if (StdWnd::isPopup(getOsWindowHandle())) return this_visible;
#ifndef XWINDOW // this is meaningless in X -- only the root window has no parent.
  if (StdWnd::getParent(getOsWindowHandle()) == INVALIDOSWINDOWHANDLE) return this_visible;
#endif
  if (!StdWnd::isValidWnd(StdWnd::getParent(getOsWindowHandle()))) return 0;
  if (getParent()) return getParent()->isVisible(); // not a popup, check its parent or fail
//keith's original version of the above line from his Linux port:
//  if (getParent()) return this_visible && getParent()->isVisible(); // not a popup, check its parent or fail
  return this_visible;
}

void BaseWnd::registerRootWndChild(RootWnd *child) {
  rootwndchildren.addItem(child);
  if (child->isVirtual())
    virtualChildren.addItem(child);
}

void BaseWnd::unregisterRootWndChild(RootWnd *child) {
  delTabOrderEntry(child);
  rootwndchildren.removeItem(child);
  if (curVirtualChildCaptured == child) {
    setVirtualChildCapture(NULL);
  }
  if (curVirtualChildFocus == child)
    curVirtualChildFocus = NULL;
  virtualChildren.removeItem(child);
  if (isPostOnInit() && isVisible())
    postDeferredCallback(DEFERREDCB_INVALIDATE, 0);
}

int BaseWnd::isVirtual() { 
  return 0; 
}

//CUT?inline int isInRect(RECT *r,int x,int y) {
//CUT?  if (x>=r->left&&x<=r->right&&y>=r->top&&y<=r->bottom) return 1;
//CUT?  return 0;
//CUT?}

int BaseWnd::ensureVirtualCanvasOk() {
  ASSERT(buffered);
  RECT ncr;

  if (isVirtual() && getParent()) return 1;

  RECT r;
  BaseWnd::getClientRect(&r);

  int size_w = r.right-r.left;
  int size_h = r.bottom-r.top;
                    
  if (!size_w||!size_h) return 0;

  if (!virtualCanvas || virtualCanvasH!=size_h || virtualCanvasW!=size_w) {
    if (virtualCanvas) {
      deleteFrameBuffer(virtualCanvas);
      virtualCanvas=NULL;
    }
    delete scalecanvas; scalecanvas = NULL;
    virtualCanvas = createFrameBuffer(size_w, size_h);
    prepareFrameBuffer(virtualCanvas, size_w, size_h);
    virtualCanvas->setBaseWnd(this);
    virtualCanvasH=size_h; virtualCanvasW=size_w;
    BaseWnd::getNonClientRect(&ncr);
    deferedInvalidateRect(&ncr);
  }
  return 1;
}

Canvas *BaseWnd::createFrameBuffer(int w, int h) {
  return new BltCanvas(w,h);      
}

void BaseWnd::prepareFrameBuffer(Canvas *canvas, int w, int h) {
  ASSERT(buffered);
  RECT r={0,0,w, h};
  RegionI reg(&r);
  virtualBeforePaint(&reg);
  canvas->selectClipRgn(NULL);
  canvas->fillRect(&r, 0);
  virtualAfterPaint(&reg);
}

void BaseWnd::deleteFrameBuffer(Canvas *canvas) {
  delete canvas;
}

// paints the client content, followed by the virtual child tree. recursive
int BaseWnd::paintTree(Canvas *canvas, Region *r) {

  onPaint(canvas, r);
  
  if (!buffered) return 1;

#ifdef WASABI_DRAW_FOCUS_RECT
  if (gotFocus()) {
    RECT ncr;
    getNonClientRect(&ncr);
    // try to use skinned focus rect
#ifdef WASABI_COMPILE_WND 
    if (WASABI_API_WND->paintset_present(Paintset::FOCUSRECT))
      WASABI_API_WND->paintset_render(Paintset::FOCUSRECT, canvas, &ncr, 128);
    else // otherwise this looks kinda nice :P
#endif
      canvas->drawRect(&ncr, 0, 0xFFFFFF, 128);
  }
#endif

  if (isVirtual() && !hasVirtualChildren()) return 0;

  Region *hostrgn = NULL;
  Region *update = r;

  if (!(hwnd != INVALIDOSWINDOWHANDLE && getParent() == NULL)) {
    hostrgn = getRegion();
    update = r->clone();
    if (hostrgn && !isRectRgn()) {
      RECT ncr = clientRect();
      Region *hostclone = hostrgn->clone();
      hostclone->offset(ncr.left, ncr.top);
      update->andRegion(hostclone);
      hostrgn->disposeClone(hostclone);
    }
  }

  RegionI *client_update = new RegionI();//FUCKO: why new
  for (int i=0;i<virtualChildren.getNumItems();i++) {
    if (!virtualChildren[i]->isVisible(1)) continue;
    RECT rChild;
    RootWnd *w = virtualChildren[i];
    w->getNonClientRect(&rChild);
    if ((rChild.right != rChild.left) && (rChild.bottom != rChild.top))
    if (update->intersectRect(&rChild, client_update)) {
      w->paintTree(canvas, client_update);
    }
  }
  delete client_update;
  if (update != r) r->disposeClone(update);

  return 1;
}

void BaseWnd::setVirtualCanvas(Canvas *c) {
  ASSERT(buffered);
  virtualCanvas = c;
}

int BaseWnd::pointInWnd(POINT *p) {
  RECT r;
  if (!isVisible(1)) return 0;
  getWindowRect(&r);
  if (!Std::pointInRect(&r, *p))
    return 0;
  for (int i = 0; i < getNumRootWndChildren(); i++) {
    RootWnd *c = enumRootWndChildren(i);
    if (!c->isVisible(1)) continue;
    RECT rChild;
    c->getWindowRect(&rChild);
    if (Std::pointInRect(&rChild, *p))
      return 0;
  }
  //NONPORTABLE
/*  HWND child = GetWindow(getOsWindowHandle(), GW_CHILD);
  while (child != NULL) {
    if (IsWindowVisible(child)) {
      RECT r2;
      GetWindowRect(child, &r2);
      if (Std::pointInRect(&r2, *p))
        return 0;
    }
    child = GetWindow(child, GW_HWNDNEXT);
  }*/
  return 1;
}

int BaseWnd::paint(Canvas *c, Region *r) {
  if (isVirtual()) {
    RegionI d;
    RECT cr;
    getClientRect(&cr);
    if (r == NULL) {
      d.addRect(&cr);
    } else {
      d.addRegion(r); 
      d.offset(cr.left, cr.top);
    }
    RootWnd *rp = getRootParent();
    deferedInvalidate();
    rp->paint(NULL, &d);
    BltCanvas *cc = static_cast<BltCanvas*>(rp->getFrameBuffer());
    if (r != NULL) c->selectClipRgn(r);
    cc->blit(cr.left, cr.top, c, 0, 0, cr.right-cr.left, cr.bottom-cr.top);
    return 1;
  }

  if (buffered && !ensureVirtualCanvasOk()) return 0;
  RegionI *deleteme = NULL;

  if (r == NULL) {
    RECT cr;
    getNonClientRect(&cr);
    deleteme = new RegionI(&cr);
    r = deleteme;
  }

  if (buffered) virtualBeforePaint(r);

  RECT rcPaint;
  r->getBox(&rcPaint);

  double ra = getRenderRatio();

  Canvas *dest = buffered ? virtualCanvas : c;

  if (!buffered || deferedInvalidRgn) {
    Region *nr=NULL;
    if (renderRatioActive()) {
      nr=r->clone();
      double d=1.0/ra;
      nr->scale(d,d,TRUE); 
    }
    RegionI i;
    if (buffered) {
#ifdef MULTITHREADED_INVALIDATIONS
      INCRITICALSECTION(s_invalidation_cs);
#endif
      if (deferedInvalidRgn->intersectRgn(nr?nr:r, &i)) { // some deferednvalidated regions needs to be repainted
        onBeforePaintTree(dest, &i);
        paintTree(dest, &i);
        if (buffered) deferedValidateRgn(&i);
      } 
    } else {
      onBeforePaintTree(dest, r);
      paintTree(dest, r);
    }
    if (nr) r->disposeClone(nr);
  }

  if (c != NULL && buffered)
    commitFrameBuffer(c, &rcPaint, ra);

  if (buffered) virtualAfterPaint(r);

  delete deleteme;
  return 1;
}

int BaseWnd::virtualOnPaint() {

  RECT cr;
  getNonClientRect(&cr);
  if (cr.left >= cr.right || cr.top >= cr.bottom) return 0;

  if (buffered && !ensureVirtualCanvasOk()) return 0;

  RegionI reg;

//CUT  GetUpdateRgn(getOsWindowHandle(), r->getOSHandle(), FALSE);
  StdWnd::getUpdateRegion(getOsWindowHandle(), reg.getOSHandle());

  PaintCanvas paintcanvas;
  if (!paintcanvas.beginPaint(this)) { virtualAfterPaint(&reg); return 0; }

#if defined(WASABI_PLATFORM_WIN32)
  // DO NOT DELETE - This looks like it does nothing, but it actually makes the GDI call us again with WM_PAINT if some window
  // moves over this one between BeginPaint and EndPaint. We still use GetUpdateRgn so we don't have to check for
  // the version of Windows. See doc. If this function is not available (should be here in 95/98/NT/2K, but we never know)
  // then we use the rcPaint rect... less precise, but still works.
//CUT  if (getRandomRgn) {
  if (StdWnd::haveGetRandomRgn()) {
    RegionI zap;
//CUT    getRandomRgn(paintcanvas.getHDC(), zap.getOSHandle(), SYSRGN);
    StdWnd::getRandomRgn(paintcanvas.getHDC(), zap.getOSHandle());
  } else { 
    RECT z;
    paintcanvas.getRcPaint(&z);
    reg.setRect(&z);
  }
#endif // platform

  // -------------

// for debug
/*
  HDC dc = GetDC(getOsWindowHandle());
  InvertRgn(dc, reg.getOSHandle());
  InvertRgn(dc, reg.getOSHandle());
  ReleaseDC(getOsWindowHandle(), dc); 
*/

  paint(&paintcanvas, &reg);

  return 1;
}

RootWnd *BaseWnd::enumVirtualChild(int _enum) {
  return virtualChildren[_enum];
}

int BaseWnd::getNumVirtuals() {
  return virtualChildren.getNumItems();
}

RootWnd *BaseWnd::enumRootWndChildren(int n) {
  return rootwndchildren.enumItem(n);
}

int BaseWnd::getNumRootWndChildren() {
  return rootwndchildren.getNumItems();
}

RootWnd *BaseWnd::findRootWndChild(int x,int y, int only_virtuals, int throughtest) {
  for(int i = getNumRootWndChildren()-1; i > -1; i--) {
    RECT r;
    RootWnd *child = enumRootWndChildren(i);
//    DebugString("findRootWndChild = entering = %s\n", child->getId());
    if (only_virtuals && !child->isVirtual()) continue;
    child->getNonClientRect(&r);
    int _x = x;
    int _y = y;
    if (!child->isVirtual()) {
      POINT pt;
      child->getPosition(&pt);
      _x -= pt.x;
      _y -= pt.y;
    } 
    int iv = child->isVisible(1);
    //int gpa = child->getPaintingAlpha();
    if (iv /*&& gpa > 0*/ && _x>=r.left&&_x<=r.right&&_y>=r.top&&_y<=r.bottom) { // GROUP
      RootWnd *z = child->findRootWndChild(_x, _y, only_virtuals, throughtest);
      if (z) return z; 
    }
/*gpa > 0 &&*/
    /*if (iv && _x>=r.left&&_x<=r.right&&_y>=r.top&&_y<=r.bottom && !child->isClickThrough() && child->ptInRegion(_x, _y)) { 
      return child;
    }*/
  }
  switch (throughtest) {
    default:
    case THROUGHTEST_NONE: break;
    case THROUGHTEST_NOCLICKTHROUGH: if (isClickThrough()) return NULL; break;
    case THROUGHTEST_NODNDTHROUGH: if (isDndThrough()) return NULL; break;
    case THROUGHTEST_NOCLICKORDNDTHROUGH: if (isClickThrough() || isDndThrough()) return NULL; break;
  }
  return ptInRegion(x, y) ? this : NULL;
}

//PORTME
int BaseWnd::handleVirtualChildMsg(UINT uMsg, int x, int y, void *p, void *d) {

  RootWnd *child=NULL;

  if(curVirtualChildCaptured) 
    child=curVirtualChildCaptured;
  else 
    child=findRootWndChild(x, y, 1, THROUGHTEST_NOCLICKTHROUGH); // warning, can return this which is not virtual

//  ASSERT(child != NULL);	// BU this came up and I don't know why, looks like it should never happen
// FG> actually it can happen when coming back from a popup menu when cpu usage is high, the window might be
// hidden (destroying) and ptInRegion returns false.
  if (!child) return 0;

  int isvirtual = child->isVirtual();

  if (child) child = child->getForwardWnd(); 

  if (child && child->isEnabled()) {
    switch(uMsg) {
      case WM_LBUTTONDOWN:
/*        if (isvirtual && child != curVirtualChildFocus)
          focusVirtualChild(child);*/
        autoFocus(child);
        if (child->wantLeftClicks())
          child->onLeftButtonDown(x, y);
        if (child->checkDoubleClick(uMsg, x, y) && child->wantDoubleClicks())
          child->onLeftButtonDblClk(x, y);
      return 1;
      case WM_MBUTTONDOWN:
/*        if (isvirtual && child != curVirtualChildFocus)
          focusVirtualChild(child);*/
        autoFocus(child);
        if (child->wantMiddleClicks())
          child->onMiddleButtonDown(x, y);
        if (child->checkDoubleClick(uMsg, x, y) && child->wantDoubleClicks())
          child->onMiddleButtonDblClk(x, y);
      return 1;
      case WM_RBUTTONDOWN:
/*        if (isvirtual && child != curVirtualChildFocus)
          focusVirtualChild(child);*/
        autoFocus(child);
        if (child->wantRightClicks())
          child->onRightButtonDown(x, y);
        if (child->checkDoubleClick(uMsg, x, y) && child->wantDoubleClicks())
          child->onRightButtonDblClk(x, y);
      return 1;

      case WM_LBUTTONUP:
        if (child->wantLeftClicks())
          child->onLeftButtonUp(x, y);
      return 1;
      case WM_MBUTTONUP:
        if (child->wantMiddleClicks())
          child->onMiddleButtonUp(x, y);
      return 1;
      case WM_RBUTTONUP:
        if (child->wantRightClicks())
          child->onRightButtonUp(x, y);
      return 1;

      case WM_MOUSEMOVE:{
        if (curVirtualChildCaptured == child || (curVirtualChildCaptured == NULL)) {
          if (child->wantMouseMoves())
            child->onMouseMove(x, y);
          return 1;
        }
        return 0;
      }
      case WM_SETCURSOR:
        int a = child->getCursorType(x, y);
        if (!p) return 0;
        *(int *)p = a;
        if (a == BASEWND_CURSOR_USERSET) {
          OSCURSORHANDLE c = child->getCustomCursor(x, y);
          if (!d) return 0;
          *(OSCURSORHANDLE *)d = c;
        }
        return 1;
    }
  }
  return 0;
}

int BaseWnd::onLeftButtonDown(int x, int y) {
  disable_tooltip_til_recapture = 1;
  abortTip();
  return 0;
}

int BaseWnd::onLeftButtonUp(int x, int y) {
  disable_tooltip_til_recapture = 1;
  abortTip();
  return 0;
}

void BaseWnd::setVirtualChildCapture(RootWnd *child) {
  if (child) {
    if (!inputCaptured) {
      beginCapture();
    }
  } else {
    endCapture();
  }
  curVirtualChildCaptured = child;
}

RootWnd *BaseWnd::getVirtualChildCapture() {
  if (inputCaptured && StdWnd::getCapture() == getOsWindowHandle())
    return curVirtualChildCaptured;
  //CUT else
  //CUT  if (inputCaptured) inputCaptured=0;
  return NULL;
}

RootWnd *BaseWnd::getBaseTextureWindow() {
  // return our base texture window if we have it
  if (btexture)
    return btexture;
  // return our parent's if they have it
  if (getParent())
    return getParent()->getBaseTextureWindow();
  else
    return NULL;
}

void BaseWnd::renderBaseTexture(CanvasBase *c, const RECT &r, int alpha) {
#ifdef WASABI_COMPILE_SKIN 
  WASABI_API_SKIN->skin_renderBaseTexture(getBaseTextureWindow(), c, (RECT *)&r, this, alpha);
#endif
}

void BaseWnd::setBaseTextureWindow(RootWnd *w) {
  btexture = w;
}

void BaseWnd::setNotifyWindow(RootWnd *newnotify) {
  notifyWindow = newnotify;
}

RootWnd *BaseWnd::getNotifyWindow() {
  return destroying ? NULL : notifyWindow;
}

int BaseWnd::gotFocus() {
  return hasfocus && curVirtualChildFocus == NULL;
}

int BaseWnd::isActive() {
  OSWINDOWHANDLE h = hwnd;
  if (h == INVALIDOSWINDOWHANDLE) {
    RootWnd *par = getParent();
    if (par == NULL) return 0;
    h = par->getOsWindowHandle();
  }
  if (h == INVALIDOSWINDOWHANDLE) return 0;
  return (StdWnd::getActiveWindow() == h);
}

int BaseWnd::onChar(unsigned int c) {
  switch (c) {
    case 9: // TAB
      if (Std::keyDown(VK_SHIFT))
        focusPrevious();
      else
        focusNext();
      return 1;
  }
  return 0;
}

/*int BaseWnd::focusVirtualChild(RootWnd *child) {
  if (!gotFocus()) setFocus();
  if (!child->wantFocus()) return 0;
  setVirtualChildFocus(child);
  return 1;
}*/

int BaseWnd::wantFocus() {
  return 0;
}

// Return 1 if there is a modal window that is not this
int BaseWnd::checkModal() {
  if (bypassModal()) return 0;
#ifdef WASABI_COMPILE_WND // this ifdef is kinda wrong... and kinda not
  RootWnd *w = WASABI_API_WND->getModalWnd();
  if (w && w != static_cast<RootWnd*>(this) && w != getDesktopParent()) {
    return 1;
  }
#endif
  return 0;
}

int BaseWnd::cascadeRepaintFrom(RootWnd *who, int pack) {
  RECT r;
  BaseWnd::getNonClientRect(&r);
  return BaseWnd::cascadeRepaintRect(&r, pack);
}

int BaseWnd::cascadeRepaint(int pack) {
  return cascadeRepaintFrom(this, pack);
}

int BaseWnd::cascadeRepaintRgn(Region *r, int pack) {
  return cascadeRepaintRgnFrom(r, this, pack);
}

int BaseWnd::cascadeRepaintRect(RECT *r, int pack) {
  return cascadeRepaintRectFrom(r, this, pack);
}

int BaseWnd::cascadeRepaintRectFrom(RECT *r, RootWnd *who, int pack) {
  RegionI reg(r);
  int rt = cascadeRepaintRgnFrom(&reg, who, pack);
  return rt; 
}

void BaseWnd::_cascadeRepaintRgn(Region *rg) {//PORTME (HDC stuff)
  if (!buffered) {
    invalidateRgn(rg);
    StdWnd::update(gethWnd());
    return;
  }

  if (!ensureVirtualCanvasOk()) return;

  HDC dc = StdWnd::getDC(getOsWindowHandle());
  if (dc == NULL) return;

  virtualBeforePaint(rg);

  deferedInvalidateRgn(rg);
  paintTree(virtualCanvas, rg);

  DCCanvas *paintcanvas;
  paintcanvas = new DCCanvas();
  paintcanvas->cloneDC(dc, this);

  double ra = getRenderRatio();

  RECT rcPaint;
  rg->getBox(&rcPaint);

  RECT rc;
  getClientRect(&rc); //JF> this gets it in virtual (non-scaled) coordinates,
                      // so we need to do these comparisons before scaling.
  rcPaint.bottom = MIN((int)rc.bottom, (int)rcPaint.bottom);
  rcPaint.right = MIN((int)rc.right, (int)rcPaint.right);
 
  if (renderRatioActive()) // probably faster than scaling the clone
  {
    rcPaint.left=(int)((rcPaint.left-1)*ra);
    rcPaint.top=(int)((rcPaint.top-1)*ra);
    rcPaint.right=(int)(rcPaint.right*ra + 0.999999);
    rcPaint.bottom=(int)(rcPaint.bottom*ra + 0.999999);
  }
  rcPaint.left = MAX(0, (int)rcPaint.left);
  rcPaint.top = MAX(0, (int)rcPaint.top);
  rcPaint.right = MIN((int)rcPaint.right, (int)(rwidth*ra));
  rcPaint.bottom  = MIN((int)rcPaint.bottom, (int)(rheight*ra));

  commitFrameBuffer(paintcanvas, &rcPaint, ra);
  virtualAfterPaint(rg);

  StdWnd::releaseDC(getOsWindowHandle(), dc);

  delete paintcanvas;
}

void BaseWnd::packCascadeRepaintRgn(Region *rg) {
  if (!deferedCascadeRepaintRgn) deferedCascadeRepaintRgn = new RegionI;
  deferedCascadeRepaintRgn->addRegion(rg);
  need_flush_cascaderepaint = 1;
}

int BaseWnd::cascadeRepaintRgnFrom(Region *_rg, RootWnd *who, int pack) {

  Region *rg = _rg->clone();

  int j = virtualChildren.searchItem(who);
  for (int i = 0; i < virtualChildren.getNumItems(); i++) {
    RootWnd *w = virtualChildren[i];
    if (w != who && w->wantSiblingInvalidations())
      w->onSiblingInvalidateRgn(rg, who, j, i);
  }

  if (!pack) {
    _cascadeRepaintRgn(rg);
  } else {
    packCascadeRepaintRgn(rg);
  }

  _rg->disposeClone(rg);

  return 1;
}

void BaseWnd::setDesktopAlpha(int a) {
  if (a && !StdWnd::isDesktopAlphaAvailable()) return;
  if (a == w2k_alpha) return;
  w2k_alpha = a;
  if (!a && scalecanvas) {
    delete scalecanvas; scalecanvas = NULL;
  }
  setLayeredWindow(w2k_alpha);
  onSetDesktopAlpha(a);
}

void BaseWnd::onSetDesktopAlpha(int a) { };

void BaseWnd::commitFrameBuffer(Canvas *paintcanvas, RECT *r, double ra) {
  ASSERT(buffered);

  if (w2k_alpha && StdWnd::isDesktopAlphaAvailable() && !cloaked) {
//CUT    BLENDFUNCTION blend= {AC_SRC_OVER, 0, wndalpha, AC_SRC_ALPHA };
//CUT    POINT pt={0,0};
    RECT spr;
    getWindowRect(&spr);
//CUT    POINT sp={spr.left,spr.top};
//CUT    SIZE ss={spr.right-spr.left, spr.bottom-spr.top};
    int sw = spr.right-spr.left, sh = spr.bottom-spr.top;
//CUT    SysCanvas c;

    if (handleRatio() && renderRatioActive()) {
      // eek slow!
      RECT r;
      getWindowRect(&r);
      int w=r.right-r.left;
      int h=r.bottom-r.top;
      if (!scalecanvas) scalecanvas = new BltCanvas(w, h);
      virtualCanvas->stretchblit(0, 0, (int)((double)virtualCanvasW * 65536.0), (int)((double)virtualCanvasH * 65536.0), scalecanvas, 0, 0, w, h);
    }

//CUT    updateLayeredWindow(hwnd, c.getHDC(), &sp, &ss, (scalecanvas ? scalecanvas : virtualCanvas)->getHDC(), &pt, 0, &blend, ULW_ALPHA);
    StdWnd::updateLayeredWnd(hwnd, spr.left, spr.top, sw, sh, (scalecanvas ? scalecanvas : virtualCanvas)->getHDC(), wndalpha);
  } else {
    if (ABS(ra-1.0) <= 0.01) {
      virtualCanvas->blit(r->left,r->top,paintcanvas,r->left,r->top,r->right-r->left,r->bottom-r->top);
    } else {
      RECT tr=*r;

      double invra=65536.0/ra;
      int lp=tr.left;
      int tp=tr.top;
      int w=tr.right-tr.left;
      int h=tr.bottom-tr.top;

      int sx=(int)((double)lp * invra);
      int sy=(int)((double)tp * invra);
      int sw=(int)((double)w * invra);
      int sh=(int)((double)h * invra);
   
      virtualCanvas->stretchblit(sx,sy,sw,sh, paintcanvas, lp, tp, w, h);
    }
  }
}

void BaseWnd::flushPaint() {
  postDeferredCallback(DEFERREDCB_FLUSHPAINT, 0);
}

void BaseWnd::do_flushPaint() {
#ifdef MULTITHREADED_INVALIDATIONS
  INCRITICALSECTION(s_invalidation_cs);
#endif
  if (!deferedInvalidRgn || deferedInvalidRgn->isEmpty()) return;
  Region *r = deferedInvalidRgn->clone();
  cascadeRepaintRgn(r);
  deferedInvalidRgn->disposeClone(r);
  deferedInvalidRgn->empty();
}

int BaseWnd::isMouseOver(int x, int y) {
  POINT pt={x, y};
  clientToScreen(&pt);
#ifdef WASABI_COMPILE_WND 
  return (WASABI_API_WND->rootWndFromPoint(&pt) == this);
#else
  RECT ncr = nonClientRect();
  return Std::pointInRect(&ncr, x, y);
#endif
}

void BaseWnd::freeResources() {
}

void BaseWnd::reloadResources() {
  invalidate(); 
}

double BaseWnd::getRenderRatio() {
  if (!handleRatio()) return 1.0;
  if (!ratiolinked) return ratio;
  return getParent() ? getParent()->getRenderRatio() : ratio;
}

void BaseWnd::setRenderRatio(double r) {
  // "snap" to 1.0
  if (ABS(r - 1.0) <= 0.02f) r = 1.0;
  if (scalecanvas) {
    delete scalecanvas; scalecanvas = NULL;
  }
  if (isInited() && r != ratio && !isVirtual() && (!getParent() || !ratiolinked)) {
    // must scale size & region accordingly
    RECT rc;
    BaseWnd::getWindowRect(&rc);
    rc.right = rc.left + rwidth;
    rc.bottom = rc.top + rheight;
    ratio = r;

    resize(&rc);

    invalidate();
  }
}

void BaseWnd::setRatioLinked(int l) {
  ratiolinked = l;
  if (isPostOnInit())
    setRenderRatio(ratio);
}

int BaseWnd::renderRatioActive() {
  return ABS(getRenderRatio() - 1.0) > 0.01f;
}

void BaseWnd::multRatio(int *x, int *y) {
  double rr = getRenderRatio();
  if (x) *x = (int)((double)(*x) * rr);
  if (y) *y = (int)((double)(*y) * rr);
}

void BaseWnd::multRatio(RECT *r) {
  double rr = getRenderRatio();
  r->left = (int)((double)(r->left) * rr);
  r->right = (int)((double)(r->right) * rr);
  r->top = (int)((double)(r->top) * rr);
  r->bottom = (int)((double)(r->bottom) * rr);
}

void BaseWnd::divRatio(int *x, int *y) {
  double rr = getRenderRatio();
  if (x) *x = (int)((double)(*x) / rr + 0.5);
  if (y) *y = (int)((double)(*y) / rr + 0.5);
}

void BaseWnd::divRatio(RECT *r) {
  double rr = getRenderRatio();
  r->left = (int)((double)(r->left) / rr + 0.5);
  r->right = (int)((double)(r->right) / rr + 0.5);
  r->top = (int)((double)(r->top) / rr + 0.5);
  r->bottom = (int)((double)(r->bottom) / rr + 0.5);
}

void BaseWnd::bringVirtualToFront(RootWnd *w) {
  changeChildZorder(w, 0);
}

void BaseWnd::bringVirtualToBack(RootWnd *w) {
  changeChildZorder(w, virtualChildren.getNumItems());
}

void BaseWnd::bringVirtualAbove(RootWnd *w, RootWnd *b) {
  ASSERT(b->isVirtual());
  int p = virtualChildren.searchItem(b);
  if (p == -1) return;
  changeChildZorder(w, p);
}

void BaseWnd::bringVirtualBelow(RootWnd *w, RootWnd *b) {
  ASSERT(b->isVirtual());
  int p = virtualChildren.searchItem(b);
  if (p == -1) return;
  changeChildZorder(w, p+1);
}

void BaseWnd::changeChildZorder(RootWnd *w, int newpos) {
  int i;
  int p = newpos;
  p = MAX(p, (int)0);
  p = MIN(p, virtualChildren.getNumItems());
  RECT cr;
  w->getClientRect(&cr);

  PtrList<RootWnd> l;
  for (i=0;i<virtualChildren.getNumItems();i++)
    if (virtualChildren[i] != w)
      l.addItem(virtualChildren[i]);

  p = virtualChildren.getNumItems() - newpos -1;
  virtualChildren.removeAll();

  int done = 0;

  for (i=0;i<l.getNumItems();i++)
    if (i==p && !done) {
      virtualChildren.addItem(w);
      i--;
      done++;
    } else {
      RECT dr, intersection;
      l.enumItem(i)->getClientRect(&dr);
      if (Std::rectIntersect(&intersection, &dr, &cr))
        l[i]->invalidateRect(&intersection);
      virtualChildren.addItem(l.enumItem(i));
    }
  if (i==p && !done) 
    virtualChildren.addItem(w);
  w->invalidate();
}

int BaseWnd::onActivate() {
  if (hasVirtualChildren()) {
    int l = getNumVirtuals();
    for(int i = 0; i < l; i++) {
      RootWnd *r = enumVirtualChild(i);
      r->onActivate();
    }
  }
  return 0;
}

int BaseWnd::onDeactivate() {
  if (hasVirtualChildren()) {
    int l = getNumVirtuals();
    for(int i = 0; i < l; i++) {
      RootWnd *r = enumVirtualChild(i);
      r->onDeactivate();
    }
  }
  return 0;
}

int BaseWnd::getDesktopAlpha() {
  return w2k_alpha;
}

Region *BaseWnd::getRegion() {
  return NULL;  
}

//CUT int BaseWnd::isTransparencyAvailable() {
//CUT #ifdef WIN32
//CUT #else
//CUT #pragma warning port me!
//CUT #endif
//CUT   return 0;
//CUT }

int BaseWnd::handleTransparency() {
  return 1; // by default all windows handle transparency, only windows blitting directly on the SCREEN (if you blit directly on the DC it's still ok), 
}           // for instance, a vis or a video using overlay should return 0, this will let the layout auto manage its alpha as that window is shown/hiden

void BaseWnd::setAlpha(int active, int inactive) {
  if (active == 254) active = 255;
  if (active == 1) active = 0;
  if (inactive == 254) inactive = 255;
  if (inactive == 1) inactive = 0;
  int oldactivealpha = activealpha;
  int oldinactivealpha = inactivealpha;
  active = MIN(255, MAX(0, active));
  if (inactive != -1) inactive = MIN(255, MAX(0, inactive));

  if (active != activealpha) {
    activealpha = active;
    if (isActive()) {
      invalidate();
      if ((oldactivealpha == 0 || activealpha == 0) && (oldactivealpha != 0 || activealpha != 0))
        invalidateWindowRegion();
    }
  }
  if (inactive == -1) inactive = active;
  if (inactive != inactivealpha) {
    inactivealpha = inactive;
    if (!isActive()) {
      invalidate();
      if ((oldactivealpha == 0 || activealpha == 0) && (oldactivealpha != 0 || activealpha != 0))
        invalidateWindowRegion();
    }
  }
}

void BaseWnd::getAlpha(int *active, int *inactive) {
  if (active) *active = activealpha;
  if (inactive) *inactive = inactivealpha;
}

int BaseWnd::getPaintingAlpha(void) {
  int a = isActive() ? MIN(255, MAX(0, activealpha)) : MIN(255, MAX(0, inactivealpha));
  ASSERT(a >= 0 && a <= 255);
  if (getParent() && getParent()->isVirtual()) {
    int b = getParent()->getPaintingAlpha();
    a = int(double(a)/255.0) * b; // function-style casting is clearer.
  }
  if (a == 254) a = 255;
  if (a == 1) a = 0;
  if (!isEnabled()) a = (a*3)/5; //a *= 0.6; // GCC warns about the cast, and we can avoid floating point arith.
  return a;
}

void BaseWnd::setClickThrough(int ct) {
  clickthrough = ct;
}

int BaseWnd::isClickThrough() {
  return clickthrough;
}

int BaseWnd::handleRatio() { return 1; }

//? #include <common/script/c_script/c_rootobj.h>

int BaseWnd::createTip() {
#ifdef USE_TOOLTIPS
  destroyTip();
  tooltip = new Tooltip(getTip());
#endif
  return -1;
}

void BaseWnd::destroyTip() {
#ifdef USE_TOOLTIPS
  // this is to avoid pb if destroytip() is being called by a time while destroying tip
  Tooltip *tt = tooltip;
  tooltip = NULL;
  delete tt;
#endif
}

int BaseWnd::runModal() {	//PORTME

  RootWnd *dp = getDesktopParent();
  if (dp && dp != this)
    return dp->runModal();

  MSG msg;

//  SetCapture(NULL);
  StdWnd::setFocus(getOsWindowHandle());

#ifdef WASABI_COMPILE_WND 
  WASABI_API_WND->pushModalWnd(this);
#else
//FUCKO: do something!
#endif
  returnvalue = 0;
  mustquit = 0;

   // Main message loop:
  while (!mustquit) {
    mustquit = !GetMessage(&msg, INVALIDOSWINDOWHANDLE, 0, 0);
#ifdef WIN32
    if (!msg.hwnd || !TranslateAccelerator(msg.hwnd, NULL, &msg)) 
#endif
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

#ifdef WASABI_COMPILE_WND 
  WASABI_API_WND->popModalWnd(this);
#else
//FUCKO: do something!
#endif
//  SetCapture(NULL);
  return returnvalue;
}

void BaseWnd::endModal(int ret) {
  RootWnd *dp = getDesktopParent();
  if (dp && dp != this) {
    dp->endModal(ret);
    return;
  }
  returnvalue = ret;
  mustquit = 1;
}

int BaseWnd::wantAutoContextMenu() { 
  return 1; 
}

void BaseWnd::onCancelCapture() {
  if (dragging) {
    dragging = 0;
    DI prevtargdi(prevtarg);
    prevtargdi.dragLeave(this);
    dragComplete(0);
    prevtarg = NULL;
    prevcandrop = 0;
    resetDragSet();
    stickyWnd = NULL;
    suggestedTitle = NULL;
  }
}

RootWnd *BaseWnd::getNextVirtualFocus(RootWnd *w) {
  if (w == NULL) {
    if (childtabs.getNumItems() > 0)
      return childtabs.getFirst()->wnd;
  }

  int a = getTabOrderEntry(w)+1;

  if (a < childtabs.getNumItems()) 
    return childtabs.enumItem(a)->wnd;

  return NULL;
}

void BaseWnd::setVirtualChildFocus(RootWnd *w) {
  if (!w) {
    curVirtualChildFocus = NULL;
    return;
  }
  ASSERT(w->isVirtual());
  if (curVirtualChildFocus)
    curVirtualChildFocus->onKillFocus();
  curVirtualChildFocus = w; 
  onSetRootFocus(w);
  StdWnd::setFocus(getOsWindowHandle());
  if (curVirtualChildFocus)
    curVirtualChildFocus->onGetFocus();
}

int BaseWnd::ptInRegion(int x, int y) {
  RECT cr;
  getNonClientRect(&cr);
  POINT pt={x-cr.left,y-cr.top};
  Region *reg = getRegion();
  if (isRectRgn()) 
    return (x >= cr.left && x <= cr.right && y >= cr.top && y <= cr.bottom);
  return reg ? reg->ptInRegion(&pt) : 0;
}

Region *BaseWnd::getComposedRegion() {
  ensureWindowRegionValid();
  return composedrgn;
}

Region *BaseWnd::getSubtractorRegion() {
  ensureWindowRegionValid();
  return subtractorrgn;  
}

int BaseWnd::ensureWindowRegionValid() {
  if (!isInited()) return 0;
  if (wndregioninvalid) {
    computeComposedRegion();
    return 1;
  }
  return 0;
}

void BaseWnd::invalidateWindowRegion() {
  wndregioninvalid = 1;
  if (getParent()) getParent()->invalidateWindowRegion();
}

void BaseWnd::computeComposedRegion() {
  if (!isPostOnInit()) return;

  wndregioninvalid = 0;

  RECT r;
  getNonClientRect(&r);

  Region *reg = getRegion();
  RegionI *_reg = NULL;

  if (!reg) {
    _reg = new RegionI;
    reg = _reg;
    if (isRectRgn())
      reg->setRect(&r);
  } else
    if (isRectRgn())
      reg->setRect(&r);

  Region *wr = reg->clone();

  if (!subtractorrgn) subtractorrgn = new RegionI();
  subtractorrgn->empty();
  if (!composedrgn) composedrgn = new RegionI;
  composedrgn->empty();

  RegionI *subme = NULL;
  RegionI *andme = NULL;
  RegionI *orme = NULL;

  // if subregion is now empty, we need to only use our region
  RECT gr;
  getNonClientRect(&gr);
  for (int i=0;i<virtualChildren.getNumItems();i++) {
    RootWnd *srw = virtualChildren.enumItem(i);
    if (!srw->isVisible(1) || srw->getPaintingAlpha() == 0) continue;
    if (srw->getRegionOp() != REGIONOP_NONE) {
      Region *sr = srw->getComposedRegion();
      if (sr) {
        Region *osr = sr->clone();
        RECT r;
        srw->getNonClientRect(&r);
        r.left -= gr.left;
        r.top -= gr.top;
        osr->offset(r.left, r.top);
/*        sr->debug();
        osr->debug();*/
        if (srw->getRegionOp() == REGIONOP_OR) {
          if (!orme) orme = new RegionI;
          orme->addRegion(osr);
        } else if (srw->getRegionOp() == REGIONOP_AND) {
          if (!andme) andme = new RegionI;
          andme->addRegion(osr);
        } else if (srw->getRegionOp() == REGIONOP_SUB) {
          if (!subme) subme = new RegionI;
          subme->addRegion(osr);
        } else if (srw->getRegionOp() == REGIONOP_SUB2) {
          if (!subme) subme = new RegionI;
          subtractorrgn->addRegion(osr);
        }
        sr->disposeClone(osr);
      }
    }
    Region *sr = srw->getSubtractorRegion();
    if (sr != NULL && !sr->isEmpty()) {
      Region *osr = sr->clone();
      RECT r;
      srw->getNonClientRect(&r);
      r.left -= gr.left;
      r.top -= gr.top;
      osr->offset(r.left, r.top);
      subtractorrgn->addRegion(osr);
      sr->disposeClone(osr);
    }
  }

  if (andme) {
    wr->andRegion(andme);
    delete andme;
  }
  if (orme) {
    wr->addRegion(orme);
    delete orme;
  }
  if (subme) {
    wr->subtractRgn(subme);
    delete subme;
  }

  composedrgn->addRegion(wr);
  reg->disposeClone(wr);
  delete _reg;
}

void BaseWnd::updateWindowRegion() {
  if (!isPostOnInit() || isVirtual()) return;
  if (getDesktopAlpha()) { // if desktopalpha is on, we can't use regions (thanks MS), we have to rely on the framebuffer correctness
//CUT    SetWindowRgn(getOsWindowHandle(), NULL, FALSE);
    StdWnd::setWndRegion(getOsWindowHandle(), NULL);
    return;
  }
  Region *_r = getComposedRegion();
  RECT test;
  _r->getBox(&test);
  Region *_s = getSubtractorRegion();
  ASSERT(_r != NULL && _s != NULL);

  Region *z = _r->clone();
  z->subtractRgn(_s);
                      
  assignWindowRegion(z);
  _r->disposeClone(z);
}

// wr is NOT scaled!!!
void BaseWnd::assignWindowRegion(Region *wr) {
  ASSERT(wr != NULL);

  if (!isPostOnInit()) return;

  int isrect = wr->isRect();

  RECT r;
  BaseWnd::getWindowRect(&r);
  r.right -= r.left;
  r.left = 0;
  r.bottom -= r.top;
  r.top = 0;

  RECT z;
  wr->getBox(&z);
  z.left = 0;
  z.top = 0;

  if (renderRatioActive()) {
    double i = getRenderRatio();
    wr->scale(i, i, FALSE);
  }

  RECT sz;
  wr->getBox(&sz);
  sz.right -= sz.left;
  sz.bottom -= sz.top;
  sz.left = 0;
  sz.top = 0;

  if (isrect && 
      ((z.right == rwidth && z.bottom == rheight) || 
       (sz.left == r.left && sz.right == r.right && sz.top == r.top && sz.bottom == r.bottom) ||
       (0)
      )
     )
//CUT    SetWindowRgn(getOsWindowHandle(), NULL, TRUE); 
    StdWnd::setWndRegion(getOsWindowHandle(), NULL, TRUE); 
  else {
    //DebugString("setting region, rwidth = %d, rheight = %d, z.right = %d, z.bottom = %d\n", rwidth, rheight, z.right, z.bottom);
//CUT    SetWindowRgn(getOsWindowHandle(), wr->makeWindowRegion(), TRUE);
    StdWnd::setWndRegion(getOsWindowHandle(), wr->makeWindowRegion(), TRUE);
  }
}

void BaseWnd::performBatchProcesses() {
  // recompute the window region if needed and apply it to the HWND
  if (wndregioninvalid && !isVirtual()) 
    if (ensureWindowRegionValid())
      updateWindowRegion();
  if (need_flush_cascaderepaint) {
    _cascadeRepaintRgn(deferedCascadeRepaintRgn);
    deferedCascadeRepaintRgn->empty();
    need_flush_cascaderepaint = 0;
  }
}

int BaseWnd::getRegionOp() {
  return regionop; 
}

void BaseWnd::setRegionOp(int op) {
  if (regionop != op) {
    regionop = op;
    invalidateWindowRegion();
  }
}

int BaseWnd::isRectRgn() {
  return rectrgn;
}

void BaseWnd::setRectRgn(int i) {
  rectrgn = i;
}

TimerClient *BaseWnd::timerclient_getMasterClient() {
  if (!isVirtual()) return this;
  RootWnd *w = getParent();
  if (w) {
    TimerClient *tc = w->getTimerClient();
    if (tc)
      return tc->timerclient_getMasterClient();
  }
  return NULL;
}

void BaseWnd::timerclient_onMasterClientMultiplex() {
  performBatchProcesses();
}

TimerClient *BaseWnd::getTimerClient() {
  return this;
}

Dependent *BaseWnd::rootwnd_getDependencyPtr() {
  return this;
}

Dependent *BaseWnd::timerclient_getDependencyPtr() {
  return this;
}

void BaseWnd::addMinMaxEnforcer(RootWnd *w) {
  minmaxEnforcers.addItem(w);
  signalMinMaxEnforcerChanged();  
}

void BaseWnd::removeMinMaxEnforcer(RootWnd *w) {
  minmaxEnforcers.removeItem(w);
  signalMinMaxEnforcerChanged();
}

void BaseWnd::signalMinMaxEnforcerChanged(void) {
  RootWnd *w = getDesktopParent();
  if (w == NULL || w == this) onMinMaxEnforcerChanged();
  else w->signalMinMaxEnforcerChanged();
}

int BaseWnd::getNumMinMaxEnforcers() {
  return minmaxEnforcers.getNumItems();
}

RootWnd *BaseWnd::enumMinMaxEnforcer(int n) {
  return minmaxEnforcers.enumItem(n);
}

int BaseWnd::onAction(const char *action, const char *param, int x, int y, int p1, int p2, void *data, int datalen, RootWnd *source) {
  return 1;
}

int BaseWnd::sendAction(RootWnd *target, const char *action, const char *param, int x, int y, int p1, int p2, void *data, int datalen) {
  ASSERT(target != NULL);
  return target->onAction(action, param, x, y, p1, p2, data, datalen, this);
}

int BaseWnd::virtualBeforePaint(Region *r) {
  if (!virtualCanvas) return 0;
  PaintCallbackInfoI pc(virtualCanvas, r);
  dependent_sendEvent(RootWnd::depend_getClassGuid(), Event_ONPAINT, PaintCallback::BEFOREPAINT, &pc);
  return 1;
}

int BaseWnd::virtualAfterPaint(Region *r) {
  if (!virtualCanvas) return 0;
  PaintCallbackInfoI pc(virtualCanvas, r);
  dependent_sendEvent(RootWnd::depend_getClassGuid(), Event_ONPAINT, PaintCallback::AFTERPAINT, &pc);
  return 1;
}

int BaseWnd::timerclient_onDeferredCallback(int p1, int p2) {
  TimerClientI::timerclient_onDeferredCallback(p1, p2);
  return onDeferredCallback(p1, p2);
}

void BaseWnd::timerclient_timerCallback(int id) {
  TimerClientI::timerclient_timerCallback(id);
  timerCallback(id);
}

int BaseWnd::setTimer(int id, int ms) {
/*  FILE *out = fopen("c:\\timerlogs.txt", "a+t");
  if (out) {
    fprintf(out, StringPrintf("setTimer for %08X(%08X) (%s)\n", this, getDependencyPtr(), getId()));
    fclose(out);
  }*/
  return timerclient_setTimer(id, ms);
}

int BaseWnd::killTimer(int id) {
  return timerclient_killTimer(id);
}

void BaseWnd::postDeferredCallback(int p1, int p2, int mindelay) {
/*  FILE *out = fopen("c:\\timerlogs.txt", "a+t");
  if (out) {
    fprintf(out, StringPrintf("postDeferredCallback for %08X(%08X) (%s)\n", this, getDependencyPtr(), getId()));
    fclose(out);
  }*/
  timerclient_postDeferredCallback(p1, p2, mindelay);
}

int BaseWnd::bypassModal() {
  return 0;
}

void BaseWnd::setRenderBaseTexture(int r) {
  renderbasetexture = r;
  if (isInited()) invalidate();
}

int BaseWnd::getRenderBaseTexture() {
  return renderbasetexture;
}

GuiObject *BaseWnd::getGuiObject() {
#ifdef WASABI_COMPILE_SKIN
  if (my_guiobject == NULL) {
    my_guiobject = static_cast<GuiObject *>(getInterface(guiObjectGuid));
  }
  return my_guiobject;
#else
  return NULL;
#endif
}

//CUT someday
int BaseWnd::getFlag(int flag) {
/*  switch (flag) {
  }*/
  return 0;
}

int BaseWnd::triggerEvent(int event, int p1, int p2) {//PORTME
  switch (event) {
    case TRIGGER_ONRESIZE:
      if (isPostOnInit())
        onResize();
      break;
    case TRIGGER_INVALIDATE:
      if (isPostOnInit())
        invalidate();
      break;
  }
  return 0;
}

int BaseWnd::windowEvent(int event, unsigned long p1, unsigned long p2, unsigned long p3, unsigned long p4) {
  switch (event) {
    case WndEvent::ONRESIZE:
      if (isPostOnInit())
        onResize();
    break;
    case WndEvent::INVALIDATE:
      if (isPostOnInit())
        invalidate();
    break;
  }
  return 0;
}

void BaseWnd::registerAcceleratorSection(const char *name, int global) {
#if defined(WASABI_COMPILE_LOCALES)
  WASABI_API_LOCALE->locales_registerAcceleratorSection(name, this, global);
#endif
}

int BaseWnd::onAcceleratorEvent(const char *name) {
  for (int i=0;i<getNumRootWndChildren();i++)
    if(enumRootWndChildren(i)->onAcceleratorEvent(name))
      return 1;
  return 0;
}

int BaseWnd::allowDeactivation() {
  return allow_deactivate
#ifdef WASABI_COMPILE_WND 
& ((WASABI_API_WND == NULL) || WASABI_API_WND->appdeactivation_isallowed(this))
#endif
;
}

void BaseWnd::onMinimize() {
  if (maximized) wasmaximized = 1;
  maximized = 0;
  minimized = 1;
  if (!isVirtual()) {
    dropVirtualCanvas();
  }
}

void BaseWnd::onMaximize() {
  wasmaximized = 0;
  minimized = 0;
  maximized = 1;
}

void BaseWnd::dropVirtualCanvas() {
  if (!buffered) return;
  deferedInvalidate();
  delete virtualCanvas;
  virtualCanvas = NULL;
}

void BaseWnd::onRestore() {
  if (minimized && wasmaximized) {
    wasmaximized = 0;
    maximized = 1;
    minimized = 0;
  } else {
    maximized = 0;
    minimized = 0;
  }
  if (buffered) {
    if (getDesktopParent() == this) {
      cascadeRepaint(TRUE);
    }
  }
}

RootWnd *BaseWnd::findWindow(const char *id) {
  RootWndFinder find_object;
  find_object.reset();
  find_object.setFindId(id);
  RootWnd *ret = findWindowChain(&find_object);
  /*if (ret == NULL) 
    DebugString("findWindow : window not found by id ! %s \n", id);*/
  return ret;
}

RootWnd *BaseWnd::findWindowByInterface(GUID interface_guid) {
  RootWndFinder find_object;
  find_object.reset();
  find_object.setFindGuid(interface_guid);
  RootWnd *ret = findWindowChain(&find_object);
/*  char str[256];
  nsGUID::toChar(interface_guid, str);
  if (ret == NULL) 
    DebugString("findWindow : object not found by guid ! %s \n", str);*/
  return ret;
}

RootWnd *BaseWnd::findWindowByCallback(FindObjectCallback *cb) {
  RootWnd *ret = findWindowChain(cb);
  /*if (ret == NULL) 
    DebugString("findWindow : object not found by callback!\n");*/
  return ret;
}

RootWnd *BaseWnd::findWindowChain(FindObjectCallback *cb, RootWnd *wcaller) {

  if (!cb) return NULL;

  if (cb->findobjectcb_matchObject(this)) return this;

  // first lets not look in subdirectories

  int i;

  for (i=0;i<getNumRootWndChildren();i++) {
    RootWnd *child = enumRootWndChildren(i);
    if (!child || child == wcaller) continue;
    if (cb->findobjectcb_matchObject(child))
      return child;
  }

  // ok so it wasn't in our content, lets try to find it as a grandchildren

  for (i=0;i<getNumRootWndChildren();i++) {
    RootWnd *child = enumRootWndChildren(i);
    if (child->getNumRootWndChildren() > 0) { 
      RootWnd *ret = child->findWindowChain(cb, this);
      if (ret) return ret;
    }
  }

  // so it wasnt one of our children, we'll hop the tree up one level and ask our parent to find it
  // for us. of course, our parents are smart, they won't ask us back when asking our sibblings
  RootWnd *p = getParent();
  if (p != NULL && wcaller != p) {
    return p->findWindowChain(cb, this);
  }

  return NULL;
}

RootWnd *BaseWnd::rootWndFromScreenPoint(const POINT *pt, int throughtest) {
  OSWINDOWHANDLE wnd = StdWnd::windowFromPoint(*pt);

  // check if it's one of ours
  if (StdWnd::getOwningProcess(wnd) != StdCPU::getCurrentProcessId()) {
    return NULL;
  }

  // check that classname is RootWndable
  char buf[80]="";
  GetClassName(wnd, buf, 80);
  buf[79] = '\0';
  if (!STREQL(buf, BASEWNDCLASSNAME))
    return NULL;

  // get RootWnd* from userdata
  RootWnd *rootwnd = reinterpret_cast<RootWnd*>(GetWindowLong(wnd, GWL_USERDATA));

  // and ask the RootWnd to find us the correct virtual child
  return rootwnd->rootWndFromPoint(const_cast<POINT *>(pt), throughtest);
}

const char *BaseWnd::timerclient_getName() {
  tcname = StringPrintf("name=\"%s\", id=\"%s\"", getRootWndName(), getId());
  return tcname;
}

void BaseWnd::setTabOrder(int a) {
  if (getParent() != NULL)
    getParent()->setVirtualTabOrder(this, a);
}

int BaseWnd::getTabOrder() {
  if (getParent() != NULL) 
    return getParent()->getVirtualTabOrder(this);
  return -1;
}

void BaseWnd::recursive_setVirtualTabOrder(RootWnd *w, float a, float lambda) {
  ASSERT(w != NULL);
  childtabs.setAutoSort(0);
  int i = getTabOrderEntry(a);
  if (i != -1) {
    TabOrderEntry *toe = childtabs.enumItem(i);
    if (toe->wnd != w) {
      lambda += TABORDER_K;
      if (lambda != 1.0)
        recursive_setVirtualTabOrder(toe->wnd, a + lambda, lambda);
    } else {
      return;
    }
  } 
  i = getTabOrderEntry(w);
  if (i != -1) {
    delete childtabs.enumItem(i);
    childtabs.removeByPos(i);
  }
  TabOrderEntry *toe = new TabOrderEntry;
  toe->wnd = w;
  toe->order = a;
  childtabs.addItem(toe);
}

void BaseWnd::setVirtualTabOrder(RootWnd *w, int a) {
  if (a == -1) {
    delTabOrderEntry(w);
    return;
  }
  recursive_setVirtualTabOrder(w, (float)a);
}

int BaseWnd::getVirtualTabOrder(RootWnd *w) {
  int a = getTabOrderEntry(w);
  if (a == -1) return -1;
  return (int)childtabs.enumItem(a);
}

int BaseWnd::getTabOrderEntry(RootWnd *w) {
  foreach(childtabs) 
    if (childtabs.getfor()->wnd == w)
      return foreach_index;
  endfor;
  return -1;
}

void BaseWnd::delTabOrderEntry(int i) {
  int a = getTabOrderEntry((float)i);
  if (a == -1) return;
  childtabs.removeByPos(a);
}

void BaseWnd::delTabOrderEntry(RootWnd *w) {
  int a = getTabOrderEntry(w);
  if (a == -1) return;
  delete childtabs.enumItem(a);
  childtabs.removeByPos(a);
}

int BaseWnd::getTabOrderEntry(float order) {
  foreach(childtabs)
    if (childtabs.getfor()->order == order) 
      return foreach_index;
  endfor;
  return -1;
}

void BaseWnd::setAutoTabOrder() {
  if (!getParent()) return;
  RootWnd *dp = getDesktopParent();
  if (!dp || dp == this) return;
  getParent()->setVirtualAutoTabOrder(this);
}

void BaseWnd::setVirtualAutoTabOrder(RootWnd *w) {
  delTabOrderEntry(w);
  float o=0;
  for (int i=0;i<childtabs.getNumItems();i++) {
    o = MAX(o, childtabs.enumItem(i)->order);
  }
  setVirtualTabOrder(w, ((int)o)+1);
}

void BaseWnd::focusNext() {
  RootWnd *dp = getDesktopParent();
  if (dp != this) {
    if (dp != NULL)
      dp->focusNext();
    return;
  }
  RootWnd *w = getTab(TAB_GETNEXT);
  if (w != NULL) w->setFocus(); 
}

void BaseWnd::focusPrevious() {
  RootWnd *dp = getDesktopParent();
  if (dp != this) {
    if (dp != NULL)
      getDesktopParent()->focusPrevious();
    return;
  }
  RootWnd *w = getTab(TAB_GETPREVIOUS);
  if (w != NULL) w->setFocus(); 
}

void BaseWnd::recursive_buildTabList(RootWnd *from, PtrList<RootWnd> *list) {
  for (int i=0;i<from->getNumTabs();i++) {
    RootWnd *r = from->enumTab(i);
    if (!r->excludeFromTabList() && r->isVisible() && r->getPaintingAlpha() > 0) {
      if (r->wantFocus())
        list->addItem(r);
      recursive_buildTabList(r, list);
    }
  }
}

RootWnd *BaseWnd::getTab(int what) {
  PtrList<RootWnd> listnow;

  recursive_buildTabList(this, &listnow);

  int p = listnow.searchItem(rootfocus);

  if (p == -1)
    for (int i = 0; i < listnow.getNumItems(); i++) {
      RootWnd *r = listnow.enumItem(i);
      if (r->gotFocus()) { 
        //DebugString("desync of rootfocus, fixing\n");
        p = i;
        assignRootFocus(r);
        break;
      }
    }

  if (what == TAB_GETNEXT && rootfocus != NULL) {

    p++;
    if (p >= listnow.getNumItems())
      p = 0;
    return listnow.enumItem(p);

  } else if (what == TAB_GETPREVIOUS && rootfocus != NULL) {

    p--;
    if (p < 0) p = listnow.getNumItems()-1;
    return listnow.enumItem(p);

  } else if (what == TAB_GETCURRENT) {

    return rootfocus;

  } else if (what == TAB_GETFIRST || (what == TAB_GETNEXT && rootfocus == NULL)) {

    return listnow.getFirst();

  } else if (what == TAB_GETLAST || (what == TAB_GETPREVIOUS && rootfocus == NULL)) {

    return listnow.getLast();

  }

  return NULL;
}

int BaseWnd::getNumTabs() {
  return childtabs.getNumItems();
}

RootWnd *BaseWnd::enumTab(int i) {
  childtabs.sort();
  return childtabs.enumItem(i)->wnd;
}

void BaseWnd::onSetRootFocus(RootWnd *w) {
  assignRootFocus(w);
  RootWnd *dp = getDesktopParent();
  if (dp && dp != this) dp->onSetRootFocus(w);
}

void BaseWnd::autoFocus(RootWnd *w) {
  if (w->getFocusOnClick() && w->wantFocus()) {
    w->setFocus();
    return;
  }
  RootWnd *g = w;
  while (1) {
    RootWnd *p = g->getParent();
    if (p == NULL) break;
    RootWnd *dp = p->getDesktopParent();
    if (dp && dp != p) {
      if (p->wantFocus() && p->getFocusOnClick()) {
        p->setFocus();
        return;
      }
      g = p;
    } else
      break;
  }
}

void BaseWnd::setNoLeftClicks(int no) {
  noleftclick = no;
}

void BaseWnd::setNoMiddleClicks(int no) {
  nomiddleclick = no;
}

void BaseWnd::setNoRightClicks(int no) {
  norightclick = no;
}

void BaseWnd::setNoDoubleClicks(int no) {
  nodoubleclick = no;
}

void BaseWnd::setNoMouseMoves(int no) {
  nomousemove = no;
}

void BaseWnd::setNoContextMenus(int no) {
  nocontextmnu = no;
}

void BaseWnd::setDefaultCursor(Cursor *c) {
  customdefaultcursor = c;
}

OSCURSORHANDLE BaseWnd::getCustomCursor(int x, int y) { 
  return customdefaultcursor ? customdefaultcursor->getOSHandle() : INVALIDOSCURSORHANDLE;
}

#ifdef WASABI_API_ACCESSIBILITY
Accessible *BaseWnd::createNewAccObj() {
  waServiceFactory *f = WASABI_API_SVC->service_enumService(WaSvc::ACCESSIBILITY, 0);
  if (f != NULL) {
    svc_accessibility *svc = castService<svc_accessibility>(f);
    if (svc != NULL) {
      Accessible *a = svc->createAccessibleObject(this);
      WASABI_API_SVC->service_release(svc);
      return a;
    }
  }
  return NULL;
}

Accessible *BaseWnd::getAccessibleObject(int createifnotexist) {
  if (!createifnotexist) return accessible;
  if (!accessible)
    accessible = createNewAccObj(); 
  else
    accessible->addRef();
  return accessible;
}

const char *BaseWnd::accessibility_getValue() { return NULL; }

const char *BaseWnd::accessibility_getDescription() { return getTip(); }

int BaseWnd::accessibility_getRole() { return 0; }

int BaseWnd::accessibility_getState() {
  int state = 0;
  if (!isVisible()) state |= STATE_SYSTEM_INVISIBLE;
  //if (isVirtual() && !wantFocus()) state |= STATE_SYSTEM_INVISIBLE;
  if (gotFocus()) state |= STATE_SYSTEM_FOCUSED;
  return state;
}

const char *BaseWnd::accessibility_getHelp() { return NULL; }

const char *BaseWnd::accessibility_getAccelerator() { return NULL; }

const char *BaseWnd::accessibility_getDefaultAction() { return NULL; }

void BaseWnd::accessibility_doDefaultAction() { }

int BaseWnd::accessibility_getNumItems() { return 0; }

const char *BaseWnd::accessibility_getName() {
  return getRootWndName();
}

const char *BaseWnd::accessibility_getItemName(int n) { return NULL; }

const char *BaseWnd::accessibility_getItemValue(int n) { return NULL; }

const char *BaseWnd::accessibility_getItemDescription(int n) { return NULL; }

int BaseWnd::accessibility_getItemRole(int n) { return 0; }

int BaseWnd::accessibility_getItemState(int n) { return 0; }

const char *BaseWnd::accessibility_getItemHelp(int n) { return NULL; }

const char *BaseWnd::accessibility_getItemAccelerator(int n) { return NULL; }

const char *BaseWnd::accessibility_getItemDefaultAction(int n) { return NULL; }

void BaseWnd::accessibility_doItemDefaultAction(int n) { }

void BaseWnd::accessibility_getItemRect(int n, RECT *r) {
  if (r != NULL) {
    r->left = 0;
    r->top = 0;
    r->right = 0;
    r->bottom = 0;
  }
}

void BaseWnd::detachAccessibleObject() {
  accessible = NULL;
}

int BaseWnd::accessibility_getItemXY(int x, int y) { return -1; }
#endif

void BaseWnd::activate() {
  StdWnd::setActiveWindow(getRootParent()->getOsWindowHandle());
}

void BaseWnd::setOSWndName(const char *name) {
  if (isVirtual()) return;
#ifdef COMPILE_WASABI_SKIN
  StdWnd::setWndName(getOsWindowHandle(), name ? WASABI_API_SKIN->varmgr_translate(name) : "");
#else
  StdWnd::setWndName(getOsWindowHandle(), name ? name : "");
#endif
}

const char *BaseWnd::getOSWndName() {
  if (isVirtual()) return NULL;
  char str[4096];
  StdWnd::getWndName(getOsWindowHandle(), str, 4095);
  str[4095] = '\0';
  osname = str;
  return osname;
}

#ifdef EXPERIMENTAL_INDEPENDENT_AOT
void BaseWnd::setAlwaysOnTop(int i) {
  // this function should not optimize itself
  if (getDesktopParent() == this) {
   if (i) {
//CUT      SetWindowPos(getOsWindowHandle(), HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOOWNERZORDER);
      StdWnd::setTopmost(getOsWindowHandle(), TRUE);
   } else {
      saveTopMosts(); 
//CUT      SetWindowPos(getOsWindowHandle(), HWND_NOTOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOOWNERZORDER);
      StdWnd::setTopmost(getOsWindowHandle(), FALSE);
      restoreTopMosts();
    }
    alwaysontop = i;
    return;
  }
  RootWnd *p = getParent();
  if (p != NULL)
    p->setAlwaysOnTop(i);
}

int BaseWnd::getAlwaysOnTop() {
  if (getDesktopParent() == this)
    return alwaysontop;
  RootWnd *p = getParent();
  if (!p) return 0;
  return p->getAlwaysOnTop();
}
#endif

#ifdef WASABI_API_ACCESSIBILITY
RootWnd *BaseWnd::accessibility_getStdObject(int objid) { return NULL; }

int BaseWnd::accessibility_isLeaf() { return 0; }
#endif

void BaseWnd::wndwatcher_onDeleteWindow(RootWnd *w) {
  if (w == rootfocus) {
    rootfocus = NULL;
  }
}

void BaseWnd::assignRootFocus(RootWnd *w) {
  rootfocuswatcher.watchWindow(w);
  rootfocus = w;
}

int BaseWnd::getNCRole() {
  return ncrole;
}

void BaseWnd::setNCRole(int _ncrole) {
  ncrole = _ncrole;
}

Canvas *BaseWnd::getFrameBuffer() {
  return virtualCanvas;
}

void BaseWnd::setForeignWnd(int i) {
  m_takenOver = i;
}

#ifdef WASABI_COMPILE_SCRIPT
ScriptObject *BaseWnd::getGuiScriptObject() {
  GuiObject *go = getGuiObject();
  if (!go) return NULL;
  return go->guiobject_getScriptObject();
}
#endif

int BaseWnd::bufferizeLockedUIMsg(int uMsg, int wParam, int lParam) {
#ifdef WASABI_COMPILE_SKIN
  if (WASABI_API_SKIN && !WASABI_API_SKIN->skin_getLockUI()) return 0;
  if (!uiwaslocked) {
    uiwaslocked = 1;
    setTimer(BUFFEREDMSG_TIMER_ID, 20);
  }
  bufferedMsgStruct msg;
  msg.msg = uMsg;
  msg.wparam = wParam;
  msg.lparam = lParam;
  bufferedmsgs.addItem(msg);
  return 1;
#else
  return 0;
#endif
}

void BaseWnd::checkLockedUI() {//PORTME :(
#ifdef WASABI_COMPILE_SKIN
  if (WASABI_API_SKIN && !WASABI_API_SKIN->skin_getLockUI()) {
    uiwaslocked = 0;
    killTimer(BUFFEREDMSG_TIMER_ID);
    while (bufferedmsgs.getNumItems()>0) {
      bufferedMsgStruct msg = bufferedmsgs.enumItem(0);
      bufferedmsgs.delByPos(0);
      SendMessage(gethWnd(), msg.msg, msg.wparam, msg.lparam);
    }
    uiwaslocked = 0;
    killTimer(BUFFEREDMSG_TIMER_ID);
  }
#endif
}

int BaseWnd::isMinimized() {
  RootWnd *w = getDesktopParent();
  if (w == this || w == NULL) return minimized;
  return w->isMinimized();
}

int BaseWnd::isMaximized() {
  RootWnd *w = getDesktopParent();
  if (w == this || w == NULL) return maximized;
  return w->isMaximized();
}

void BaseWnd::setNonBuffered(int nb) {
  buffered = !nb;
}

void BaseWnd::onBeforePaintTree(Canvas *dest, Region *r) {
  dest->fillRegion(r, RGB(0,0,0));
}

int BaseWnd::callDefProc(OSWINDOWHANDLE wnd, UINT msg, int wparam, int lparam) {
#ifdef WIN32
  if (m_oldWndProc) return CallWindowProc((LRESULT (__stdcall *)(HWND, UINT, WPARAM, LPARAM))m_oldWndProc, wnd, msg, wparam, lparam);
  return DefWindowProc(wnd, msg, wparam, lparam);
#endif
  return 0;
}

void BaseWnd::updateCursor() {
  int ct = BASEWND_CURSOR_POINTER;
  int _x, _y;
  StdWnd::getMousePos(&_x, &_y);
  screenToClient(&_x, &_y);
  OSCURSORHANDLE c = INVALIDOSCURSORHANDLE;
  if (!handleVirtualChildMsg(WM_SETCURSOR,_x,_y, &ct, &c)) {
    ct = getCursorType(_x, _y);
  }
#if defined(WIN32)
  char *wincursor = NULL;
#elif defined(XWINDOW)
  int wincursor = 0;
#else
#error port me!
#endif // platform
  switch (ct) {
    case BASEWND_CURSOR_USERSET: 
#ifdef WIN32
    {
      if (c == NULL) 
        c = getCustomCursor(_x, _y);
      if (c != NULL) {
        SetCursor(c);
        return;
      }
    }
#endif
    break;
    default:
    case BASEWND_CURSOR_POINTER:
#if defined(WIN32)
      wincursor = IDC_ARROW;
#elif defined(XWINDOW)
      wincursor = None; // go to default
#endif //platform
    break;
    case BASEWND_CURSOR_NORTHSOUTH:
#if defined(WIN32)
      wincursor = IDC_SIZENS;
#elif defined(XWINDOW)
      wincursor = XC_sb_v_double_arrow;
#endif //platform
    break;
    case BASEWND_CURSOR_EASTWEST:
#if defined(WIN32)
      wincursor = IDC_SIZEWE;
#elif defined(XWINDOW)
      wincursor = XC_sb_h_double_arrow;
#endif //platform
    break;
#if defined(WIN32) //back-compat only.
    case BASEWND_CURSOR_NORTHWEST_SOUTHEAST:
      wincursor = IDC_SIZENWSE;
    break;
    case BASEWND_CURSOR_NORTHEAST_SOUTHWEST:
      wincursor = IDC_SIZENESW;
    break;
#endif //platform
    case BASEWND_CURSOR_NORTHWEST:
#if defined(WIN32)
      wincursor = IDC_SIZENWSE;
#elif defined(XWINDOW)
      wincursor = XC_top_left_corner;
#endif //platform
    break;
    case BASEWND_CURSOR_SOUTHEAST:
#if defined(WIN32)
      wincursor = IDC_SIZENWSE;
#elif defined(XWINDOW)
      wincursor = XC_bottom_right_corner;
#endif //platform
    break;
    case BASEWND_CURSOR_NORTHEAST:
#if defined(WIN32)
      wincursor = IDC_SIZENESW;
#elif defined(XWINDOW)
      wincursor = XC_top_right_corner;
#endif //platform
    break;
    case BASEWND_CURSOR_SOUTHWEST:
#if defined(WIN32)
      wincursor = IDC_SIZENESW;
#elif defined(XWINDOW)
      wincursor = XC_bottom_left_corner;
#endif //platform
    break;
    case BASEWND_CURSOR_4WAY:
#if defined(WIN32)
      wincursor = IDC_SIZEALL;
#elif defined(XWINDOW)
      wincursor = XC_fleur;
#endif //platform
    break;
    case BASEWND_CURSOR_EDIT:
#if defined(WIN32)
      wincursor = IDC_IBEAM;
#elif defined(XWINDOW)
      wincursor = XC_xterm;
#endif //platform
    break;
  }
#if defined(WIN32)
  if (wincursor != NULL) SetCursor(LoadCursor(NULL, wincursor));
#elif defined(XWINDOW)
  XWindow::setCursor( hwnd, wincursor );
#endif //platform
}

void BaseWnd::setMargins(int left, int top, int right, int bottom) {
  margin[0] = left;
  margin[1] = top;
  margin[2] = right;
  margin[3] = bottom;
}

// most of the time you would not need to call this, since clientRect already applies the margins,
// however, for things like getPreferences(SUGGESTED_W/H), you might.
void BaseWnd::getMargins(int *left, int *top, int *right, int *bottom) {
  if (left) *left = margin[0];
  if (top) *top = margin[1];
  if (right) *right = margin[2];
  if (bottom) *bottom = margin[3];
}
