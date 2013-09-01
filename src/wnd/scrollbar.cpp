#include "precomp.h"

#include <bfc/std.h>
#include <bfc/std_wnd.h>
#include <draw/argb32.h>
#include <wnd/canvas.h>

#include <wnd/notifmsg.h>

#include "scrollbar.h"

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x20A
#endif

#define TIMER_ID  9871
#define TIMER_ID2 9872

#define FIRST_DELAY 350
#define NEXT_DELAY  75

ScrollBar::ScrollBar() {
  leftrgn = NULL;
  rightrgn = NULL;
  buttonrgn = NULL;

  position = 0;
  moving = 0;
  lefting = 0;
  righting = 0;
  clicked = 0;
  height = DEFAULT_HEIGHT;
  buttonx = 0;

  shiftleft = 0;
  shiftright = 0;

  curmouseposition = POS_NONE;
  clickmouseposition = POS_NONE;
  pageing = 0;
  timer = 0;
  npages = 100;
  pageway = PAGE_NONE;
  updown = 256;
  insetpos = 0;

  vertical = 0;
setVirtual(FALSE);//BU> shouldn't be needed but conceals some clickthru bugs
}

ScrollBar::~ScrollBar() {
  deleteResources();
}

void ScrollBar::deleteResources() {
  delete leftrgn; leftrgn = NULL;
  delete buttonrgn; buttonrgn = NULL;
  delete rightrgn; rightrgn = NULL;
}

// this one is inherited
void ScrollBar::freeResources() {
  SCROLLBAR_PARENT::freeResources();
  deleteResources();
}

void ScrollBar::reloadResources() {
  SCROLLBAR_PARENT::reloadResources();
  loadBmps();
}

LRESULT ScrollBar::wndProc(OSWINDOWHANDLE hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_MOUSEWHEEL: {
      // redirect mousewheeling to parent wnd
      if (getParent()) return getParent()->wndProc(hWnd, uMsg, wParam, lParam);
    }
    default:
      return SCROLLBAR_PARENT::wndProc(hWnd, uMsg, wParam, lParam);
  }
}


int ScrollBar::onMouseMove (int x, int y) {

  SCROLLBAR_PARENT::onMouseMove(x, y);
  lastx = x;
  lasty = y;

  if (clicked && clickmouseposition == POS_BUTTON) {

    POINT pt={x,y};
    int x;
    if (!vertical) 
      x = pt.x - clickpos.x;
    else
      x = pt.y - clickpos.y;

    RECT r;
    getClientRect(&r);
    int maxwidth;
    if (!vertical)
      maxwidth = (r.right-r.left)-(shiftright+shiftleft+bmpbutton.getWidth())+1;
    else
      maxwidth = (r.bottom-r.top)-(shiftright+shiftleft+bmpbutton.getHeight())+1;
    buttonx = MIN(MAX(clickbuttonx + x, 0), maxwidth);
    calcPosition();
    invalidate();

  } else {

    int oldposition = curmouseposition;
    curmouseposition = getMousePosition();
    if (oldposition != curmouseposition) invalidate();

    if (curmouseposition != POS_NONE && !getCapture())
      beginCapture();

    if (curmouseposition == POS_NONE && getCapture() && !clicked && !pageing)
      endCapture();
  }


  return 1;
}

int ScrollBar::getWidth() {
  if (bmpbutton.isInvalid()) return 0;//FUCKO: what to return here
  if (!vertical)
    return bmpbutton.getHeight();
  else
    return bmpbutton.getWidth();
  return 0;
}

int ScrollBar::getMousePosition() {
  int v = POS_NONE;

//return v; // BU dirty hack to keep from crashing until we get bitaps going again

  POINT pt={lastx, lasty};

  RECT c;
  getClientRect(&c);
  pt.x -= c.left;
  pt.y -= c.top;

  Region *l, *b, *r;
  ASSERT(leftrgn != NULL);
  l = leftrgn->clone();
  ASSERT(buttonrgn != NULL);
  b = buttonrgn->clone();
  if (!vertical)
    b->offset(buttonx+shiftleft, 0);
  else
    b->offset(0, buttonx+shiftleft);
  ASSERT(rightrgn != NULL);
  r = rightrgn->clone();
  if (!vertical)
    r->offset(c.right-c.left-bmpleft.getWidth(), 0);
  else
    r->offset(0, c.bottom-c.top-bmpleft.getHeight());

  if (b->ptInRegion(&pt))
    v = POS_BUTTON;
  if (l->ptInRegion(&pt))
    v = POS_LEFT;
  if (r->ptInRegion(&pt))
    v = POS_RIGHT;

  leftrgn->disposeClone(l);
  buttonrgn->disposeClone(b);
  rightrgn->disposeClone(r);

  return v;
}

int ScrollBar::onLeftButtonDown(int x, int y) {
  clickmouseposition = getMousePosition();
  if (!pageing && clickmouseposition != POS_NONE) {
    clicked = 1;
    if (clickmouseposition == POS_LEFT || clickmouseposition == POS_RIGHT)
      handleUpDown();
    if (clickmouseposition) {
      clickpos.x = lastx;
      clickpos.y = lasty;
      clickbuttonx = buttonx;
    }
  } else {
    clicked = 0;
    pageing = 1;
    handlePageUpDown();
  }
  invalidate();
  return 1;
}

void ScrollBar::handleUpDown() {
  setTimer(TIMER_ID2, FIRST_DELAY);
  timer2 = 1;
  firstdelay = 1;

  checkUpDown();
}

int ScrollBar::checkUpDown() {
  if (!clicked) {
    if (timer2) {
      killTimer(TIMER_ID2);
      timer2 = 0;
      return 1;
    }
  }

  if (getMousePosition() == clickmouseposition)
    upDown(clickmouseposition);

  return 1;

}

void ScrollBar::handlePageUpDown() {

  setTimer(TIMER_ID, FIRST_DELAY);
  timer = 1;
  firstdelay = 1;

  checkPageUpDown();
}

int ScrollBar::checkPageUpDown() {

  if (!pageing) {
    if (timer) {
      killTimer(TIMER_ID);
      timer = 0;
      pageway = PAGE_NONE;
      return 1;
    }
  }

  POINT pt={lastx,lasty};
  RECT c;
  getClientRect(&c);
  pt.x -= c.left;
  pt.y -= c.top;
  
  if (!vertical) {
    int middlebutton = shiftleft + buttonx + bmpbutton.getWidth()/2;
    Region *r = buttonrgn->clone();
    r->offset(buttonx+shiftleft, 0);
    if (pt.x > middlebutton && !r->ptInRegion(&pt) && pageway != PAGE_DOWN)
      pageUp();
    if (pt.x < middlebutton && !r->ptInRegion(&pt) && pageway != PAGE_UP)
      pageDown();
    buttonrgn->disposeClone(r);
  } else {
    int middlebutton = shiftleft + buttonx + bmpbutton.getHeight()/2;
    Region *r = buttonrgn->clone();
    r->offset(0, buttonx+shiftleft);
    if (pt.y > middlebutton && !r->ptInRegion(&pt) && pageway != PAGE_DOWN)
      pageUp();
    if (pt.y < middlebutton && !r->ptInRegion(&pt) && pageway != PAGE_UP)
      pageDown();
    buttonrgn->disposeClone(r);
  }
  return 1;

}

int ScrollBar::onLeftButtonUp(int x, int y) {
  clicked = 0;
  clickmouseposition = POS_NONE;
  curmouseposition = POS_NONE;
  onMouseMove(x,y);
  if (pageing) {
    pageing = 0;
    checkPageUpDown();
  }
  onSetFinalPosition();
  invalidate();
  return 1;
}

int ScrollBar::onRightButtonDown(int x, int y) {
  return 1;
}

int ScrollBar::onRightButtonUp(int x, int y) {
  return 1;
}

int ScrollBar::onMouseWheelUp(int clicked, int lines) {
  return 1;
}

int ScrollBar::onMouseWheelDown(int clicked, int lines) {
  return 1;
}

int ScrollBar::onPaint(Canvas *canvas) {
  Bitmap &thisleft = curmouseposition == POS_LEFT ? (clicked ? bmplpressed : bmplhilite) : bmpleft;
  Bitmap &thisbutton = curmouseposition == POS_BUTTON ? (clicked ? bmpbpressed : bmpbhilite) : bmpbutton;
  Bitmap &thisright = curmouseposition == POS_RIGHT ? (clicked ? bmprpressed : bmprhilite) : bmpright;

  if (curmouseposition != clickmouseposition && clicked) {
    thisleft = bmpleft;
    thisbutton = bmpbutton;
    thisright = bmpright;
  }

  RECT r;
  PaintBltCanvas paintcanvas;

  if (canvas == NULL) {
    if (!paintcanvas.beginPaint(this)) return 0;
    canvas = &paintcanvas;
  }
  SCROLLBAR_PARENT::onPaint(canvas);

  getClientRect(&r);

  renderBaseTexture(canvas, r);

  if (!vertical) {
    RECT c;

    c.left = r.left;
    c.right = r.left;
    c.top = r.top;
    c.bottom = r.bottom;
    if (bmpbackgroundleft.valid()) {
      c.right = c.left + bmpbackgroundleft.getWidth();
      bmpbackgroundleft.stretchToRectAlpha(canvas, &c);
    }
    int l = c.right;
    c.left = r.right;
    c.right = r.right;
    if (bmpbackgroundright.valid()) {
      c.left = r.right - bmpbackgroundright.getWidth();
      bmpbackgroundright.stretchToRectAlpha(canvas, &c);
    }
    c.right = c.left;
    c.left = l;
    if (bmpbackgroundmiddle.valid()) {
      bmpbackgroundmiddle.stretchToRectAlpha(canvas, &c);
    }

    c.left = r.left + buttonx+shiftleft;
    c.top = r.top + 0;
    c.right = r.left + buttonx+thisbutton.getWidth()+shiftleft;
    c.bottom = r.top + getWidth();

    thisbutton.stretchToRectAlpha(canvas, &c);

    c.left = r.left;
    c.top = r.top;
    c.right = r.left + thisleft.getWidth();
    c.bottom = r.top + getWidth();

    thisleft.stretchToRectAlpha(canvas, &c);

    c.left = r.right-thisright.getWidth();
    c.top = r.top;
    c.right = r.right;
    c.bottom = r.top+getWidth();

    thisright.stretchToRectAlpha(canvas, &c);
  } else {
    RECT c;

    c.top = r.top;
    c.bottom = r.top;
    c.left = r.left;
    c.right = r.right;
    if (bmpbackgroundleft.valid()) {
      c.bottom = c.top + bmpbackgroundleft.getHeight();
      bmpbackgroundleft.stretchToRectAlpha(canvas, &c);
    }
    int l = c.bottom;
    c.top = r.bottom;
    c.bottom = r.bottom;
    if (bmpbackgroundright.valid()) {
      c.top = r.bottom - bmpbackgroundright.getHeight();
      bmpbackgroundright.stretchToRectAlpha(canvas, &c);
    }
    c.bottom = c.top;
    c.top = l;
    if (bmpbackgroundmiddle.valid()) {
      bmpbackgroundmiddle.stretchToRectAlpha(canvas, &c);
    }

    c.left = r.right - thisleft.getWidth();
    c.top = r.top+buttonx + shiftleft;
    c.right = r.right;
    c.bottom = r.top+buttonx+thisbutton.getHeight() + shiftleft;

    thisbutton.stretchToRectAlpha(canvas, &c);

    c.left = r.right - thisleft.getWidth();
    c.top = r.top;
    c.right = r.right;
    c.bottom = r.top+thisleft.getHeight();

    thisleft.stretchToRectAlpha(canvas, &c);

    c.left = r.right-thisright.getWidth();
    c.top = r.bottom-thisright.getHeight();
    c.right = r.right;
    c.bottom = r.bottom;

    thisright.stretchToRectAlpha(canvas, &c);
  }

  return 1;
}

int ScrollBar::getHeight() {
  return height;
}

void ScrollBar::setHeight(int newheight) {
  height = newheight;
}

int ScrollBar::onResize() {
  calcXPosition();
  invalidate();
  return 1;
}

int ScrollBar::onInit() {
  SCROLLBAR_PARENT::onInit();
  return 1;
}

void ScrollBar::setBitmaps(char *left, char *lpressed, char *lhilite,
                  char *right, char *rpressed, char *rhilite, 
                  char *button, char *bpressed, char *bhilite) {

  deleteResources();

#ifdef WASABI_COMPILE_SKIN
  bmpleft = left;
  bmplpressed = lpressed;
  bmplhilite = lhilite;
  bmpright = right;
  bmprpressed = rpressed;
  bmprhilite = rhilite;
  bmpbutton = button;
  bmpbpressed = bpressed;
  bmpbhilite = bhilite;
#else
#if 1
  // some placeholders in case you call this but there's no skin api
  bmpleft = Bitmap(16, 16, 0xffff0fff);
  bmplpressed = Bitmap(16, 16, 0xff0000ff);
  bmplhilite = Bitmap(16, 16, 0xff00ffff);
  bmpright = Bitmap(16, 16, 0xffff0000);
  bmprpressed = Bitmap(16, 16, 0xffff0000);
  bmprhilite = Bitmap(16, 16, 0xfffff000);
  bmpbutton = Bitmap(16, 16, 0xffff0f0f);
  bmpbpressed = Bitmap(16, 16, 0xffffffff);
  bmpbhilite = Bitmap(16, 16, 0xfff0ffff);
#endif
#endif

  loadBmps();
}

void ScrollBar::setBackgroundBitmaps(const char *left, const char *middle, const char *right) {
#ifdef WASABI_COMPILE_SKIN
  bmpbackgroundleft = left;
  bmpbackgroundmiddle = middle;
  bmpbackgroundright = right;
#else
#if 1
  // some placeholders in case you call this but there's no skin api
  bmpbackgroundleft = Bitmap(16, 16, 0xffff00ff);
  bmpbackgroundmiddle = Bitmap(16, 16, 0xff00ff00);
  bmpbackgroundright = Bitmap(16, 16, 0xff0000ff);
#endif
#endif
}

void ScrollBar::loadBmps() {

  ASSERT(leftrgn == NULL && buttonrgn == NULL && rightrgn == NULL);

  if (bmpleft.valid()) leftrgn = new RegionI(&bmpleft);
  if (bmpbutton.valid()) buttonrgn = new RegionI(&bmpbutton);
  if (bmpright.valid()) rightrgn = new RegionI(&bmpright);

  calcOverlapping();
  calcXPosition();
}

void ScrollBar::setPosition(int pos) {
  setPrivatePosition(pos, FALSE);
}

void ScrollBar::setPrivatePosition(int pos, BOOL signal, BOOL smooth) {
  if (insetpos) return; // helps stupid people (like me)
  insetpos = 1;
  position = MIN(SCROLLBAR_FULL, pos);
  position = MAX(0, position);
  calcXPosition();
  if (signal) onSetPosition(smooth);
  if (isInited() && isVisible())
  invalidate();
  insetpos = 0;
}

int ScrollBar::getPosition() {
  return position;
}

int ScrollBar::onSetPosition(BOOL smooth) {
  notifyParent(ChildNotify::SCROLLBAR_SETPOSITION, smooth);
  return 1;
}

int ScrollBar::onSetFinalPosition() {
  notifyParent(ChildNotify::SCROLLBAR_SETFINALPOSITION);
  return 1;
}

void ScrollBar::calcOverlapping() {

  if (!vertical) {

    shiftleft = bmpleft.getWidth();
    if (leftrgn && buttonrgn) {
      int i;
      for (i=shiftleft;i>=0;i--) {
        Region *reg = buttonrgn->clone();
        reg->offset(i, 0);
        if (leftrgn->intersectRgn(reg)) {
          i++;
          buttonrgn->disposeClone(reg);
          break;
        }
        buttonrgn->disposeClone(reg);
      }
      if (i >= 0)
        shiftleft = i;
    }

    shiftright = bmpright.getWidth();
    if (rightrgn && buttonrgn) {
      int i;
      for (i=0;i>=-shiftright;i--) {
        Region *reg = rightrgn->clone();
        reg->offset(i+bmpbutton.getWidth(), 0);
        if (reg->intersectRgn(buttonrgn)) {
          i++;
          rightrgn->disposeClone(reg);
          break;
        }
        rightrgn->disposeClone(reg);
      }
      if (i >= -shiftright)
        shiftright += i;
    }

  } else {

    shiftleft = bmpleft.getHeight();
    if (leftrgn && buttonrgn) {
      int i;
      for (i=shiftleft;i>=0;i--) {
        Region *reg = buttonrgn->clone();
        reg->offset(0, i);
        if (leftrgn->intersectRgn(reg)) {
          i++;
          buttonrgn->disposeClone(reg);
          break;
        }
        buttonrgn->disposeClone(reg);
      }
      if (i >= 0)
        shiftleft = i;
    }

    shiftright = bmpright.getHeight();
    if (rightrgn && buttonrgn) {
      int i;
      for (i=0;i>=-shiftright;i--) {
        Region *reg = rightrgn->clone();
        reg->offset(0, i+bmpbutton.getHeight());
        if (reg->intersectRgn(buttonrgn)) {
          i++;
          rightrgn->disposeClone(reg);
          break;
        }
        rightrgn->disposeClone(reg);
      }
      if (i >= -shiftright)
        shiftright += i;
    }

  }

}

void ScrollBar::calcXPosition() {

  if (!isInited()) return;

  RECT r;
  getClientRect(&r);

  int maxwidth;

  if (!vertical)
    maxwidth = (r.right-r.left)-(bmpbutton.getWidth()+shiftleft+shiftright)+1;
  else
    maxwidth = (r.bottom-r.top)-(bmpbutton.getHeight()+shiftleft+shiftright)+1;
  int oldx = buttonx;
  buttonx = (int)(((float)getPosition() / SCROLLBAR_FULL) * maxwidth);
  if (buttonx != oldx)
    invalidate();
}

void ScrollBar::calcPosition() {

  if (!isInited()) return;

  RECT r;
  getClientRect(&r);

  int maxwidth;

  if (!vertical)
    maxwidth = r.right-r.left-(bmpbutton.getWidth()+shiftleft+shiftright)+1;
  else
    maxwidth = r.bottom-r.top-(bmpbutton.getHeight()+shiftleft+shiftright)+1;
  setPrivatePosition((int)((float)buttonx / maxwidth * SCROLLBAR_FULL));
  //invalidate();
}

void ScrollBar::timerCallback(int id) {
  switch (id) {
    case TIMER_ID:
      if (firstdelay) {
        killTimer(TIMER_ID);
        setTimer(TIMER_ID, NEXT_DELAY);
        timer = 1;
        firstdelay = 0;
      }
      checkPageUpDown();
      break;
    case TIMER_ID2:
      if (firstdelay) {
        killTimer(TIMER_ID2);
        setTimer(TIMER_ID2, NEXT_DELAY);
        timer2 = 1;
        firstdelay = 0;
      }
      checkUpDown();
      break;
    default:
      SCROLLBAR_PARENT::timerCallback(id);
  }
}

// FG> smooth scrolling forced on, sorry, microsoft does it too so the user perceives IE scrolling as faster than it actually is
// eventho they tell you "The smooth-scrolling effect for list boxes should be disabled when this setting is FALSE. Your application must do this if it creates customized list boxes", they
// break their own rule so people don't bitch too much. ergo there is no reason we should not do that too.

int ScrollBar::pageUp() { 

  pageway = PAGE_UP;

  setPrivatePosition((int)MAX(0.f, (float)getPosition() + (float)SCROLLBAR_FULL / (npages-1)), TRUE, 1/*Std::osparam_getSmoothScroll()*/);

  return 1; 
};

int ScrollBar::pageDown() { 

  pageway = PAGE_DOWN;

  setPrivatePosition((int)MIN((float)SCROLLBAR_FULL, (float)getPosition() - (float)SCROLLBAR_FULL / (npages-1)), TRUE, 1/*Std::osparam_getSmoothScroll()*/);

  return 1; 
};

void ScrollBar::setNPages(int n) {
  //ASSERT(n >= 2);
  if (n < 2) n = 2;
  npages = n;
}

void ScrollBar::gotoPage(int page) {

  page = MIN(page, npages-1);
  page = MAX(page, 0);

  setPrivatePosition((int)((float)SCROLLBAR_FULL / (npages-1) * page), TRUE, FALSE);

}

void ScrollBar::setUpDownValue(int newupdown) {
  updown = newupdown;
}

int ScrollBar::upDown(int which) {
  switch (which) {
    case POS_LEFT:
      setPrivatePosition(getPosition()-updown);
      break;
    case POS_RIGHT:
      setPrivatePosition(getPosition()+updown);
      break;
  }
  return 1;
}

#ifdef WASABI_PLATFORM_WIN32
//happy for win32, we have some nice default scrolbar buttons when skin isn't present.

static Bitmap makeFrameControl(int t, int c, int w, int h) {
  ASSERT(w > 0);
  ASSERT(h > 0);
  BltCanvas bc(w, h);
  RECT r = Std::makeRect(0, 0, w, h);
  int res = DrawFrameControl(bc.getHDC(), &r, t, c);
  if (!res) {
//DebugString("dfc fail");
  }
  // force alpha to full
  for (int j = 0; j < w*h; j++) {
    ((ARGB32*)bc.getBits())[j] |= 0xff000000;
  }
  return Bitmap((ARGB32*)bc.getBits(), w, h);
}

#endif

void ScrollBar::setVertical(int isvertical) {
  vertical = isvertical;
  calcOverlapping();
#ifndef WASABI_COMPILE_SKIN
#ifdef WASABI_PLATFORM_WIN32
//happy for win32, we have some nice default scrolbar buttons when skin isn't present.

  if (vertical) {

    int sbw = StdWnd::getMetric(StdWnd::SCROLLBAR_VWIDTH);
    int sbh = StdWnd::getMetric(StdWnd::SCROLLBAR_VHEIGHT);

    bmpleft = makeFrameControl(DFC_SCROLL, DFCS_SCROLLUP, sbw, sbh);
    bmplpressed = makeFrameControl(DFC_SCROLL, DFCS_SCROLLUP|DFCS_PUSHED, sbw, sbh);
    bmplhilite = bmpleft;
    
    bmpright = makeFrameControl(DFC_SCROLL, DFCS_SCROLLDOWN, sbw, sbh);
    bmprpressed = makeFrameControl(DFC_SCROLL, DFCS_SCROLLDOWN|DFCS_PUSHED, sbw, sbh);
    bmprhilite = bmpright;

    bmpbutton = makeFrameControl(DFC_BUTTON, DFCS_BUTTONPUSH, sbw, sbh);
    bmpbpressed = makeFrameControl(DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_PUSHED, sbw, sbh);
    bmpbhilite = bmpbutton;

  } else { // horizontal

    int sbw = StdWnd::getMetric(StdWnd::SCROLLBAR_HWIDTH);
    int sbh = StdWnd::getMetric(StdWnd::SCROLLBAR_HHEIGHT);

    bmpleft = makeFrameControl(DFC_SCROLL, DFCS_SCROLLLEFT, sbw, sbh);
    bmplpressed = makeFrameControl(DFC_SCROLL, DFCS_SCROLLLEFT|DFCS_PUSHED, sbw, sbh);
    bmplhilite = bmpleft;
    
    bmpright = makeFrameControl(DFC_SCROLL, DFCS_SCROLLRIGHT, sbw, sbh);
    bmprpressed = makeFrameControl(DFC_SCROLL, DFCS_SCROLLRIGHT|DFCS_PUSHED, sbw, sbh);
    bmprhilite = bmpright;

    bmpbutton = makeFrameControl(DFC_BUTTON, DFCS_BUTTONPUSH, sbw, sbh);
    bmpbpressed = makeFrameControl(DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_PUSHED, sbw, sbh);
    bmpbhilite = bmpbutton;
  }

  unsigned long c = GetSysColor(COLOR_SCROLLBAR);
  c = BSWAP(c) >> 8;
  bmpbackgroundmiddle = Bitmap(1, 1, c|MKARGB32(255,0,0,0));

  loadBmps();
#endif
#endif
  if (isInited())
    invalidate();
}
