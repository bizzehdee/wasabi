#include "precomp.h"

#include "virtualwnd.h"

#include "accessible.h"
#include "region.h"
//CUT #include "../studio/api.h"
#include "usermsg.h"
#include <bfc/std_wnd.h>

VirtualWnd::VirtualWnd() {
  virtualX = virtualY = virtualH = virtualW = 0;
  bypassvirtual = 0;
  focus = 0;
  resizecount = 0;
  lastratio = 1;
}

VirtualWnd::~VirtualWnd() {
  if (!bypassvirtual) {
    if (focus && getParent())
      getParent()->setVirtualChildFocus(NULL); // propagates down to rootparent
    // get root parent without calling virtual on this
    RootWnd *w = getParent();
    if (w) {
      w = w->getRootParent();
      if (w && w->getVirtualChildCapture() == this)
        w->setVirtualChildCapture(NULL);
    }
  }
}

int VirtualWnd::init(RootWnd *parWnd, int nochild) {
  if (!bypassvirtual) 
    setParent(parWnd);
  return(VIRTUALWND_PARENT::init(parWnd,nochild));
}

int VirtualWnd::init(OSMODULEHANDLE moduleHandle, OSWINDOWHANDLE parent, int nochild) {
  if (!bypassvirtual) {
    ASSERTPR(getParent()!=NULL, "Virtual window created without specifying BaseWnd parent");
    if (getStartHidden())
      this_visible = 0;
    else {
      this_visible = 1;
    }

    onInit();

    onPostOnInit();

    if (isVisible()) onSetVisible(1);

    return 1;
  } else
    return VIRTUALWND_PARENT::init(moduleHandle, parent, nochild);
}

OSWINDOWHANDLE VirtualWnd::getOsWindowHandle() {
//	ASSERTPR(getParent() != NULL, "Virtual window used as base parent !");
  if (!bypassvirtual) {
    if (!getParent()) return INVALIDOSWINDOWHANDLE;
    return getParent()->getOsWindowHandle();
  } else {
   return VIRTUALWND_PARENT::getOsWindowHandle();
  }
}

OSMODULEHANDLE VirtualWnd::getOsModuleHandle() {
  if (!bypassvirtual) {
    if(!getParent()) return INVALIDOSMODULEHANDLE;
    return getParent()->getOsModuleHandle();
  } else
    return VIRTUALWND_PARENT::getOsModuleHandle();
}

void VirtualWnd::resize(RECT *r, int wantcb) {
  if (!bypassvirtual) {
    resize(r->left, r->top, r->right-r->left, r->bottom-r->top, wantcb);
  } else
   VIRTUALWND_PARENT::resize(r, wantcb);
}

// fg> the resizecount > 1 is a hack, it should be 0 but i need more time to fix this thing, at least this way we don't lose the optim
void VirtualWnd::resize(int x, int y, int w, int h, int wantcb) {
  if (!bypassvirtual) {

    if (x == NOCHANGE) x = virtualX;
    if (y == NOCHANGE) y = virtualY;
    if (w == NOCHANGE) w = virtualW;
    if (h == NOCHANGE) h = virtualH;

    double thisratio = getRenderRatio();
    if (resizecount > 1 && virtualX == x && virtualY == y && virtualW == w && virtualH == h && lastratio == thisratio) return;
    lastratio = thisratio;

    if (isVisible()) {
      RECT r;
      getNonClientRect(&r);
      invalidateRect(&r);
    }

    virtualX=x; virtualY=y;
    virtualW=w; virtualH=h;

    if (isVisible()) {
      RECT r;
      getNonClientRect(&r);
      invalidateRect(&r);
    }

    setRSize(x, y, w, h);

    if (wantcb && isPostOnInit()) {
      resizecount = MIN(resizecount+1, 2); 
      onResize();
    }
  } else
   VIRTUALWND_PARENT::resize(x, y, w, h, wantcb);
}

//CUTvoid VirtualWnd::resize(RECT *r) {
//CUT  resize(r->left, r->top, r->right-r->left, r->bottom-r->top);
//CUT}

void VirtualWnd::move(int x, int y) {
  if (!bypassvirtual) {
    if (isVisible()) {
      RECT r;
      getNonClientRect(&r);
      invalidateRect(&r);
    }

    virtualX=x; virtualY=y;

    if (isVisible()) {
      RECT r;
      getNonClientRect(&r);
      invalidateRect(&r);
    }
  } else
   VIRTUALWND_PARENT::move(x, y);
}

void VirtualWnd::invalidate() {
  if (!bypassvirtual) {
    if (!getRootParent()) return;
    RECT r(clientRect());
    getRootParent()->invalidateRectFrom(&r, this);
//	VIRTUALWND_PARENT::invalidate();
  } else
   VIRTUALWND_PARENT::invalidate();
}

void VirtualWnd::invalidateRect(RECT *r) {
  if (!bypassvirtual) {
    if(!getRootParent()) return;
      getRootParent()->invalidateRectFrom(r, this);
  } else
   VIRTUALWND_PARENT::invalidateRect(r);
}

void VirtualWnd::invalidateRgn(Region *reg) {
  if (!bypassvirtual) {
    if(!getRootParent()) return;
    getRootParent()->invalidateRgnFrom(reg, this);
  } else
   VIRTUALWND_PARENT::invalidateRgn(reg);
}

void VirtualWnd::validate() {
  if (!bypassvirtual) {
    if(!getRootParent()) return;
    RECT r;
    getClientRect(&r);
    getRootParent()->validateRect(&r);
  } else
   VIRTUALWND_PARENT::validate();
}

void VirtualWnd::validateRect(RECT *r) {
  if (!bypassvirtual) {
    if(!getRootParent()) return;
    getRootParent()->validateRect(r);
  } else
   VIRTUALWND_PARENT::validateRect(r);
}

void VirtualWnd::validateRgn(Region *reg) {
  if (!bypassvirtual) {
    if(!getRootParent()) return;
    getRootParent()->validateRgn(reg);
  } else
   VIRTUALWND_PARENT::validateRgn(reg);
}

void VirtualWnd::getClientRect(RECT *rect) {
  if (!bypassvirtual) {
    getNonClientRect(rect);
    rect->left += margin[0];
    rect->top += margin[1];
    rect->right -= margin[2];
    rect->bottom -= margin[3];
  } else {
   VIRTUALWND_PARENT::getClientRect(rect);
  }
}

void VirtualWnd::getNonClientRect(RECT *rect) {
  if (!bypassvirtual) {
    rect->left=virtualX; rect->right=virtualX+virtualW;
    rect->top=virtualY; rect->bottom=virtualY+virtualH;
  } else
   VIRTUALWND_PARENT::getNonClientRect(rect);
}

void VirtualWnd::getWindowRect(RECT *rect) {
  if (!bypassvirtual) {
    RECT a;
    getRootParent()->getWindowRect(&a);
    int x = virtualX, y = virtualY, w = virtualW, h = virtualH;
    if (renderRatioActive()) {
      multRatio(&x, &y);
      multRatio(&w, &h);
    }
    
    RECT pcr;
    StdWnd::getClientRect(getParent()->gethWnd(), &pcr); // don't go thru rootwnd->getClientRect!
    getRootParent()->clientToScreen((int *)&pcr.left, (int *)&pcr.top);
    RECT pwr;
    getRootParent()->getWindowRect(&pwr);
    x += pcr.left-pwr.left;
    y += pcr.top-pwr.top;

    rect->left=a.left+x; rect->right=rect->left+w;
    rect->top=a.top+y; rect->bottom=rect->top+h;
  } else
   VIRTUALWND_PARENT::getWindowRect(rect);
}

int VirtualWnd::beginCapture() {
  if (!allowCapture) return 0;
  if (!bypassvirtual) {
    disable_tooltip_til_recapture = 0;
    getRootParent()->setVirtualChildCapture(this);
    return 1;
  } else
   return VIRTUALWND_PARENT::beginCapture();
}

int VirtualWnd::endCapture() {
  if (!allowCapture) return 0;
  if (!bypassvirtual) {
    if (getRootParent() == NULL) return 0;
    getRootParent()->setVirtualChildCapture(NULL);
    return 1;
  } else
   return VIRTUALWND_PARENT::endCapture();
}

int VirtualWnd::getCapture() {
  if (!bypassvirtual) {
    if (getRootParent() == NULL) return 0;
    return getRootParent()->getVirtualChildCapture() == this;
  } else
   return VIRTUALWND_PARENT::getCapture();
}

void VirtualWnd::setVirtualChildCapture(BaseWnd *child) {
  if (!bypassvirtual) {
    getParent()->setVirtualChildCapture(child);
  } else
   VIRTUALWND_PARENT::setVirtualChildCapture(child);
}

// eek
void VirtualWnd::repaint() {
  if (!bypassvirtual) {
    if(!getParent()) return;
    getParent()->repaint(); 
  } else
   VIRTUALWND_PARENT::repaint();
}

/*int VirtualWnd::focusNextSibbling(int dochild) {
  return 1;
}

int VirtualWnd::focusNextVirtualChild(BaseWnd *child) {
  return 1;
}*/

int VirtualWnd::cascadeRepaint(int pack) {
  if (!bypassvirtual) {
    if (getRootParent()) {
      RECT r;
      VirtualWnd::getNonClientRect(&r);
      getRootParent()->cascadeRepaintRectFrom(&r, this, pack);
    }
    return 1;
  } else
   return VIRTUALWND_PARENT::cascadeRepaint(pack);
}

int VirtualWnd::cascadeRepaintRect(RECT *r, int pack) {
  if (!bypassvirtual) {
    if (getRootParent()) {
      getRootParent()->cascadeRepaintRectFrom(r, this, pack);
    }
    return 1;
  } else
   return VIRTUALWND_PARENT::cascadeRepaintRect(r, pack);
}

int VirtualWnd::cascadeRepaintRgn(Region *r, int pack) {
  if (!bypassvirtual) {
    if (getRootParent()) {
      getRootParent()->cascadeRepaintRgnFrom(r, this, pack);
    }
    return 1;
  } else
   return VIRTUALWND_PARENT::cascadeRepaintRgn(r, pack);
}

/*RootWnd *VirtualWnd::getWindowBehindMyself(int x, int y) {
  RECT r;
  if (!bypassvirtual) {
    if (!getParent()) return NULL;
    int n = getParent()->getNumVirtuals();

    RootWnd *c = NULL;

    for (int i=n-1;i>=0;i++) {
      c = getParent()->getVirtualChild(i);
      if (c == this) break;
    }

    i--;
    if (i < 0) return getParent();

    for (;i>=0; i--) {
      c = getParent()->getVirtualChild(i);
      c->getNonClientRect(&r);
      if (x>=r.left&&x<=r.right&&y>=r.top&&y<=r.bottom)
        return c;
    }
    return getParent();
  } else
   return NULL;
}*/

RootWnd *VirtualWnd::rootWndFromPoint(POINT *pt) {
  if (!bypassvirtual) {
    if (!getParent()) return NULL;
    return getParent()->rootWndFromPoint(pt);
  } else
   return VIRTUALWND_PARENT::rootWndFromPoint(pt);
}

double VirtualWnd::getRenderRatio() {
  if (!bypassvirtual) {
    if (!getParent()) return 1.0;
    return getParent()->getRenderRatio();
  } else
   return VIRTUALWND_PARENT::getRenderRatio();
}

void VirtualWnd::bringToFront() {
  if (!bypassvirtual) {
    if(!getParent()) return;
    //getParent()->bringVirtualToFront(this); TODO: FIX!!!
  } else
   VIRTUALWND_PARENT::bringToFront();
}

void VirtualWnd::bringToBack() {
  if (!bypassvirtual) {
    if(!getParent()) return;
    //getParent()->bringVirtualToBack(this); TODO: FIX!!!
  } else
   VIRTUALWND_PARENT::bringToBack();
}

void VirtualWnd::bringAbove(BaseWnd *o) {
  if (!bypassvirtual) {
    if(!getParent()) return;
    getParent()->bringVirtualAbove(this, o);
  }/* else
   VIRTUALWND_PARENT::bringAbove();*/
}

void VirtualWnd::bringBelow(BaseWnd *o) {
  if (!bypassvirtual) {
    if(!getParent()) return;
    getParent()->bringVirtualBelow(this, o);
  }/* else
   VIRTUALWND_PARENT::bringBelow();*/
}

int VirtualWnd::reparent(RootWnd *newparent) {
  if (!bypassvirtual) {
    if (getParent()) 
      getParent()->unregisterRootWndChild(this);
    parentWnd = NULL;
    newparent->registerRootWndChild(this);
    onSetParent(newparent);
    newparent->invalidate();
    return 1;
  } else {
   return VIRTUALWND_PARENT::reparent(newparent);
  }
}

int VirtualWnd::setVirtual(int i) {

//  ASSERT(!isInited()); // cut

  if (isInited()) return 0; 
  bypassvirtual = !i;
  return 1;
}

RootWnd *VirtualWnd::getRootParent() {
  if (!bypassvirtual) {
    if (!getParent()) return NULL;
    RootWnd *t = this;
    while (t->isVirtual()) {
      if (!t->getParent()) return NULL;
      t = t->getParent();
    }
    return t;
  } else {
    return VIRTUALWND_PARENT::getRootParent();
  }
}

int VirtualWnd::gotFocus() {
  if (!bypassvirtual) 
    return focus;
  else 
    return VIRTUALWND_PARENT::gotFocus();
}

int VirtualWnd::onGetFocus() {
  if (!bypassvirtual) {
    focus = 1;
    getRootParent()->onSetRootFocus(this);
    invalidate();
#ifdef WASABI_API_ACCESSIBILITY
    Accessible *a = getAccessibleObject();
    if (a != NULL) 
      a->onGetFocus();
#endif
  } else 
    return VIRTUALWND_PARENT::onGetFocus();
  return 1;
}

int VirtualWnd::onKillFocus() {
  if (!bypassvirtual) {
    focus = 0;
    invalidate();
  } else 
    return VIRTUALWND_PARENT::onKillFocus();
  return 1;
}

void VirtualWnd::setFocus() {
  RootWnd *f = this;
  if (!f->wantFocus() && f->getParent()) {
    while (f) {
      RootWnd *rp = f->getRootParent();
      if (rp == f) rp = f->getParent();
      f = rp;
      if (f && (!f->getParent() || f->wantFocus()
#ifdef WASABI_COMPILE_WND
          || f == WASABI_API_WND->main_getRootWnd()
#endif
        )) {
        f->setFocus();
        break;
      }
    }
  } else {
    if (!bypassvirtual) {
      if (getParent()) {
        getParent()->setVirtualChildFocus(this);
      }
    } else 
      VIRTUALWND_PARENT::setFocus();
  }
}

void VirtualWnd::setVirtualChildFocus(RootWnd *child) {
  if (!bypassvirtual) {
    getParent()->setVirtualChildFocus(child);
  } else
   VIRTUALWND_PARENT::setVirtualChildFocus(child);
}

int VirtualWnd::onActivate() {
  if (bypassvirtual) 
    return VIRTUALWND_PARENT::onActivate();
  return 1;
}

int VirtualWnd::onDeactivate() {
  if (bypassvirtual) 
    return VIRTUALWND_PARENT::onDeactivate();
  return 1;
}

void VirtualWnd::setAllowDeactivation(int allow) {
  RootWnd *w = getDesktopParent();
  if (w != NULL && w != this) 
    w->setAllowDeactivation(allow);
  else VIRTUALWND_PARENT::setAllowDeactivation(allow);
}

int VirtualWnd::allowDeactivation() {
  RootWnd *w = getDesktopParent();
  if (w != NULL && w != this) 
    return w->allowDeactivation();
  return VIRTUALWND_PARENT::allowDeactivation();
}

int VirtualWnd::onMouseMove(int x, int y) {
  if (bypassvirtual) 
    return VIRTUALWND_PARENT::onMouseMove(x, y);
  if (dragging) handleDragOver(x, y);
  return 1;
}

int VirtualWnd::onLeftButtonUp(int x, int y) {
  if (bypassvirtual) 
    return VIRTUALWND_PARENT::onLeftButtonUp(x, y);
  if (dragging) handleDragDrop(x, y);
  return 1;
}

int VirtualWnd::onMiddleButtonUp(int x, int y) {
  if (bypassvirtual) 
    return VIRTUALWND_PARENT::onMiddleButtonUp(x, y);
  if (dragging) handleDragDrop(x, y);
  return 1;
}



/* todo:  setCursor 
          
   + real childs going invisible should deferedInvalidate their rect on their parent window if it has a virtualCanvas
*/          
          


// No need for screenToClient/clientToScreen overrides since the virtual's origin is the same as it's parent
