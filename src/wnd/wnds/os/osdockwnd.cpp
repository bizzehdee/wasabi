#include <precomp.h>
#include "osdockwnd.h"
#include <bfc/std_wnd.h>
#include <bfc/util/inifile.h>
#include <wnd/canvas.h>

OSDockWnd::OSDockWnd() : m_gripvisible(1), 
                         m_bordervisible(1),
                         m_snapable(1),
                         m_snap_collapsed(0),
                         m_snap_bdown(0),
                         m_snap_inarea(0),
                         m_snap_width(-1),
                         m_snap_height(-1) {
  MEMSET(&m_resizer, 0, sizeof(RECT));
  MEMSET(&m_snapbtn, 0, sizeof(RECT));
}

int OSDockWnd::onPaint(Canvas *c) {
  RECT r;
  OSDOCKWND_PARENT::getClientRect(&r);

  c->fillRect(&r, GetSysColor(COLOR_3DFACE));

  if (wantBorder())
    c->drawSunkenRect(&r, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));

  RECT griprect = r;

  switch (getCurDockSite()) {
    case DOCK_LEFT:
    case DOCK_RIGHT:
      griprect.bottom = griprect.top + getResizeBarSize();
      if (wantBorder()) {
        griprect.top += 1;
        griprect.right -=1;
        griprect.left +=1;
      }

    break;

    case DOCK_TOP:
    case DOCK_BOTTOM:
      griprect.right = griprect.left + getResizeBarSize();
      if (wantBorder()) {
        griprect.top += 1;
        griprect.bottom -=1;
        griprect.left +=1;
      }
    break;
  }

  if (getDockResizable()) {
    OSDockWnd::getClientRect(&m_resizer);
    switch (getCurDockSite()) {
      case DOCK_LEFT:
        m_resizer.left = m_resizer.right + 1;
        m_resizer.right = m_resizer.left + getResizeBarSize();
        break;
      case DOCK_TOP:
        m_resizer.top = m_resizer.bottom + 1;
        m_resizer.bottom = m_resizer.top + getResizeBarSize();
        break;
      case DOCK_RIGHT:
        m_resizer.right = m_resizer.left - 1;
        m_resizer.left = m_resizer.right - getResizeBarSize();
        break;
      case DOCK_BOTTOM:
        m_resizer.bottom = m_resizer.top - 1;
        m_resizer.top = m_resizer.bottom - getResizeBarSize();
        break;
    }
#if 0
    c->drawSunkenRect(&m_resizer, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
#else
    RECT xx = m_resizer;
//    xx.left = xx.right;
    switch (getCurDockSite()) {
    case DOCK_LEFT:
    case DOCK_RIGHT:
      xx.top -= 80;
      xx.left += 1;
      xx.right -= 1;
      xx.bottom += 80;
      break;

    case DOCK_TOP:
    case DOCK_BOTTOM:
      xx.top += 1;
      xx.left -= 80;
      xx.right += 80;
      xx.bottom -= 1;
      break;
    }
    c->drawRect(&xx, TRUE, GetSysColor(COLOR_3DSHADOW));
#endif

    if (m_snapable) {
      switch (getCurDockSite()) {
        case DOCK_LEFT:
        case DOCK_RIGHT:
          m_snapbtn.left = m_resizer.left;
          m_snapbtn.top = m_resizer.top + (m_resizer.bottom - m_resizer.top - 64) / 2;
          m_snapbtn.bottom = m_snapbtn.top + SNAPBTN_SIZE;
          m_snapbtn.right = m_resizer.right;
          c->drawSunkenRect(&m_snapbtn, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
          m_snapbtn.top++;
          m_snapbtn.bottom--;
          if (!(m_snap_bdown && m_snap_inarea)) c->drawSunkenRect(&m_snapbtn, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
          drawSnapArrow(c);
          break;
        case DOCK_TOP:
        case DOCK_BOTTOM:
          m_snapbtn.top = m_resizer.top;
          m_snapbtn.left = m_resizer.left + (m_resizer.right - m_resizer.left - 64) / 2;
          m_snapbtn.right = m_snapbtn.left + SNAPBTN_SIZE;
          m_snapbtn.bottom = m_resizer.bottom;
          c->drawSunkenRect(&m_snapbtn, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
          m_snapbtn.left++;
          m_snapbtn.right--;
          if (!(m_snap_bdown && m_snap_inarea)) c->drawSunkenRect(&m_snapbtn, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
          drawSnapArrow(c);
          break;
      }
    }
  }

  if (wantGrips()) {
    c->fillRect(&griprect, GetSysColor(COLOR_3DFACE));
    int site = getCurDockSite();
    if (site == DOCK_LEFT || site == DOCK_RIGHT) {
      drawHGrip(c, griprect);
    } else if (site == DOCK_TOP || site == DOCK_BOTTOM) {
      drawVGrip(c, griprect);
    }
  }

  return 1;
}

void OSDockWnd::getClientRect(RECT *r) {
  OSDOCKWND_PARENT::getClientRect(r);
#if 0
  r->left++;
  r->top++;
  r->bottom--;
  r->right--;
#endif
  if (wantGrips()) {
    int site = getCurDockSite();
    if (site == DOCK_LEFT || site == DOCK_RIGHT) {
      r->top += GRIP_WIDTH;
    } else if (site == DOCK_TOP || site == DOCK_BOTTOM) {
      r->left += GRIP_WIDTH;
    }
  }
  if (getDockResizable()) {
    switch (getCurDockSite()) {
      case DOCK_LEFT: r->right -= getResizeBarSize()/*CUT+1*/; break;
      case DOCK_TOP: r->bottom -= getResizeBarSize()/*CUT+1*/; break;
      case DOCK_RIGHT: r->left += getResizeBarSize()/*CUT+1*/; break;
      case DOCK_BOTTOM: r->top += getResizeBarSize()/*CUT+1*/; break;
    }
  }
}

void OSDockWnd::drawHGrip(Canvas *c, RECT r) {
//  RECT r;
//  OSDOCKWND_PARENT::getClientRect(&r);
#if 1
  int w = r.right - r.left;
  r.left = r.left+2;
  r.top = r.top+2;
  r.right = r.left+w-4;
  r.bottom = r.top+3;
#endif
  c->drawSunkenRect(&r, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
#if 1
  Std::offsetRect(&r, 0, 4);
  c->drawSunkenRect(&r, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
#endif
}

void OSDockWnd::drawVGrip(Canvas *c, RECT r) {
#if 1
  int h = r.bottom - r.top;
  r.left = r.left+2;
  r.top = r.top+2;
  r.right = r.left+3;
  r.bottom = r.top + h-4;
#endif
  c->drawSunkenRect(&r, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
#if 1
  Std::offsetRect(&r, 4, 0);
  c->drawSunkenRect(&r, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
#endif
}

void OSDockWnd::setBorderVisible(int v) {
  if (m_bordervisible == v) return;
  m_bordervisible = v;
  cascadeRepaint();
}

void OSDockWnd::setGripVisible(int v) {
  if (m_gripvisible == v) return;
  m_gripvisible = v;
  cascadeRepaint();
}

int OSDockWnd::hitTest(int x, int y) {
  POINT pt={x,y};

  RECT r;
  OSDockWnd::getClientRect(&r);
  if (PtInRect(&r, pt)) return HITTEST_NONE;
  
  if (!getDockResizable()) return HITTEST_DRAGANDDOCK;

  if (PtInRect(&m_snapbtn, pt)) return HITTEST_NONE;

  if (PtInRect(&m_resizer, pt)) {
    switch (getCurDockSite()) {
      case DOCK_LEFT: return HITTEST_RESIZE_RIGHT;
      case DOCK_TOP: return HITTEST_RESIZE_BOTTOM;
      case DOCK_RIGHT: return HITTEST_RESIZE_LEFT;
      case DOCK_BOTTOM: return HITTEST_RESIZE_TOP;
    }
  }
  return HITTEST_DRAGANDDOCK;
}

int OSDockWnd::onLeftButtonDown(int x, int y) {
  OSDOCKWND_PARENT::onLeftButtonDown(x, y);
  POINT pt={x,y};
  if (PtInRect(&m_snapbtn, pt)) {
    m_snap_bdown = 1;
    m_snap_inarea = 1;
    beginCapture();
    invalidateRect(&m_snapbtn);
    UpdateWindow(gethWnd());
  }
  return 1;
}

int OSDockWnd::onLeftButtonUp(int x, int y) {
  OSDOCKWND_PARENT::onLeftButtonUp(x, y);
  POINT pt={x,y};
  if (m_snap_bdown) {
    m_snap_bdown = 0;
    m_snap_inarea = 0;
    endCapture();
    if (PtInRect(&m_snapbtn, pt)) {
      onSnapClick();
    }
  }
  return 1;
}

int OSDockWnd::onMouseMove(int x, int y) {
  OSDOCKWND_PARENT::onMouseMove(x, y);
  POINT pt={x,y};
  if (m_snap_bdown) {
    if (PtInRect(&m_snapbtn, pt)) {
      if (!m_snap_inarea) {
        m_snap_inarea = 1;
        invalidateRect(&m_snapbtn);
        UpdateWindow(gethWnd());
      }
    } else {
      if (m_snap_inarea) {
        m_snap_inarea = 0;
        invalidateRect(&m_snapbtn);
        UpdateWindow(gethWnd());
      }
    }
  }
  return 1;
}

void OSDockWnd::onSnapClick() {
  if (m_snap_width == -1 || m_snap_height == -1) {
    m_snap_width = DockWnd::getDockedWidth();
    m_snap_height = DockWnd::getDockedHeight();
  }
  m_snap_collapsed = !m_snap_collapsed;
  getDockHost()->updateDockedWindows();
}

void OSDockWnd::getDockResizeRect(RECT *r) {
  *r = m_resizer;
}

void OSDockWnd::loadCustomState(const char *inifile, const char *section) {
  IniFile ini(inifile);
  int was_snapable = ini.getInt(section, StringPrintf("%s_snapable", getName()), 0);
  m_snap_collapsed = ini.getInt(section, StringPrintf("%s_snap_collapsed", getName()), m_snap_collapsed);
  m_snap_width= ini.getInt(section, StringPrintf("%s_snap_width", getName()), m_snap_width);
  m_snap_height = ini.getInt(section, StringPrintf("%s_snap_height", getName()), m_snap_height);
  if (was_snapable) {
    DockWnd::setDockedWidth(m_snap_width);
    DockWnd::setDockedHeight(m_snap_height);
  } 
}

void OSDockWnd::saveCustomState(const char *inifile, const char *section) {
  IniFile ini(inifile);
  ini.setInt(section, StringPrintf("%s_snapable", getName()), m_snapable && getDockResizable());
  ini.setInt(section, StringPrintf("%s_snap_collapsed", getName()), m_snap_collapsed);
  ini.setInt(section, StringPrintf("%s_snap_width", getName()), m_snap_width);
  ini.setInt(section, StringPrintf("%s_snap_height", getName()), m_snap_height);
}

int OSDockWnd::getDockedWidth() {
  if (m_snapable) {
    if (m_snap_collapsed) return getResizeBarSize();
  }
  return OSDOCKWND_PARENT::getDockedWidth();
}

int OSDockWnd::getDockedHeight() {
  if (m_snapable) {
    if (m_snap_collapsed) return getResizeBarSize();
  }
  return OSDOCKWND_PARENT::getDockedHeight();
}

void OSDockWnd::setDockedWidth(int w) {
  if (m_snapable) {
    if (w <= getResizeBarSize() * 2) {
      m_snap_collapsed = 1;
      return;
    }
    if (m_snap_collapsed) 
      m_snap_collapsed = 0;
    m_snap_width = w;
  }
  OSDOCKWND_PARENT::setDockedWidth(w);
}

void OSDockWnd::setDockedHeight(int h) {
  if (m_snapable) {
    if (h <= getResizeBarSize() * 2) {
      m_snap_collapsed = 1;
      return;
    }
    if (m_snap_collapsed) 
      m_snap_collapsed = 0;
    m_snap_height = h;
  }
  OSDOCKWND_PARENT::setDockedHeight(h);
}

void OSDockWnd::drawSnapArrow(Canvas *c) {
  int way = 0; // left top right bottom
  int x, y;
  switch (getCurDockSite()) {
    case DOCK_LEFT:
      way = m_snap_collapsed ? 1 : 3;
      x = m_snapbtn.left + 2 + (m_snap_collapsed ? 1 : 0);
      y = m_snapbtn.top + (SNAPBTN_SIZE/2) - 3;
      break;
    case DOCK_TOP:
      way = m_snap_collapsed ? 2 : 4;
      x = m_snapbtn.left + (SNAPBTN_SIZE/2) - 3;
      y = m_snapbtn.top + 2 + (m_snap_collapsed ? 1 : 0);
      break;
    case DOCK_RIGHT:
      way = m_snap_collapsed ? 3 : 1;
      x = m_snapbtn.left + 2 - (m_snap_collapsed ? 1 : 0);
      y = m_snapbtn.top + (SNAPBTN_SIZE/2) - 3;
      break;
    case DOCK_BOTTOM:
      way = m_snap_collapsed ? 4 : 2;
      x = m_snapbtn.left + (SNAPBTN_SIZE/2) - 3;
      y = m_snapbtn.top + 2 - (m_snap_collapsed ? 1 : 0);
      break;
  }
  if (m_snap_bdown && m_snap_inarea) { x++; y++; }
  switch (way) {
    case 1: // toward left
      c->moveTo(x, y);
      c->lineTo(x, y+5);
      c->moveTo(x+1, y+1);
      c->lineTo(x+1, y+4);
      c->moveTo(x+2, y+2);
      c->lineTo(x+2, y+3);
      break;
    case 2: // toward top
      c->moveTo(x, y);
      c->lineTo(x+5, y);
      c->moveTo(x+1, y+1);
      c->lineTo(x+4, y+1);
      c->moveTo(x+2, y+2);
      c->lineTo(x+3, y+2);
      break;
    case 3: // toward right
      c->moveTo(x+2, y);
      c->lineTo(x+2, y+5);
      c->moveTo(x+1, y+1);
      c->lineTo(x+1, y+4);
      c->moveTo(x, y+2);
      c->lineTo(x, y+3);
      break;
    case 4: // toward bottom
      c->moveTo(x, y+2);
      c->lineTo(x+5, y+2);
      c->moveTo(x+1, y+1);
      c->lineTo(x+4, y+1);
      c->moveTo(x+2, y);
      c->lineTo(x+3, y);
      break;
  }
}

void OSDockWnd::setDockSnapable(int snapable) {
  if (snapable == m_snapable) return;
  m_snapable = snapable;
  if (isInited()) {
    invalidate();
    UpdateWindow(gethWnd());
  }
}

void OSDockWnd::onDockChange() {
}

// -

OSDockWndHoster::OSDockWndHoster(BaseWnd *wnd, int dockedwidth) {
  myhwnd = NULL;
  mywnd = wnd;
  mywnd->setParent(this);

  if (dockedwidth > 0) setDockedWidth(dockedwidth);
}

OSDockWndHoster::OSDockWndHoster(OSWINDOWHANDLE wnd, int dockedwidth) {
  ASSERT(wnd != NULL);
  myhwnd = wnd;
  mywnd = NULL;
  StdWnd::setParent(wnd, getOsWindowHandle());

  if (dockedwidth > 0) setDockedWidth(dockedwidth);
}

OSDockWndHoster::OSDockWndHoster(int dockedwidth) {
  myhwnd = NULL;
  this->mywnd = NULL;

  if (dockedwidth > 0) setDockedWidth(dockedwidth);
}

OSDockWndHoster::~OSDockWndHoster() {
//FUCKO: Destroy the hwnd?
  delete mywnd;
}

int OSDockWndHoster::onInit() {
  int r = OSDOCKWNDHOSTER_PARENT::onInit();

  if (mywnd != NULL) mywnd->init(this);
  
  return r;
}

int OSDockWndHoster::onResize() {
  int r = OSDOCKWNDHOSTER_PARENT::onResize();

  if (mywnd) resizeWndToClient(mywnd);

  return r;
}

void OSDockWndHoster::setWindowHandle(OSWINDOWHANDLE wnd) {
  ASSERT(myhwnd == NULL);
  ASSERT(mywnd == NULL);
  ASSERT(wnd != NULL);
  myhwnd = wnd;
  StdWnd::setParent(wnd, getOsWindowHandle());
}
