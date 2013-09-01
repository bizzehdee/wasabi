#include <precomp.h>
#include "ostoolbarwnd.h"
#include <wnd/canvas.h>
#include <bfc/std_wnd.h>
#include "commandcb.h"

OSToolBarEntry::OSToolBarEntry(OSToolBarWnd *parent, const char *tooltip)
: m_parent(parent), m_tip(tooltip), m_disabled(0), m_cmd(0), m_stretch(0) {
  ZERO(m_rect);
}

OSToolBarEntry::~OSToolBarEntry() {
}

void OSToolBarEntry::setLastRect(RECT r) {
  m_rect = r;
}

RECT OSToolBarEntry::getLastRect() {
  return m_rect;
}

void OSToolBarEntry::render(Canvas *canvas, const RECT &ir, int hilited, int pushed) {
  if (hilited && !isDisabled()) {
    if (pushed) 
      canvas->drawSunkenRect(&ir, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
    else
      canvas->drawSunkenRect(&ir, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
  }
}

int OSToolBarEntry::isDisabled() {
  int retval = !!m_disabled;
  CommandCallback *cc = m_parent->getCommandCallback();
  if (cc) retval = !cc->onGetEnabled(m_cmd, !retval);
  return retval;
}
 
// --

OSToolBarEntryIcon::OSToolBarEntryIcon(OSToolBarWnd *parent, int icon_resource, int icow, const char *tooltip) :
  OSToolBarEntry(parent, tooltip),
  m_icon_resource(icon_resource),
  icon_width(icow)
{
  m_icon = (HICON) LoadImage(WASABI_API_APP->main_gethInstance(), MAKEINTRESOURCE(icon_resource), IMAGE_ICON, icon_width, icon_width, LR_DEFAULTCOLOR);
}

OSToolBarEntryIcon::~OSToolBarEntryIcon() {
  if (m_icon) DestroyIcon(m_icon); 
}

void OSToolBarEntryIcon::render(Canvas *canvas, const RECT &ir, int hilited, int pushed) {
  DrawState(canvas->getHDC(), NULL, NULL, (long)m_icon, 0, ir.left+3, ir.top+3, 16, 16, DST_ICON|(isDisabled()?DSS_DISABLED:0));
  OSToolBarEntry::render(canvas, ir, hilited, pushed);
}

// --

OSToolBarEntrySeparator::OSToolBarEntrySeparator(OSToolBarWnd *parent) :
  OSToolBarEntry(parent)
{ }

void OSToolBarEntrySeparator::render(Canvas *canvas, const RECT &ir, int hilited, int pushed) {
  int x = ir.left, y = ir.top;
  int w = ir.right - ir.left, h = ir.bottom - ir.top;

  if (w > h) {
    canvas->pushPen(GetSysColor(COLOR_3DSHADOW));
    canvas->lineDraw(x+3, y+4, x+w-3, y+4);
    canvas->popPen();
    canvas->pushPen(GetSysColor(COLOR_3DHILIGHT));
    canvas->lineDraw(x+3, y+5, x+w-3, y+5);
    canvas->popPen();
  } else {
    canvas->pushPen(GetSysColor(COLOR_3DSHADOW));
    canvas->lineDraw(x+4, y+3, x+4, y+h-3);
    canvas->popPen();
    canvas->pushPen(GetSysColor(COLOR_3DHILIGHT));
    canvas->lineDraw(x+5, y+3, x+5, y+h-3);
    canvas->popPen();
  }
}

// --

OSToolBarEntryWndHoster::OSToolBarEntryWndHoster(OSToolBarWnd *parent, BaseWnd *wnd, int dockedwidth) : OSToolBarEntry(parent){
  myhwnd = NULL;
  mywnd = wnd;
  mywnd->setParent(parent);

  m_dockedwidth = dockedwidth;
}

OSToolBarEntryWndHoster::OSToolBarEntryWndHoster(OSToolBarWnd *parent, OSWINDOWHANDLE wnd, int dockedwidth): OSToolBarEntry(parent) {
  ASSERT(wnd != NULL);
  myhwnd = wnd;
  mywnd = NULL;
  StdWnd::setParent(wnd, parent->getOsWindowHandle());

  m_dockedwidth = dockedwidth;
}

OSToolBarEntryWndHoster::OSToolBarEntryWndHoster(OSToolBarWnd *parent, int dockedwidth): OSToolBarEntry(parent) {
  myhwnd = NULL;
  this->mywnd = NULL;

  m_dockedwidth = dockedwidth;
}

OSToolBarEntryWndHoster::~OSToolBarEntryWndHoster() {
  delete mywnd;
}


void OSToolBarEntryWndHoster::onParentInit(){
  if (mywnd)
    mywnd->init(getParent());
}


void OSToolBarEntryWndHoster::onParentResize() {
  RECT r = getLastRect();
  if (mywnd)
    mywnd->resize(&r);
}

void OSToolBarEntryWndHoster::setWindowHandle(OSWINDOWHANDLE wnd) {
  ASSERT(myhwnd == NULL);
  ASSERT(mywnd == NULL);
  ASSERT(wnd != NULL);
  myhwnd = wnd;
  StdWnd::setParent(wnd, getParent()->getOsWindowHandle());
}

int OSToolBarEntryWndHoster::getRenderWidth() { 
  if ( m_dockedwidth == -2 ) {
    RECT r;
    getParent()->getClientRect(&r);
    return r.right - r.left;
  }
  return m_dockedwidth; 

}
int OSToolBarEntryWndHoster::getRenderHeight() { 
  if ( m_dockedwidth == -2 ) {
    RECT r;
    getParent()->getClientRect(&r);
    return r.bottom - r.top;
  }  
  return m_dockedwidth; 
}

void OSToolBarEntryWndHoster::render(Canvas *canvas, const RECT &ir, int hilited, int pushed) {
  if (mywnd) mywnd->invalidate();
}


OSToolBarWnd::OSToolBarWnd() {
  m_tip = NULL;
  m_cc = NULL;
  m_hilite = NULL;
  m_down = 0;
  m_last_pushed = -1;
  want_middle_clicks = is_middle_click = 0;
  setVirtual(0);
}

OSToolBarWnd::~OSToolBarWnd() {
  delete m_tip; m_tip = NULL;
  m_hilite = NULL;
  m_entries.deleteAll();
}

OSToolBarEntry *OSToolBarWnd::addIconEntry(int icon_resource, int icon_width, const char *tooltip) {
  invalidate();
  OSToolBarEntry *ret = m_entries.addItem(new OSToolBarEntryIcon(this, icon_resource, icon_width, tooltip));
  onResize();
  return ret;
}

OSToolBarEntry *OSToolBarWnd::addSeparator() {
  invalidate();
  OSToolBarEntry *ret = m_entries.addItem(new OSToolBarEntrySeparator(this));
  onResize();
  return ret;
}

OSToolBarEntry *OSToolBarWnd::addUserClass(OSToolBarEntry *wnd) {
  m_entries.addItem(wnd);
  invalidate();
  onResize();
  return wnd;
}

void OSToolBarWnd::removeUserClass(OSToolBarEntry *wnd) {
  m_entries.removeItem(wnd);
  invalidate();
  onResize();
}

int OSToolBarWnd::onInit() {
  int r = OSTOOLBARWND_PARENT::onInit();

  foreach(m_entries)
    m_entries.getfor()->onParentInit();
  endfor

  return r;
}

int OSToolBarWnd::onResize() {
  RECT r = clientRect();
  int w = r.right - r.left, h = r.bottom - r.top;
  int site = getCurDockSite();

  if (site == DOCK_LEFT || site == DOCK_RIGHT) {
    int y = r.top + 1;
    int availStretch = h - 2;
    int nbStretch = 0;
    foreach(m_entries)
      OSToolBarEntry *entry = m_entries.getfor();
      int entryh = entry->getRenderHeight();
      if ( entry->getStretch() ) {
        nbStretch +=1;
      }
      availStretch -= entryh;
    endfor;
    foreach(m_entries)
      OSToolBarEntry *entry = m_entries.getfor();
      int entryh = entry->getRenderHeight();
      if ( entry->getStretch() && nbStretch ) {
        int stretchAmount = availStretch / nbStretch;
        entryh += stretchAmount;
        availStretch -= entryh;
        nbStretch -= 1;        
      }
      RECT ir = {r.left+3, y+1, r.left+w-2, y+entryh};
      entry->setLastRect(ir);

      y += entryh;
    endfor;
  } else if (site == DOCK_TOP || site == DOCK_BOTTOM) {
    int x = r.left + 1;
    int availStretch = w - 2;
    int nbStretch = 0;
    foreach(m_entries)
      OSToolBarEntry *entry = m_entries.getfor();
      int entryh = entry->getRenderHeight();
      if ( entry->getStretch() ) {
        nbStretch +=1;
      }
      availStretch -= entryh;
    endfor;
    foreach(m_entries)
      OSToolBarEntry *entry = m_entries.getfor();
      int entryw = entry->getRenderWidth();
      if ( entry->getStretch() && nbStretch ) {
        int stretchAmount = availStretch / nbStretch;
        entryw += stretchAmount;
        availStretch -= entryw;
        nbStretch -= 1;        
      }
      RECT ir = {x+1, r.top+4, x+entryw, r.bottom-2};
      entry->setLastRect(ir);

      x += entryw;
    endfor;
  }

  foreach(m_entries)
    OSToolBarEntry *entry = m_entries.getfor();
    entry->onParentResize();
  endfor;

  return 1;
}

int OSToolBarWnd::onPaint(Canvas *canvas) {
  OSTOOLBARWND_PARENT::onPaint(canvas);

  int mx, my;
  StdWnd::getMousePos(&mx, &my);

  foreach(m_entries)
    OSToolBarEntry *entry = m_entries.getfor();

    RECT ncr = entry->getLastRect();
    clientToScreen(&ncr);
    int r_down = (m_down && (m_last_pushed==foreach_index) && Std::pointInRect(&ncr, mx, my));

    entry->render(canvas, entry->getLastRect(), (m_hilite==entry), r_down);

  endfor;

  return 1;
}

OSToolBarEntry *OSToolBarWnd::getEntryAt(int x, int y) {
  foreach(m_entries)
    OSToolBarEntry *entry = m_entries.getfor();
    RECT ir = entry->getLastRect();
    if (Std::pointInRect(&ir, x, y)) return entry;
  endfor;
  return NULL;
}

int OSToolBarWnd::getEntryPos(OSToolBarEntry *entry) {
  return m_entries.searchItem(entry);
}

int OSToolBarWnd::hitTest(int x, int y) {
  OSTOOLBARWND_PARENT::onMouseMove(x, y);
  return (getEntryAt(x, y) != NULL) ? HITTEST_NONE : HITTEST_DRAGANDDOCK;
}

int OSToolBarWnd::onMouseMove(int x, int y) {
  OSTOOLBARWND_PARENT::onMouseMove(x, y);
  if (!isDragging()) localMouseMove(x, y);
  return 1;
}

void OSToolBarWnd::localMouseMove(int x, int y) {
  OSToolBarEntry *old = m_hilite;
  OSToolBarEntry *at = getEntryAt(x, y);
  if (!at) {
    if (m_hilite) {
      onLeaveIcon(m_hilite);
      m_hilite = NULL;
    }
  } else {
    if (at != m_hilite) {
      if (m_hilite) onLeaveIcon(m_hilite);
      m_hilite = at;
      onEnterIcon(m_hilite);
    } else {
      // repaint current in case its animated -BU
      invalidateIcon(m_hilite);
    }
  }
  if (!old && m_hilite) beginCapture();
  else if (old && !m_hilite) endCapture();
}

void OSToolBarWnd::invalidateIcon(OSToolBarEntry *entry) {
  RECT r = entry->getLastRect();
  invalidateRect(&r);
  cascadeRepaintRect(&r);
}

int OSToolBarWnd::onLeftButtonDown(int x, int y) {
  OSTOOLBARWND_PARENT::onLeftButtonDown(x, y);
  if (m_hilite) invalidateIcon(m_hilite);
  foreach(m_entries)
    RECT ir = m_entries.getfor()->getLastRect();
    if (Std::pointInRect(&ir, x, y)) {
      m_last_pushed = foreach_index;
      break;
    }
  endfor
  m_down = 1;
  return 1;
}

int OSToolBarWnd::onLeftButtonUp(int x, int y) {
  OSTOOLBARWND_PARENT::onLeftButtonUp(x, y);
  if (m_hilite) {
    OSToolBarEntry *at = getEntryAt(x, y);
    if (m_hilite == at) {
      endCapture(); // in case event loads up a dialog ...
      if (!at->isDisabled()) {
        onClickIcon(at);
        at->onClicked();
        onLeaveIcon(at); // (for parity sake)
        beginCapture(); // ... we need to leave and restore icon states ...
        StdWnd::getMousePos(&x, &y);
        screenToClient(&x, &y);
        localMouseMove(x, y); // ... according to new mouse pos
      }
    }
  }

  m_down = 0;
  m_last_pushed = -1;

  return 1;
}

int OSToolBarWnd::onMiddleButtonDown(int x, int y) {
  if (!want_middle_clicks) return 0;

  return onLeftButtonDown(x, y);
}

int OSToolBarWnd::onMiddleButtonUp(int x, int y) {
  if (!want_middle_clicks) return 0;

  is_middle_click = 1;

  int r = onLeftButtonUp(x, y);

  is_middle_click = 0;

  return r;
}

void OSToolBarWnd::onClickIcon(OSToolBarEntry *entry) {
  if (m_cc && entry->getCommand() != -1) m_cc->onCommand(entry->getCommand());
  //DebugString("%s clicked\n", entry->getTooltip());
}

void OSToolBarWnd::onLeaveIcon(OSToolBarEntry *entry) {
  invalidateIcon(entry);
  delete m_tip;
  m_tip = NULL;
}

void OSToolBarWnd::onEnterIcon(OSToolBarEntry *entry) {
  invalidateIcon(entry);
  delete m_tip;
  m_tip = new OSToolTip(entry->getTooltip());
  m_tip->init(this);
}

