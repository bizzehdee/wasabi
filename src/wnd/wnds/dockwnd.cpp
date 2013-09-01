#include <precomp.h>
#include "dockwnd.h"
#include <wnd/popup.h>
#include <wnd/bltcanvas.h>

DockWnd::DockWnd() : m_allowed_sites(DOCK_ALL), 
                     m_docked_w(24), 
                     m_docked_h(24),
                     m_cursite(DOCK_NONE),
                     m_dockhost(NULL),
                     m_dragging(0),
                     m_dock_resizable(0),
                     m_resizing(RESIZE_NONE) {
}

DockWnd::~DockWnd() {
}

RootWnd *DockWnd::getWindow() {
  return this;
}

void DockWnd::setDockSites(int allowed_sites) { 
  m_allowed_sites = allowed_sites; 
}

int DockWnd::getDockSites() { 
  return m_allowed_sites; 
}

int DockWnd::getDockedWidth() {
  if (m_allowed_sites & (DOCK_LEFT|DOCK_RIGHT)) 
    return m_docked_w;
  return -1;
}

int DockWnd::getDockedHeight() {
  if (m_allowed_sites & (DOCK_TOP|DOCK_BOTTOM)) 
    return m_docked_h;
  return -1;
}

void DockWnd::setDockedWidth(int w) {
  m_docked_w = w;
}

void DockWnd::setDockedHeight(int h) {
  m_docked_h = h;
}

int DockWnd::getDockResizable() { 
  return m_dock_resizable;
}

void DockWnd::setDockResizable(int r) {
  m_dock_resizable = r;
}

int DockWnd::onRightButtonUp(int x, int y) {
  DOCKWND_PARENT::onLeftButtonUp(x, y);
  if (!m_dockhost) return 1;
  PopupMenu p;
  if (String(getName()).notempty()) {
    p.addCommand(getName(), 0, 0, 1);
    p.addSeparator();
  }
  if (m_allowed_sites & DOCK_LEFT) p.addCommand("Dock &Left", DOCK_LEFT, m_cursite == DOCK_LEFT);
  if (m_allowed_sites & DOCK_TOP) p.addCommand("Dock &Top", DOCK_TOP, m_cursite == DOCK_TOP);
  if (m_allowed_sites & DOCK_RIGHT) p.addCommand("Dock &Right", DOCK_RIGHT, m_cursite == DOCK_RIGHT);
  if (m_allowed_sites & DOCK_BOTTOM) p.addCommand("Dock &Bottom", DOCK_BOTTOM, m_cursite == DOCK_BOTTOM);
  int r = p.popAtMouse();
  if (r != -1) m_dockhost->dockWindow(this, r);
  return 1;
}

void DockWnd::onDock(DockHostWnd *host, int site) {
  m_dockhost = host;
  m_cursite = site;
}

int DockWnd::onResize() {
  DOCKWND_PARENT::onResize();
  invalidate();
  return 1;
}

int DockWnd::onInit() {
  DOCKWND_PARENT::onInit();
  const char *name = getName();
  return 1;
}

int DockWnd::getCurDockSite() {
  return m_cursite;
}

void DockWnd::onCancelCapture() {
  DOCKWND_PARENT::onCancelCapture();
  if (m_dragging) {
    eraseDragRect();
    m_dragging = 0;
    #ifdef WIN32
    LockWindowUpdate(NULL);
    #endif
  }
}

int DockWnd::getCursorType(int x, int y) {
  switch (hitTest(x, y)) {
    case HITTEST_DRAGANDDOCK: return BASEWND_CURSOR_POINTER;
    case HITTEST_RESIZE_LEFT: return BASEWND_CURSOR_EASTWEST;
    case HITTEST_RESIZE_TOP: return BASEWND_CURSOR_NORTHSOUTH;
    case HITTEST_RESIZE_RIGHT: return BASEWND_CURSOR_EASTWEST;
    case HITTEST_RESIZE_BOTTOM: return BASEWND_CURSOR_NORTHSOUTH;
  }
  return DOCKWND_PARENT::getCursorType(x, y);
}

int DockWnd::onLeftButtonDown(int x, int y) {
  DOCKWND_PARENT::onLeftButtonDown(x, y);

  switch (hitTest(x, y)) {
    case HITTEST_NONE: return 1;
    case HITTEST_DRAGANDDOCK: {
      RECT r;
      getNonClientRect(&r);
      m_dragrect_offset_x = x - r.left;
      m_dragrect_offset_y = y - r.top;

      beginCapture();
      m_dragging = 1;

      #ifdef WIN32
      LockWindowUpdate(GetDesktopWindow());
      #endif

      clientToScreen(&x, &y);
      drawDragRect(x, y);
      break;
    }
    case HITTEST_RESIZE_LEFT: {
        m_resize_clickx = x;
        m_resize_clicky = y;
        RECT rr;
        getDockResizeRect(&rr);
        m_resize_offset = x - rr.left;
        m_resizing = RESIZE_LEFT;
        beginCapture();
        clientToScreen(&x, &y);
        drawResizeLine(x, y);
      }
      break;
    case HITTEST_RESIZE_TOP: {
        m_resize_clickx = x;
        m_resize_clicky = y;
        RECT rr;
        getDockResizeRect(&rr);
        m_resize_offset = y - rr.top;
        m_resizing = RESIZE_TOP;
        beginCapture();
        clientToScreen(&x, &y);
        drawResizeLine(x, y);
      }
      break;
    case HITTEST_RESIZE_RIGHT: {
        m_resize_clickx = x;
        m_resize_clicky = y;
        RECT rr;
        getDockResizeRect(&rr);
        m_resize_offset = x - rr.left;
        m_resizing = RESIZE_RIGHT;
        beginCapture();
        clientToScreen(&x, &y);
        drawResizeLine(x, y);
      }
      break;
    case HITTEST_RESIZE_BOTTOM: {
        m_resize_clickx = x;
        m_resize_clicky = y;
        RECT rr;
        getDockResizeRect(&rr);
        m_resize_offset = y - rr.top;
        m_resizing = RESIZE_BOTTOM;
        beginCapture();
        clientToScreen(&x, &y);
        drawResizeLine(x, y);
      }
      break;
  }

  return 1;
}

int DockWnd::onLeftButtonUp(int x, int y) {
  DOCKWND_PARENT::onLeftButtonUp(x, y);
  if (m_dragging) {
    eraseDragRect();
    m_dragging = 0;
    endCapture();
    #ifdef WIN32
    LockWindowUpdate(NULL);
    #endif
    if (m_drag_lastdocksite != DOCK_NONE) m_dockhost->dockWindow(this, m_drag_lastdocksite, m_drag_lastdockpos);
  }
  if (m_resizing) {
    eraseResizeLine();
    endCapture();
    RECT r;
    VirtualWnd::getClientRect(&r);
    RECT pr;
    m_dockhost->VirtualWnd::getClientRect(&pr);
    POINT p;
    getPosition(&p);
    POINT pbr;
    getBRPosition(&pbr);
    RECT cr;
    VirtualWnd::getClientRect(&cr);
    int bs = getDockResizeBarSize();
    switch (m_resizing) {
      case RESIZE_RIGHT:
        pr.right -= m_dockhost->getRightOffset();
        if (x-cr.left < m_resize_offset) x = m_resize_offset + cr.left;
        if (x-cr.left > (pr.right-pr.left + m_resize_offset) - bs - p.x) x = (pr.right-pr.left) - bs + m_resize_offset - p.x + cr.left;
        setDockedWidth(r.right-r.left + (x - m_resize_clickx));
        break;
      case RESIZE_BOTTOM:
        pr.bottom -= m_dockhost->getBottomOffset();
        if (y-cr.top < m_resize_offset) y = m_resize_offset + cr.top;
        if (y-cr.top > (pr.bottom-pr.top) + m_resize_offset - bs - p.y) y = (pr.bottom-pr.top) - bs + m_resize_offset - p.y + cr.top;
        setDockedHeight(r.bottom-r.top + (y - m_resize_clicky));
        break;
      case RESIZE_LEFT:
        if (x-cr.left > (cr.right-cr.left) + m_resize_offset - bs) x = (cr.right-cr.left) + m_resize_offset - bs + cr.left;
        if (x-cr.left < -((pr.right-pr.left) - (cr.right-cr.left) - m_resize_offset - pbr.x)) x = -((pr.right-pr.left) - (cr.right-cr.left) - m_resize_offset - pbr.x) + cr.left;
        setDockedWidth(r.right-r.left + (m_resize_clickx - x));
        break;
      case RESIZE_TOP:
        if (y-cr.top > (cr.bottom-cr.top) + m_resize_offset - bs) y = (cr.bottom-cr.top) +m_resize_offset - bs + cr.top;
        if (y-cr.top < -((pr.bottom-pr.top) - (cr.bottom-cr.top) - m_resize_offset - pbr.y)) y = -((pr.bottom-pr.top) - (cr.bottom-cr.top) - m_resize_offset - pbr.y) + cr.top;
        setDockedHeight(r.bottom-r.top + (m_resize_clicky - y));
        break;
    }
    m_resizing = RESIZE_NONE;
    m_dockhost->updateDockedWindows();
    invalidate();
    UpdateWindow(gethWnd());
    updateCursor();
  }
  return 1;
}

int DockWnd::onMouseMove(int x, int y) {
  DOCKWND_PARENT::onMouseMove(x, y);
  if (m_dragging) {
    clientToScreen(&x, &y);
    if (x != m_dragrect_x || y != m_dragrect_y) {
      eraseDragRect();
      drawDragRect(x, y);
    }
  }
  if (m_resizing) {
    RECT pr;
    m_dockhost->VirtualWnd::getClientRect(&pr);
    POINT p;
    getPosition(&p);
    POINT pbr;
    getBRPosition(&pbr);
    RECT cr;
    VirtualWnd::getClientRect(&cr);
    int bs = getDockResizeBarSize();
    switch (m_resizing) {
      case RESIZE_RIGHT:
        pr.right -= m_dockhost->getRightOffset();
        if (x-cr.left < m_resize_offset) x = m_resize_offset + cr.left;
        if (x-cr.left > (pr.right-pr.left + m_resize_offset) - bs - p.x) x = (pr.right-pr.left) - bs + m_resize_offset - p.x + cr.left;
        break;
      case RESIZE_BOTTOM:
        pr.bottom -= m_dockhost->getBottomOffset();
        if (y-cr.top < m_resize_offset) y = m_resize_offset + cr.top;
        if (y-cr.top > (pr.bottom-pr.top) + m_resize_offset - bs - p.y) y = (pr.bottom-pr.top) - bs + m_resize_offset - p.y + cr.top;
        break;
      case RESIZE_LEFT:
        if (x-cr.left > (cr.right-cr.left) + m_resize_offset - bs) x = (cr.right-cr.left) + m_resize_offset - bs + cr.left;
        if (x-cr.left < -((pr.right-pr.left) - (cr.right-cr.left) - m_resize_offset - pbr.x)) x = -((pr.right-pr.left) - (cr.right-cr.left) - m_resize_offset - pbr.x) + cr.left;
        break;
      case RESIZE_TOP:
        if (y-cr.top > (cr.bottom-cr.top) + m_resize_offset - bs) y = (cr.bottom-cr.top) +m_resize_offset - bs + cr.top;
        if (y-cr.top < -((pr.bottom-pr.top) - (cr.bottom-cr.top) - m_resize_offset - pbr.y)) y = -((pr.bottom-pr.top) - (cr.bottom-cr.top) - m_resize_offset - pbr.y) + cr.top;
        break;
    }
    clientToScreen(&x, &y);
    if (x != m_resize_x || y != m_resize_y) {
      eraseResizeLine();
      drawResizeLine(x, y);
    }
  }
  return 1;
}

void DockWnd::drawDragRect(int x, int y) {
  m_dragrect_x = x;
  m_dragrect_y = y;

  int cx = x;
  int cy = y;
  ASSERT(m_dockhost != NULL);
  m_dockhost->screenToClient(&cx, &cy);

  RECT dockrect;
  
  m_drag_lastdocksite = m_dockhost->testDockWindow(this, cx, cy, &dockrect, &m_drag_lastdockpos);
  m_dragrect_dropallowed = m_drag_lastdocksite != 0;

  int w = dockrect.right-dockrect.left;
  int h = dockrect.bottom-dockrect.top;

  if (m_dragrect_offset_x > w) m_dragrect_offset_x = 4;
  if (m_dragrect_offset_y > h) m_dragrect_offset_y = 4;

  m_dragrect.left = x-m_dragrect_offset_x;
  m_dragrect.top = y-m_dragrect_offset_y;
  m_dragrect.right = x-m_dragrect_offset_x+w;
  m_dragrect.bottom = y-m_dragrect_offset_y+h;

  drawXorRect(&m_dragrect, m_dragrect_dropallowed);
}

void DockWnd::eraseDragRect() {
  drawXorRect(&m_dragrect, m_dragrect_dropallowed);
}

void DockWnd::drawXorRect(RECT *xr, int dropallowed) {
  #ifdef WIN32
  HDC sysdc = GetDCEx(NULL, NULL, DCX_LOCKWINDOWUPDATE);

  int shift = 0;
  HPEN pen;
  if (dropallowed) {
    pen = CreatePen(PS_SOLID, 0, 0xFFFFFF);
  } else {
    shift = 3;
    HANDLE handle = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER)+(8*8*32));
    char *dib = (char *)GlobalLock(handle);
    BITMAPINFOHEADER *bih = (BITMAPINFOHEADER *)dib;
    bih->biSize = sizeof(BITMAPINFOHEADER);
    bih->biWidth = 8;
    bih->biHeight = 8;
    bih->biPlanes = 1;
    bih->biBitCount = 32;
    bih->biCompression = BI_RGB;
    bih->biSizeImage = 0;
    bih->biXPelsPerMeter = 0;
    bih->biYPelsPerMeter = 0;
    COLORREF *bits = (COLORREF *)(dib + sizeof(BITMAPINFOHEADER));
    for (int i=0;i<8;i++) {
      for (int j=0;j<8;j++) {
        if (!((j+i) % 2)) 
          *bits = 0xFFFFFF;
        else 
          *bits = 0;
        bits++;
      }
    }
    LOGBRUSH lb;
    lb.lbColor = DIB_RGB_COLORS;
    lb.lbHatch = (LONG)handle;
    lb.lbStyle = BS_DIBPATTERN;
    GlobalUnlock(handle);
    pen = ExtCreatePen(PS_GEOMETRIC, dropallowed ? 1 : 4, &lb, 0, NULL);
    GlobalFree(handle);
  }
  HBRUSH oldbrush = (HBRUSH)SelectObject(sysdc, GetStockObject(NULL_BRUSH));
  HPEN oldpen = (HPEN)SelectObject(sysdc, pen);
  int oldmode = SetROP2(sysdc,R2_XORPEN);

  Rectangle(sysdc, xr->left, xr->top, xr->right, xr->bottom);

  SetROP2(sysdc, oldmode);
  SelectObject(sysdc, oldpen);
  SelectObject(sysdc, oldbrush);
  DeleteObject(pen);

  ReleaseDC(NULL, sysdc);
  #endif
}

void DockWnd::drawResizeLine(int x, int y) {
  m_resize_x = x;
  m_resize_y = y;
  drawXorLine(m_resize_x, m_resize_y);
}

void DockWnd::eraseResizeLine() {
  drawXorLine(m_resize_x, m_resize_y);
}

void DockWnd::drawXorLine(int x, int y) {
  #ifdef WIN32
  HDC sysdc = GetDCEx(NULL, NULL, DCX_LOCKWINDOWUPDATE);

  int shift = 0;
  HPEN pen;

  pen = CreatePen(PS_SOLID, 1, 0xFFFFFF);

  HBRUSH oldbrush = (HBRUSH)SelectObject(sysdc, GetStockObject(WHITE_BRUSH));
  HPEN oldpen = (HPEN)SelectObject(sysdc, pen);
  int oldmode = SetROP2(sysdc,R2_XORPEN);

  RECT r;
  VirtualWnd::getClientRect(&r);
  VirtualWnd::clientToScreen(&r);
  switch (m_resizing) {
    case RESIZE_LEFT:
    case RESIZE_RIGHT:
      Rectangle(sysdc, x-m_resize_offset, r.top, x-m_resize_offset+getDockResizeBarSize(), r.bottom);
      break;
    case RESIZE_TOP:
    case RESIZE_BOTTOM:
      Rectangle(sysdc, r.left, y-m_resize_offset, r.right, y-m_resize_offset+getDockResizeBarSize());
      break;
  }

  SetROP2(sysdc, oldmode);
  SelectObject(sysdc, oldpen);
  SelectObject(sysdc, oldbrush);
  DeleteObject(pen);

  ReleaseDC(NULL, sysdc);
  #endif
}

int DockWnd::getDockResizeBarSize() {
  RECT rr;
  getDockResizeRect(&rr);
  switch (getCurDockSite()) {
    case DOCK_LEFT:
    case DOCK_RIGHT:
      return rr.right-rr.left;
    case DOCK_TOP:
    case DOCK_BOTTOM:
      return rr.bottom-rr.top;
  }
  return 4;
}

//override!
void DockWnd::getDockResizeRect(RECT *r) {
  DockWnd::getClientRect(r);
  switch (getCurDockSite()) {
    case DOCK_LEFT:
      r->left = r->right - 4;
      break;
    case DOCK_RIGHT:
      r->right = r->left + 4;
      break;
    case DOCK_TOP:
      r->top = r->bottom - 4;
      break;
    case DOCK_BOTTOM:
      r->bottom = r->top + 4;
      break;
  }
}

