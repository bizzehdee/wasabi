#include <precomp.h>
#include "osstatusbarwnd.h"
#include "ostooltip.h"
#include "commandcb.h"

#ifndef CCS_BOTTOM
#define CCS_BOTTOM 0x00000003L
#endif

#ifndef SBARS_SIZEGRIP
#define SBARS_SIZEGRIP 0x0100
#endif

#define TIMER_BUTTON_L 1320
#define TIMER_BUTTON_M 1321
#define TIMER_BUTTON_R 1322
#define DBL_CLICKOFFSET 3

StatusBarPart::StatusBarPart(OSStatusBarWnd *parent) : m_width(-1), m_parent(parent), m_style(0), m_icon(NULL),
  m_data(0), m_dirty(1)
{
  for ( int i = 0; i < sizeof(m_commands)/sizeof(m_commands[0]); i++ )
    m_commands[i] = -1;
}

void StatusBarPart::copyFrom(StatusBarPart *part) {
  if ( !part ) return;
  for ( int i = 0; i < sizeof(m_commands)/sizeof(m_commands[0]); i++ )
    m_commands[i] = part->m_commands[i];
  m_text = part->m_text;
  m_style = part->m_style;
  m_tooltip = part->m_tooltip;
  m_width = part->m_width;
  m_icon = part->m_icon;
  m_data = part->m_data;
  m_dirty = 1;
}

void StatusBarPart::setWidth(int width) {
  if ( width != m_width ) {
    m_width = width;
    m_dirty = 1;
    m_parent->resizeParts();
    m_parent->updateStatusBar();
  }
}

void StatusBarPart::setText(const char *text) {
  if (!m_text.isequal(text)) {
    m_text = text;
    m_dirty = 1;
    m_parent->updateStatusBar();
  }
}

void StatusBarPart::setTooltip(const char *text) {
  if ( m_tooltip.notempty() && !text )
    text = "";
  if (!m_tooltip.isequal(text)) {
    m_tooltip = text;
    m_dirty = 1;
    m_parent->updateStatusBar();
  }
}

void StatusBarPart::setStyle(int style) {
  m_style = style;
  m_dirty = 1;
  m_parent->updateStatusBar();
}

void StatusBarPart::setIcon(HICON icon) {
  m_icon = icon;
  m_dirty = 1;
  m_parent->updateStatusBar();
}

void StatusBarPart::setCommand(int type, int id)  {
  if ( type >=0 && type < sizeof(m_commands)/sizeof(m_commands[0]) )
    m_commands[type] = id;
}

int StatusBarPart::getCommand(int type)  {
  if ( type >=0 && type < sizeof(m_commands)/sizeof(m_commands[0]) )
    return m_commands[type];
  return -1;
}

void StatusBarPart::render(HDC hdc,RECT* r,DWORD data) {
  if ( r ) { 
    RECT r1 = *r;
    int width = r1.right - r1.left;
    int level = width * data / 100;
    r1.right = r1.left + level;
    HBRUSH hbr = CreateSolidBrush (GetSysColor (COLOR_HIGHLIGHT));
    FillRect (hdc, (LPRECT)&r1, hbr);
    DeleteObject (hbr);
  }
}

void StatusBarPart::setData(DWORD value) { 
  if ( value != m_data ) {
    m_data = value; 
    m_dirty = 1;
    m_parent->updateStatusBar(); 
  }
}

DWORD StatusBarPart::getData() { return m_data; }

OSStatusBarWnd::OSStatusBarWnd() {
  INITCOMMONCONTROLSEX icce = { sizeof(INITCOMMONCONTROLSEX), ICC_WIN95_CLASSES };
  InitCommonControlsEx(&icce); 
  m_tooltipwnd = new OSToolTip();
  m_tooltipwnd->setParent(this);
  m_cc = NULL;
}

OSStatusBarWnd::~OSStatusBarWnd() {
  delete m_tooltipwnd;
  m_tooltipwnd = NULL;
  m_parts.deleteAll();
}

int OSStatusBarWnd::onInit() {
  OSSTATUSBARWND_PARENT::onInit();
  SetWindowLong(gethWnd(), GWL_STYLE, (GetWindowLong(gethWnd(), GWL_STYLE) /*CUT| SBT_NOBORDERS*/ | SBARS_SIZEGRIP | SBT_TOOLTIPS) & ~WS_CLIPCHILDREN);
  m_tooltipwnd->init(this);
  return 1;
}

LRESULT OSStatusBarWnd::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_GETFONT:
    case WM_GETTEXT:
    case WM_GETTEXTLENGTH:
    case WM_NCHITTEST:
    case WM_PAINT:
    case WM_SETFONT:
    case WM_SETTEXT:
    case WM_ERASEBKGND:
      return callDefProc(hWnd, uMsg, wParam, lParam);
    case WM_WINDOWPOSCHANGING:
    case WM_WINDOWPOSCHANGED:
    case WM_CREATE:
    case WM_DESTROY:
    case WM_SIZE:
      OSSTATUSBARWND_PARENT::wndProc(hWnd, uMsg, wParam, lParam);
      return callDefProc(hWnd, uMsg, wParam, lParam);
/* this can go away once doubleclick detection is fixed in basewnd */
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK: {
      int xPos, yPos;
      xPos = (signed short)LOWORD(lParam);
      yPos = (signed short)HIWORD(lParam);
      xPos = (int)((float)xPos / getRenderRatio());
      yPos = (int)((float)yPos / getRenderRatio());
      handleDblClicks(uMsg,xPos,yPos);
      return callDefProc(hWnd, uMsg, wParam, lParam);
                           }

  }
  return OSSTATUSBARWND_PARENT::wndProc(hWnd, uMsg, wParam, lParam);
}

StatusBarPart *OSStatusBarWnd::addNewPart() {
  return m_parts.addItem(new StatusBarPart(this));
}

void OSStatusBarWnd::removePart(StatusBarPart *part) {
  m_parts.deleteItem(part);
}

StatusBarPart *OSStatusBarWnd::enumPart(int n) {
  return m_parts.enumItem(n);
}

int OSStatusBarWnd::getNumParts() {
  return m_parts.getNumItems();
}

void OSStatusBarWnd::updateStatusBar() {
  if (!isInited() || m_inBulkUpdate ) return;

  //resizeParts();

  foreach(m_parts)
    if ( m_parts.getfor()->isDirty() ) {
    DWORD data =  m_parts.getfor()->getData();
  if (data)  
  SendMessage(gethWnd(), SB_SETTEXT,
      foreach_index | m_parts.getfor()->getStyle(),
      (LPARAM) data);
  else
  SendMessage(gethWnd(), SB_SETTEXT,
      foreach_index | m_parts.getfor()->getStyle(),
      (LPARAM) m_parts.getfor()->getText());

    HICON icon = m_parts.getfor()->getIcon();
    HICON curicon = (HICON) SendMessage(gethWnd(), SB_GETICON, foreach_index,0);
    if ( icon != curicon )
      SendMessage(gethWnd(), SB_SETICON, foreach_index,(LPARAM) icon);
    RECT r;
    const char * ttiptxt = m_parts.getfor()->getTooltip();
    SendMessage(gethWnd(), SB_GETRECT, foreach_index,(LPARAM) &r);
    m_tooltipwnd->setTool(foreach_index+1,&r,ttiptxt);
    m_parts.getfor()->clean();
    }
  endfor
  
}


void OSStatusBarWnd::resizeParts()  {

  int nparts = m_parts.n();

  MemBlock<int> array(nparts);

  RECT r;
  getClientRect(&r);
  int rgBorders[3];
  callDefProc(gethWnd(),SB_GETBORDERS, 0, (LPARAM)&rgBorders);

  int nbstrectched = 0;
  int availForStretch = r.right - r.left - 2 * rgBorders[0] + rgBorders[2];
  // add grip size
  availForStretch += rgBorders[0] + ::GetSystemMetrics(SM_CXVSCROLL) + ::GetSystemMetrics(SM_CXBORDER) * 2;
  foreach(m_parts)
    if( m_parts.getfor()->getStyle() & SBP_STRETCH )
      nbstrectched += 1;
    availForStretch -=  m_parts.getfor()->getWidth() + rgBorders[2];
  endfor

  int z = 0;
  foreach(m_parts)
    int width = m_parts.getfor()->getWidth();
    z += width;
    if( m_parts.getfor()->getStyle() & SBP_STRETCH && nbstrectched) {
      int strecthAmount = availForStretch / nbstrectched;
      z += strecthAmount;
      availForStretch -= width;
      nbstrectched -= 1;
    }
    array[foreach_index] = z;
  endfor
  
  SendMessage(gethWnd(), SB_SETPARTS, nparts, (LPARAM)array.getMemory());

}

void OSStatusBarWnd::setBulkUpdate(int b)  {
  if ( m_inBulkUpdate && !b )  { 
    m_inBulkUpdate = false;
    updateStatusBar();
  }
  else if ( b ) {
    m_inBulkUpdate = true;
  }
}


int OSStatusBarWnd::onResize() {
  resizeParts();
  foreach(m_parts)
    RECT r;
    const char * ttiptxt = m_parts.getfor()->getTooltip();
    if ( ttiptxt ) {
      SendMessage(gethWnd(), SB_GETRECT, foreach_index,(LPARAM) &r);
      m_tooltipwnd->setToolRect(foreach_index+1,&r);
    }
  endfor
  return TRUE;
}


int OSStatusBarWnd::handleDblClicks(UINT uMsg, int x, int y)  {
  if ( uMsg == WM_LBUTTONDBLCLK ) {
    onLeftButtonDblClk(x,y);
  } else if ( uMsg == WM_RBUTTONDBLCLK ) {
    onRightButtonDblClk(x,y);
  } else if ( uMsg == WM_MBUTTONDBLCLK ) {
    onMiddleButtonDblClk(x,y);
  }
  return 1;
}


int OSStatusBarWnd::onLeftButtonDown(int x, int y)  {
  killTimer(TIMER_BUTTON_L);
  lastClickP[0].x = x;
  lastClickP[0].y = y;
  setTimer(TIMER_BUTTON_L,StdWnd::getDoubleClickDelay()+2);
  return 0;
} 


int OSStatusBarWnd::onMiddleButtonDown(int x, int y)  {
  killTimer(TIMER_BUTTON_M);
  lastClickP[1].x = x;
  lastClickP[1].y = y;
  setTimer(TIMER_BUTTON_M,StdWnd::getDoubleClickDelay()+2);
  return 0;
} 


int OSStatusBarWnd::onRightButtonDown(int x, int y)  {
  killTimer(TIMER_BUTTON_R);
  lastClickP[2].x = x;
  lastClickP[2].y = y;
  setTimer(TIMER_BUTTON_R,StdWnd::getDoubleClickDelay()+2);  
  return 0;
} 


int OSStatusBarWnd::onLeftButtonDblClk(int x, int y)  {
  killTimer(TIMER_BUTTON_L);
  handleCommand(DBL_CLICKOFFSET,x,y);
  return 0;
} 


int OSStatusBarWnd::onMiddleButtonDblClk(int x, int y)  {
  killTimer(TIMER_BUTTON_M);
  handleCommand(TIMER_BUTTON_M- TIMER_BUTTON_L + DBL_CLICKOFFSET,x,y);
  return 0;
} 


int OSStatusBarWnd::onRightButtonDblClk(int x, int y)  {
  killTimer(TIMER_BUTTON_R);
  handleCommand(TIMER_BUTTON_R - TIMER_BUTTON_L + DBL_CLICKOFFSET,x,y);
  return 0;
} 


void OSStatusBarWnd::timerCallback(int id)  {
  if ( id  <= TIMER_BUTTON_R && id >= TIMER_BUTTON_L )  {
    killTimer(id);
    int type = id - TIMER_BUTTON_L;
    int x = lastClickP[type].x;
    int y = lastClickP[type].y;
    handleCommand(type,x,y);
    lastClickP[type].x = 0;
    lastClickP[type].y = 0;
  }
}


int OSStatusBarWnd::hitTest(int x,int y)  {
  RECT r;
  foreach(m_parts)
      SendMessage(gethWnd(), SB_GETRECT, foreach_index,(LPARAM) &r);
      if ( Std::pointInRect(&r,x,y) )
        return foreach_index;
  endfor
  return -1;
}

int OSStatusBarWnd::handleCommand(int type, int x, int y)  {
  int index = hitTest(x,y);
  if ( index != -1 && m_cc) {
    StatusBarPart * part = m_parts.enumItem(index);
    if ( part ) {
      int command = part->getCommand(type);
      m_cc->onCommand(command);
    }
  }
  return -1;
}

void OSStatusBarWnd::renderPart(int part,HDC hdc,RECT* r,DWORD data)  {
  StatusBarPart * targetpart = m_parts.enumItem(part);
  if ( targetpart )
    targetpart->render(hdc,r,data);

}
