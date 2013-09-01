#include "precomp.h"

#include <bfc/std.h>
#include <bfc/std_wnd.h>
#include <bfc/ptrlist.h>
#include <bfc/attrib/attribs.h>
#include <wnd/usermsg.h>
#include <wnd/notifmsg.h>
#include <wnd/accessible.h>
#include <wnd/region.h>

#include "listwnd.h"

#ifdef WASABI_COMPILE_SKIN
#include <api/skin/skinclr.h>
#endif

//CUT #include "../../studio/api.h"
//fixme #include "../../common/xlatstr.h"
#define _(x) (x)

#define DEF_TEXT_SIZE 14	// Default text size

#define ITEMLIST_INC	4092
#define LISTWND_DRAG_TIMERID 0x8972
#define LISTWND_DRAG_MARGIN 12
#define LISTWND_DRAG_TIMERDELAY 100
#define DRAGSKIP_START 5

#define X_SHIFT 0
#define Y_SHIFT 0
#define DRAG_THRESHOLD 4
#define COLUMNS_DEFAULT_HEIGHT 16
#define COLUMNS_DEFAULT_WIDTH  150
#define COLUMNS_MARGIN 2
#define COLUMNS_RESIZE_THRESHOLD 4
#define COLUMNS_MIN_WIDTH  1
#define COLSEPHEIGHT 1

#ifdef WASABI_COMPILE_SKIN
#include <api/skin/skinclr.h>
static SkinColor textColor("studio.list.text");// todo: have own color in skin.cpp
static SkinColor bgcolor("studio.list.background");// todo: have own color in skin.cpp
static SkinColor color_item_selected_fg("studio.list.item.selected.fg"); // RGB(0, 0, 128)
static SkinColor color_item_selected("studio.list.item.selected"); // RGB(0, 0, 128)
static SkinColor color_item_focused("studio.list.item.focused");// RGB(0, 128, 128)
static SkinColor color_item_focusrect("studio.list.item.focusrect");// RGB(0, 128, 128)
static SkinColor color_headers("studio.list.column.background");//RGB(0, 152, 208)
static SkinColor columnTextColor("studio.list.column.text");
static SkinColor columnSepColor("studio.list.column.separator");
#else
namespace {
_color textColor("studio.list.text", "#ffffff");// todo: have own color in skin.cpp
_color bgcolor("studio.list.background");// todo: have own color in skin.cpp
_color color_item_selected_fg("studio.list.item.selected.fg", "#ffffff"); // RGB(0, 0, 128)
_color color_item_selected("studio.list.item.selected", "#000020"); // RGB(0, 0, 128)
_color color_item_focused("studio.list.item.focused", "#309030");// RGB(0, 128, 128)
_color color_item_focusrect("studio.list.item.focused", "#ffffff");// RGB(0, 128, 128)
_color color_headers("studio.list.column.background", "#d0d8d0");//RGB(0, 152, 208)
_color columnTextColor("studio.list.column.text", "#000000");
_color columnSepColor("studio.list.column.separator", "#000000");
};
#endif

typedef struct {
  char *label;
  int column;
} listSubitemStruct;

class listItem {
friend ListWnd;
public:
  ListWnd *getList() const { return listwnd; }
protected:
  listItem(ListWnd *lw) : listwnd(lw) {
    data = 0;
    subitems = NULL;
  }
  ~listItem() {
    if (subitems != NULL) {
      for (int i=0;i<subitems->getNumItems();i++) {
        listSubitemStruct *subitem = subitems->enumItem(i);
        if (subitem->label)
          FREE(subitem->label);
      }
      subitems->freeAll();
      delete subitems;
    }
  }
//CUT  void setList(ListWnd *newlist) { listwnd=newlist; }

  String label;
  LPARAM data;
  PtrList<listSubitemStruct> *subitems;
  ListWnd *listwnd;
//fixme  AutoSkinBitmap icon;
};

class CompareListItem {
public:
  static int compareItem(listItem *p1, listItem *p2);
};

#define SELITEMEXPAND 2040

//note to whoever redid this as a bitlist
// a) this is pointlessly slow as a bitlist given the memory used
// b) perhaps you should investigate bitlist.h
class SelItemList {
public:
  SelItemList(ListWnd *parent) :
    listwnd(parent), list(SELITEMEXPAND), num_selected(0) { }

  void setSelected(int pos, int selected, int cb=1) {
    int s=list.getSize();
    if (pos >= s) {
      if (!selected) {
        // turning off something that's out of range is a no-brainer
        return;
      }
      int ns = ((pos / SELITEMEXPAND)+1)*SELITEMEXPAND;
      list.setSize(ns);
    }
    
    char *l=list.getMemory();
    if(selected) {
      // quick and dirty way to prevent more than one item from
      // ever being selected?
      if (listwnd->getPreventMultipleSelection()) {
        listwnd->deselectAll(cb);
      }
      if(!isSelected(pos)) {
        l[pos] = 1;
        num_selected++;
        if (cb) listwnd->itemSelection(pos, TRUE);
        listwnd->invalidateItem(pos);
      }
    } else {
      if(isSelected(pos)) {
        l[pos] = 0;
        num_selected--;
        if (cb) listwnd->itemSelection(pos, FALSE);
        listwnd->invalidateItem(pos);
      }
    }
  }
  int isSelected(int pos) {
    if (pos >= list.getSize()) return 0;
    return list.getMemory()[pos];
  }
  int getNumSelected() { return num_selected; }

  void deleteByPos(int pos) {
    ASSERT(pos >= 0);
    int s = list.getSize();
    if (pos >= s) return;
    num_selected -= isSelected(pos);
    char *m = list.getMemory()+pos;
    MEMMOVE(m, m+1, s-(pos+1));
  }
protected:
friend ListWnd;
  void deselectAll() {	// for internal use, doesn't send callbacks
    list.zeroMemory();
    num_selected = 0;
  }

private:
  ListWnd *listwnd;
  MemBlock<char> list;
  int num_selected;
};

int CompareListItem::compareItem(listItem *p1, listItem *p2) {
  return p1->getList()->sortCompareItem(p1, p2);
}

ListWnd::ListWnd() {
#ifdef WASABI_COMPILE_SKIN
  if (WASABI_API_SKIN->skin_getVersion() >= 1) {
    textColor.setElementName("wasabi.list.text");
    textColor.setColorGroup("");
    bgcolor.setElementName("wasabi.list.background");
    bgcolor.setColorGroup("");
    color_item_selected_fg.setElementName("wasabi.list.text.selected");
    color_item_selected_fg.setColorGroup("");
    color_item_selected.setElementName("wasabi.list.text.selected.background");
    color_item_selected.setColorGroup("");
    color_headers.setElementName("wasabi.list.column.background");
    color_headers.setColorGroup("");
    columnTextColor.setElementName("wasabi.list.column.text");
    columnTextColor.setColorGroup("");
    columnSepColor.setElementName("wasabi.list.column.frame.bottom");
    columnSepColor.setColorGroup("");
    color_item_focused.setColorGroup("");
    color_item_focusrect.setColorGroup("");
  }
#endif
  selectonupdown = 1;
  setAutoSort(FALSE);
  setOwnerDraw(FALSE);
  setSortDirection(0);
  setSortColumn(0);
  lastcolsort=-1;
  dragtimeron = 0;
  dragskip = DRAGSKIP_START;
  dragskipcount = 0;
  item_invalidate_border = 0;

  selItemList = new SelItemList(this);
	metrics_ok = FALSE;
  itemExtraHeightTop = 0;
  itemExtraHeightBot = 0;
  setFontSize(DEF_TEXT_SIZE);	// sets itemHeight
  redraw = TRUE;
  columnsHeight = COLUMNS_DEFAULT_HEIGHT;
  lastItemFocused = NULL;
  lastItemFocusedPos = -1;
  lastAddedItem = NULL;
  selectionStart = -1;
  colresize = -1;
  resizing_col = FALSE;
  processbup = FALSE;
  bdown = FALSE;
  nodrag = FALSE;
  showColumnsHeaders = TRUE;
  rectselecting=0;
  preventMultipleSelection = 0;
  wantautodeselect = 1;
  registerAcceleratorSection("listwnd");
  hoverselect = 0;
  firstItemVisible = -1;
  lastItemVisible = -1;
  showicons = 0;
  listwnd_middle_button_selects=TRUE;
  listwnd_want_smoothscroll=FALSE;
}

ListWnd::~ListWnd() {
  deleteAllItems();
  columnsList.deleteAll();
  delete selItemList;
  nodrag=FALSE;
}

int ListWnd::onInit() {

  LISTWND_PARENT::onInit();

  setBgBitmap("wasabi.list.background");
  setLineHeight(itemHeight);

  return 1;
}

int ListWnd::onPostOnInit() {
  LISTWND_PARENT::onPostOnInit();
  recalcHeaders();
  return 1;
}

int ListWnd::onPaint(Canvas *canvas) {

  PaintCanvas paintcanvas;
  PaintBltCanvas paintbcanvas;

  if (canvas == NULL) {
    if (needDoubleBuffer()) {
      if (!paintbcanvas.beginPaintNC(this)) return 0;
      canvas = &paintbcanvas;
    } else {
      if (!paintcanvas.beginPaint(this)) return 0;
      canvas = &paintcanvas;
    }
  }

  LISTWND_PARENT::onPaint(canvas);

  RegionI *clip = new RegionI();
  canvas->getClipRgn(clip); 
  Region *orig;
  orig = clip->clone();

  drawColumnHeaders(canvas);

  if (getColumnsHeight() > 0) {
    RECT cr;
    getClientRect(&cr);
    cr.bottom = cr.top;
    cr.top -= getColumnsHeight();
    RegionI *colReg = new RegionI(&cr);
    clip->subtractRegion(colReg);
    canvas->selectClipRgn(clip);
    delete colReg;
  }

  canvas->setTextOpaque(FALSE);
  canvas->pushTextSize(getFontSize());

  firstItemVisible = -1;
  lastItemVisible = -1;

  //drawSubItems(canvas, x, &y, items, r.top, r.bottom, 0);
  drawItems(canvas);

  canvas->popTextSize();

  canvas->selectClipRgn(orig); // reset cliping region;

  clip->disposeClone(orig);
  delete clip;

  return 1;
}

int ListWnd::onResize() {
  LISTWND_PARENT::onResize();
  recalcHeaders();
  return 1;
}

int ListWnd::getNumItems(void) {
  return itemList.getNumItems();
}

void ListWnd::drawItems(Canvas *canvas) {

  RECT cr, clipbox;
  getClientRect(&cr);

  //CUT RegionI *clip = new RegionI();
  RegionI clip;
  if (!canvas->getClipRgn(&clip)) {
    clip.setRect(&cr);
  }

  if (GetClipBox(canvas->getHDC(), &clipbox) == NULLREGION) {
    getClientRect(&clipbox);
  }

//  DebugString("%d %d %d %d\n", c.left, c.top, c.right, c.bottom);

//  float f,l;
  calcBounds();

  //int first, last;

/*  RECT s=c;

  s.bottom = min(s.bottom, r.bottom);
  s.top = max(s.top, getLabelHeight()+(showColumnsHeaders ? getColumnsHeight() : 0));

  f = ((float)(s.top - getLabelHeight() - (showColumnsHeaders ? getColumnsHeight() : 0) - getYShift() + getScrollY())) / (float)itemHeight;
  l = ((float)(s.bottom - getLabelHeight() - (showColumnsHeaders ? getColumnsHeight() : 0) - getYShift() + getScrollY())) / (float)itemHeight;
  first = (int)f;
  first = max(0,first);
  last = (int)l;
  if ((float)((int)l) != l) {
    last++;
  }*/

//CUT  int g=0;

  for (int i=firstItemVisible;i<=lastItemVisible && i<itemList.getNumItems();i++) {
    RECT ir={0,0,0,0};
    if (!getItemRect(i, &ir)) continue;
    int sav=ir.right;
    ir.right = cr.right;
    if (!clip.intersectRect(&ir)) 
      continue;
    ir.right=sav;
//CUT    g++;

    LPARAM itemdata = getItemData(i);
    int itemselected = getItemSelected(i);
    int itemfocused = getItemFocused(i);

    onPreItemDraw(canvas, i, &ir, itemdata, itemselected, itemfocused);

    int sel = getItemSelected(i);
    canvas->pushPen(PS_SOLID, 1, getColumnSepColor());

    if (!ownerDraw(canvas, i, &ir, itemdata, itemselected, itemfocused)) {
      canvas->pushTextBold(getTextBold(itemdata));
      canvas->pushTextItalic(getTextItalic(itemdata));
      canvas->setTextOpaque(FALSE);
      canvas->pushTextColor(sel ? getSelFgColor(itemdata) : getTextColor(itemdata));
      canvas->pushTextSize(getFontSize());
      int x = -getScrollX();

      if (getItemFocused(i)) {
        canvas->fillRect(&ir, getFocusColor(itemdata) | 0xFF000000);
      } else if (sel) {
        canvas->fillRect(&ir, getSelBgColor(itemdata) | 0xFF000000);
      }

      if (needFocusRect(itemdata)) 
        canvas->drawRect(&ir, 1, getFocusRectColor(itemdata) | 0xFF000000);

      if (showicons) {
//fixme        SkinBitmap *icon = getItemIcon(i);
//fixme        if (icon) {
//fixme          RECT dst={x+X_SHIFT+r.left+1, ir.top+1, x+X_SHIFT+r.left+itemHeight-1, ir.top+itemHeight-1};
//fixme          icon->stretchToRect(canvas, &dst);
//fixme        }
        x += itemHeight;
      }
      int xsep = wantColSepOnItems()?COLSEPHEIGHT:0;
      for (int j=0;j<columnsList.getNumItems();j++) {
        ListColumn *col = columnsList[j];
        RECT cr=ir;
        cr.left = x+X_SHIFT+clipbox.left;
        cr.right = cr.left + col->getWidth()-X_SHIFT*2+xsep;

        int cb = getColumnBold(itemdata, j);
        if (cb != -1) canvas->pushTextBold(cb);

        const char *txt = getSubitemText(i, j);
        char ptxt[1024]="";
        if (txt == NULL) {
          LPARAM data = getItemData(i);
          if (j == 0) txt = convertlParam(data);
          else {
            convertlParamColumn(j, i, data, ptxt, 1023);
            ptxt[1023]='\0';
            txt = ptxt;
          }
        }

        if (!ownerDrawColumn(canvas, i, &cr, j, itemdata, itemselected, itemfocused, txt)) {
          if (j > 0 && wantColSepOnItems()) {
            canvas->moveTo(x, ir.top);
            canvas->lineTo(x, ir.top+itemHeight);
          }
          switch (col->getAlignment()) {
            case COL_LEFTALIGN:
            canvas->textOutEllipsed(cr.left+2, cr.top, cr.right-cr.left-4, cr.bottom-cr.top, txt);
            break;
            case COL_CENTERALIGN: {
              RECT _cr = {cr.left+2, cr.top, cr.right-4, cr.bottom};
              canvas->textOutCentered(&_cr, txt);
            }
            break;
            case COL_RIGHTALIGN: {
              int __x = cr.left;
              int __y = cr.top;
              int fw = canvas->getTextWidth(txt);
              int aw = cr.right-cr.left-4;
              __x -= fw-aw;
              canvas->textOut(__x, __y, txt);
            }
            break;
          }
        }//owner draw col

        x += col->getWidth()+xsep;

        if (wantColSepOnItems()) {
          canvas->moveTo(x, ir.top);
          canvas->lineTo(x, ir.top+itemHeight);
        }
        if (cb != -1) canvas->popTextBold();
      } // draw column data
      canvas->popTextSize();
      canvas->popTextColor();
      canvas->popTextBold();
      canvas->popTextItalic();
    }

    canvas->popPen();

    onPostItemDraw(canvas, i, &ir, itemdata, itemselected, itemfocused);
  }
//CUT/*
//CUT  OutputDebugString("%d items draw\n", g);
//CUT*/
//CUT  delete clip;
}

int ListWnd::wantColSepOnItems() {
  return 0;
}

int ListWnd::getXShift() {
  if (wantColSepOnItems()) return X_SHIFT; else return 0;
}

int ListWnd::getFirstItemSelected() {
  return getNextItemSelected(-1);
}

int ListWnd::getNextItemSelected(int lastpos) {
  if (lastpos < -1) lastpos = -1;
  for (int i=lastpos+1;i<itemList.getNumItems();i++)
    if (getItemSelected(i))
      return i;
  return -1;
}

void ListWnd::calcBounds() {
  lastComplete = TRUE;
  firstComplete = TRUE;
  float f,l;
  RECT r;
  getClientRect(&r);

  f = ((float)(getScrollY()-Y_SHIFT)  / itemHeight);
  l = ((float)(getScrollY()-Y_SHIFT+(r.bottom-r.top)) / itemHeight) - 1.0f;

  firstItemVisible = (int)f;
  lastItemVisible = (int)l;

  if ((float)((int)l) != l) {
    lastItemVisible++;
    lastComplete = FALSE;
  }
  if ((float)((int)f) != f && f >= 0) {
    firstComplete = FALSE;
  }
}

// Draws tiled background
void ListWnd::drawBackground(Canvas *canvas) {
  LISTWND_PARENT::drawBackground(canvas);
  drawColumnHeaders(canvas);
}

void ListWnd::drawColumnHeaders(Canvas *c) {

  if (columnsList.getNumItems() == 0 || !showColumnsHeaders) return;

  RECT r;
  getClientRect(&r);
  r.top -= getColumnsHeight();
  r.bottom = r.top + getColumnsHeight();
  if (renderRatioActive())
    r.left -= (int)((double)getScrollX()*getRenderRatio());
  else
    r.left-=getScrollX();

  c->fillRect(&r, color_headers | 0xFF000000);
  int x = r.left + X_SHIFT/*+ 1*/;

  if (showicons) 
    x += itemHeight + 2;

  c->pushTextColor(columnTextColor);
  c->setTextOpaque(FALSE);
  c->pushTextSize(getColumnsHeight());
  c->pushTextBold(TRUE);
  c->pushPen(PS_SOLID, 1, getColumnSepColor());

  int start = 0;

  for (int i=0;i<columnsList.getNumItems();i++) {
    ListColumn *col = columnsList[i];
    int width = col->getWidth();
    if (i > 0) {
      c->moveTo(x, r.top);
      c->lineTo(x, r.top+getColumnsHeight());
    }
    RECT ch;
    ch.left = x+COLUMNS_MARGIN-((i==0)?X_SHIFT:0);
    ch.top = r.top;
    ch.right = ch.left + col->getWidth()-1-COLUMNS_MARGIN*2+((i==0)?X_SHIFT:0);
    ch.bottom = ch.top + getColumnsHeight()-1;
    col->customDrawHeader(c, &ch);
    x+=width/*+1*/;
  }
  c->moveTo(x, r.top);
  c->lineTo(x, r.top+getColumnsHeight());
  c->popPen();

  c->popTextSize();
  c->popTextColor();
  c->popTextBold();
}

COLORREF ListWnd::getColumnSepColor() {
  return columnSepColor;
}

int ListWnd::getHeaderHeight() {
  return (showColumnsHeaders && columnsList.getNumItems() > 0) ? getColumnsHeight() : 0;
}

// Returns the current tree width in pixels
int ListWnd::getContentsWidth() {
  return getColumnsWidth()+X_SHIFT;
}

// Returns the current tree height in pixels
int ListWnd::getContentsHeight() {
  return itemList.getNumItems()*(itemHeight+(wantColSepOnItems()?COLSEPHEIGHT:0))+Y_SHIFT*2;
}

void ListWnd::setAutoSort(int dosort) {
  autosort = dosort;
  itemList.setAutoSort(dosort);
}

void ListWnd::setOwnerDraw(int doownerdraw) {
  ownerdraw = doownerdraw;
}

void ListWnd::setItemHeight(int h) {
  itemHeight = h;
}

void ListWnd::setItemExtraHeight(int top, int bot) {
  itemExtraHeightTop = top;
  itemExtraHeightBot = bot;
}

int ListWnd::setFontSize(int size) {
  LISTWND_PARENT::setFontSize(size);
  if (size >= 0) textsize = size;
#ifdef WASABI_COMPILE_FONTS
  SysCanvas c;
  c.pushTextSize(getFontSize());
  itemHeight = c.getTextHeight()+2+itemExtraHeightTop+itemExtraHeightBot;
  c.popTextSize();
#else
  itemHeight = 10+itemExtraHeightTop+itemExtraHeightBot;	// usable default
#endif
  redraw = TRUE;
  metrics_ok = FALSE;
  invalidate();
  return 1;
}

int ListWnd::getFontSize() {
#ifndef WASABINOMAINAPI
  return textsize+api->metrics_getDelta();
#else
  //MULTIAPI-FIXME: not handling delta
  return textsize;
#endif
}

void ListWnd::onSetVisible(int show) {
  LISTWND_PARENT::onSetVisible(show);
  if (show) invalidate();
}

int ListWnd::fullyVisible(int pos) {
  return ( ( (lastComplete && pos <= lastItemVisible)
             ||
             (!lastComplete && pos < lastItemVisible)
           )
           &&
           (
             (firstComplete && pos >= firstItemVisible)
             ||
             (!firstComplete && pos > firstItemVisible)
           )
         );
}

void ListWnd::ensureItemVisible(int pos1, int pos2) {// FIX pos2
  int pos = pos1;
  if (pos >= itemList.getNumItems()) pos = itemList.getNumItems()-1;
  if (pos < 0) pos = 0;
#if 1
  if (fullyVisible(pos)) return;
#endif

  RECT c = clientRect();
  int y=pos*itemHeight;	// y is topleft of item

#if 1
  int showing_height = c.bottom - c.top;

  if (getScrollY() < y)	// scrolling up
    y = (y - showing_height) + itemHeight*3;
  else
    y -= itemHeight*2;

  if (y < 0) y = 0;
  else if (y + showing_height > getContentsHeight()) {
    // just show bottom pane
    y = getContentsHeight()-showing_height;
  }
#endif

  if (listwnd_want_smoothscroll)
    smoothScrollToY(y);	// smoooth
  else
    scrollToY(y);
}

void ListWnd::scrollToItem(int pos) {
  if (!isInited()) return;
  scrollToY(Y_SHIFT+pos*(itemHeight+(wantColSepOnItems()?COLSEPHEIGHT:0)));
}

int ListWnd::getNumColumns() {
  return columnsList.getNumItems();
}

ListColumn *ListWnd::getColumn(int n) {
  return columnsList.enumItem(n);
}

int ListWnd::getColumnWidth(int c) {
  ListColumn *col = columnsList[c];
  ASSERT(col != NULL);
  return col->getWidth();
}

int ListWnd::selectAll(int cb) {

  int i;

  if (preventMultipleSelection) return 1;

  for (i=0;i<itemList.getNumItems();i++) {
    selItemList->setSelected(i, TRUE, cb);
  }

  if (cb)
    notifySelChanged();
  return 1;
}

int ListWnd::deselectAll(int cb) {

//BU sez CUT: why should deselect all remove focus as well?
//  int lif = getItemFocused();
//  if (lif != -1) 
//    invalidateItem(lif);
//  lastItemFocused = NULL;

  for (int i = 0; i < itemList.getNumItems(); i++) {
    selItemList->setSelected(i, FALSE, cb);
  }

  if (cb)
    notifySelChanged();
  return 1;
}

void ListWnd::notifySelChanged(int item, int sel) {
  if (!getRedraw()) return;
  if (item == -1)
    notifyParent(ChildNotify::LISTWND_SELCHANGED);
  else
    notifyParent(ChildNotify::LISTWND_ITEMSELCHANGED, item, sel);
}

int ListWnd::invertSelection(int cb) {
  if (preventMultipleSelection) return 1;
  for (int i = 0; i < itemList.getNumItems(); i++) {
    toggleSelection(i, FALSE, cb);
  }
  return 1;
}

int ListWnd::invalidateItem(int pos) {
  RECT r;
  if (!isInited()) return 0;
  int rv = getItemRect(pos, &r);
  r.top -= item_invalidate_border;
  r.bottom += item_invalidate_border;
  if (rv)
    invalidateRect(&r);
  return rv;
}

int ListWnd::onLeftButtonDblClk(int x, int y) {
  // check for column dblclick
  int colhit;
  if ((colhit = hitTestColumns(Std::makePoint(x, y))) >= 0) {
    return onColumnDblClick(colhit, x, y);
  }

  if (itemList.getNumItems() == 0) return 0;
  POINT p={x,y};
  int i = hitTest(p);
  if (i > -1) {
    notifyParent(ChildNotify::LISTWND_DBLCLK, i, 0);
    onDoubleClick(i);
    return 1;
  }
  return 0;
}

int ListWnd::onMiddleButtonDown(int x, int y) {
  nodrag=TRUE;
  return 1;
}

int ListWnd::onMiddleButtonUp(int x, int y) {
  nodrag=FALSE;
  int i = hitTest(x,y);
  if (i >= 0) {	// did hit something
    if (listwnd_middle_button_selects) {
      setItemFocused(i);	// it always gets the focus
      if (!getItemSelected(i)) {	// reselect the item out of the cur selection
        ListWnd::onLeftButtonDown(x,y); // don't call inherited!
        ListWnd::onLeftButtonUp(x,y); // don't call inherited!
      }
    }
    onMiddleClick(i);
  } else {
    if (wantAutoDeselect())
      deselectAll();
  }
  
  return 1;
}

int ListWnd::onRightButtonDown(int x, int y) {
  nodrag=TRUE;
  return 1;
}

int ListWnd::onRightButtonUp(int x, int y) {
  nodrag=FALSE;
  int i = hitTest(x,y);
  if (i >= 0) {	// did hit something
    setItemFocused(i);	// it always gets the focus
    if (!getItemSelected(i)) {	// reselect the item out of the cur selection
      ListWnd::onLeftButtonDown(x,y); // don't call inherited!
      ListWnd::onLeftButtonUp(x,y); // don't call inherited!
    }
    onRightClick(i);
  } else {
    if (wantAutoDeselect())
      deselectAll();
  }
  onContextMenu(x,y);
  
  return 1;
}

int ListWnd::onLeftButtonDown(int x, int y) {
  if (colresize != -1) {
    resizing_col = TRUE;
    drawXorLine(colresizept.x);
  }

  processbup = FALSE;
  bdown = TRUE;
  bdownx = x;
  bdowny = y;

  if (!resizing_col) {

    POINT p={x,y};
    int i = hitTest(p);
    if (i >= 0) {
      if (Std::keyDown(VK_SHIFT)) {
        if (getItemSelected(i))
          processbup=TRUE;
        else
          setSelectionEnd(i);
        } else 
          if (Std::keyDown(VK_CONTROL)) {
            if (getItemSelected(i))
              processbup=TRUE;
            else
              toggleSelection(i);
          } else {
            if (getItemSelected(i))
              processbup = TRUE;
            else
              setSelectionStart(i);
          } 
    } else {
      if (wantAutoDeselect())
        deselectAll();
      /*rectselecting=1;
      selectStart.x = x;
      selectStart.y = y;
      selectLast.x = x;
      selectLast.y = y;
      drawRect(selectStart.x, selectStart.y, x, y);
      beginCapture();*/
    }
  }

  return 1;
}

int ListWnd::onLeftButtonUp(int x, int y) {

  bdown = FALSE;

  if (resizing_col) {
    resizing_col = FALSE;
    drawXorLine(colresizept.x);
    calcNewColWidth(colresize, colresizept.x);
    recalcHeaders();
    return 1;
  }

  // check for column label click
  int colhit;
  if ((colhit = hitTestColumnsLabel(Std::makePoint(x, y))) >= 0) {
    ListColumn *lc = getColumn(colhit);
    int ret = lc->onHeaderClick();
    if (!ret) ret = onColumnLabelClick(colhit, x, y);
    return ret;
  }

  POINT p={x,y};
  int i = hitTest(p);

  if (rectselecting || (processbup && !resizing_col) || hoverselect) {
    if (i >= 0) {
      if (Std::keyDown(VK_SHIFT)) {
        if (getItemSelected(i))
          setSelectionStart(i);
        else
          setSelectionEnd(i);
      } else {
        if (Std::keyDown(VK_CONTROL) || !wantAutoDeselect()) {
          toggleSelection(i);
          selectionStart = i;
        } else {
          setSelectionStart(i);
        } 
      }
    } else {
/*      if (rectselecting) {
        drawRect(selectStart.x, selectStart.y, selectLast.x, selectLast.y);
        endCapture();
        rectselecting=0;
        selectRect(selectStart.x, selectStart.y, x, y);
      } else*/
        if (wantAutoDeselect())
          deselectAll();
    }
  }

  if (i >= 0) {
    int cn = hitTestColumnClient(x);
    int r = 0;
    if (cn >= 0) {
      ListColumn *lc = getColumn(cn);
      ASSERT(lc != NULL);
      r = lc->onColumnLeftClick(i);
    }
    if (!r) onLeftClick(i);
  }

  return 1;
}

int ListWnd::onMouseMove(int x, int y) {
  LISTWND_PARENT::onMouseMove(x,y);

  if (!bdown && (Std::keyDown(MK_RBUTTON) || Std::keyDown(MK_LBUTTON))) {
    bdown = TRUE;
    processbup = TRUE;
    _enterCapture();
  }

  if (!rectselecting && bdown && !resizing_col && !nodrag && (ABS(x-bdownx) >= 4 || ABS(y-bdowny) >= 4)) {
    processbup = FALSE;
    bdown = FALSE;
    int i = hitTest(x, y);
    if (i != -1) {
      onBeginDrag(i);
      return 1;
    }
  }

/*  if (rectselecting) {
    drawRect(selectStart.x, selectStart.y, selectLast.x, selectLast.y);
    selectLast.x = x;
    selectLast.y = y;
    drawRect(selectStart.x, selectStart.y, selectLast.x, selectLast.y);
    return 1;
  }*/

  POINT p={x,y};
  if (wantResizeCols()) {
    if (!resizing_col) {
      int c = hitTestColumns(p, &colresizeo);
      if (c != -1) {
        if (colresize != c) {
          SetCursor(LoadCursor(NULL, IDC_SIZEWE)); // NONPORTABLE
          if (!getCapture())
            beginCapture();
          colresize = c;
          colresizept = p;
        }
      } else {
        if (colresize != -1) {
          SetCursor(LoadCursor(NULL, IDC_ARROW)); // NONPORTABLE
          endCapture();
          colresize = -1;
        }
      }
    } else {
      if (p.x + getScrollX() < colresizeo + COLUMNS_MIN_WIDTH) {
        p.x = colresizeo + COLUMNS_MIN_WIDTH - getScrollX();
      }
      drawXorLine(colresizept.x);
      colresizept = p;
      drawXorLine(colresizept.x);
    }
  }

  if (hoverselect) {
    int i = hitTest(x, y);
    if (i >= 0 && !getItemSelected(i)) {
      deselectAll(0);
      setSelected(i, 1, 0);
      char t[256]="";
      getItemLabel(i, 0, t, 255);
      foreach(tempselectnotifies)
        sendAction(tempselectnotifies.getfor(), "tempselectnotify", t);
      endfor;
    }
  }
  return 1;
}

// NONPORTABLE / Todo: implement necessary stuff in canvas
void ListWnd::drawXorLine(int x) {
  HDC dc = GetDC(gethWnd());
	HBRUSH brush = CreateSolidBrush(0xFFFFFF);
	HPEN pen = CreatePen(PS_SOLID,0,0xFFFFFF);
	HBRUSH oldB = (HBRUSH)SelectObject(dc, brush);
	HPEN oldP = (HPEN)SelectObject(dc, pen);
	int mix = SetROP2(dc,R2_XORPEN);

	RECT r;
	getClientRect(&r);
  r.top -= getColumnsHeight();
  r.bottom = r.top + getColumnsHeight();

  if (renderRatioActive()) {
    multRatio(&x);
    multRatio(&r);
  }

  MoveToEx(dc, x, r.top, NULL);
  LineTo(dc, x, r.bottom);

	SelectObject(dc, oldB);
	SelectObject(dc, oldP);
	SetROP2(dc, mix);
	DeleteObject(pen);
	DeleteObject(brush);
	ReleaseDC(gethWnd(), dc);
}

void ListWnd::calcNewColWidth(int c, int px) {
  RECT r;
	getClientRect(&r);
  r.top -= getColumnsHeight();
  r.bottom = r.top + getColumnsHeight();
  px += getScrollX();
  int x = r.left+X_SHIFT;
  for (int i=0;i<columnsList.getNumItems();i++) {
    ListColumn *col = columnsList[i];
    if (col->getIndex() == c) {
      int w = px - x;
      col->setWidth(w);
      setSlidersPosition();
      return;
    }
    x += col->getWidth();  
  }
  return;
}

int ListWnd::hitTestColumns(POINT p, int *origin) {
  RECT r;
  if (!showColumnsHeaders) return -1;
  int best=-1, besto, bestd=9999;
	getClientRect(&r);
  r.top -= getColumnsHeight();
  r.bottom = r.top + getColumnsHeight();
  p.x += getScrollX();
  if (p.y > r.top && p.y < r.top + getColumnsHeight()) {
    int x = r.left+X_SHIFT;
    for (int i=0;i<columnsList.getNumItems();i++) {
      ListColumn *col = columnsList[i];
      x += col->getWidth();  
      if (p.x > x-COLUMNS_RESIZE_THRESHOLD && p.x < x+COLUMNS_RESIZE_THRESHOLD) {
        int d = ABS(p.x-x);
        if (d < bestd) {
          bestd = d;
          besto = x - col->getWidth();
          best = col->getIndex();
        }
      }
    }
  }

  if (best != -1)
    if (origin != NULL) *origin = besto;
  return best;
}

int ListWnd::hitTestColumnClient(int x) {
  RECT cr = clientRect();
  int x1 = cr.left+X_SHIFT;
  foreach(columnsList)
    int x2 = x1 + columnsList.getfor()->getWidth();
    if (x >= x1 && x <= x2) return foreach_index;
    x1 = x2;
  endfor
  return -1;
}

void ListWnd::invalidateColumns() {
  RECT r;
	getClientRect(&r);
  r.top -= getColumnsHeight();
  r.bottom = r.top + getColumnsHeight();
  invalidateRect(&r);
}

int ListWnd::hitTestColumnsLabel(POINT p) {
  RECT r;
  if (!showColumnsHeaders) return -1;
	getClientRect(&r);
  r.top -= getColumnsHeight();
  r.bottom = r.top + getColumnsHeight();
  p.x += getScrollX();
  if (p.y > r.top && p.y < r.top + getColumnsHeight()) {
    int x = X_SHIFT;
    for (int i=0;i<columnsList.getNumItems();i++) {
      ListColumn *col = columnsList[i];
      if (p.x >= x && p.x <= x+col->getWidth())
        return i;
      x += col->getWidth();  
    }
  }
  return -1;
}

void ListWnd::setSelectionStart(int pos, int wantcb) {
  if (wantAutoDeselect())
    deselectAll(wantcb);
  if (!selItemList->isSelected(pos)) {
    selItemList->setSelected(pos, TRUE, wantcb);
    lastItemFocused = itemList[pos];
    lastItemFocusedPos = pos;
    invalidateItem(pos);
    repaint();
  }

  selectionStart = pos;
  notifySelChanged();
}

void ListWnd::setSelectionEnd(int pos) {

  if (itemList.getNumItems() == 0) return;
  if (selectionStart == -1) selectionStart = 0;

  if (wantAutoDeselect())
    deselectAll();

  int inc = (selectionStart > pos) ? -1 : 1;
  int i = selectionStart;

  while (1) {
    if (!selItemList->isSelected(i)) {
      selItemList->setSelected(i, TRUE);
      lastItemFocused = itemList[i];
      lastItemFocusedPos = i;
      invalidateItem(i);
    }
    if (i == pos) break;
    i += inc;
  }
  notifySelChanged();
}

void ListWnd::setSelected(int pos, int selected, int cb) {
  selItemList->setSelected(pos, selected, cb);
}

void ListWnd::toggleSelection(int pos, int setfocus, int cb) {
  if (!selItemList->isSelected(pos)) {
    selItemList->setSelected(pos, TRUE, cb);
    if (setfocus) {
      if (selItemList->getNumSelected() > 1) {
        for (int i=0;i<itemList.getNumItems();i++)
          if (selItemList->isSelected(i))
            invalidateItem(i);
      }
      lastItemFocused = itemList[pos];
      lastItemFocusedPos = pos;
    }
  } else {
    selItemList->setSelected(pos, FALSE, cb);
    if (setfocus) {
      lastItemFocused = NULL;
      lastItemFocusedPos = -1;
    }
  }

  invalidateItem(pos);
  if (cb)
    notifySelChanged();
}

int ListWnd::onBeginDrag(int iItem) {
  // nothing by default
  lastItemFocused = NULL;
  lastItemFocusedPos = -1;
  invalidateItem(iItem);
  return 0;
}

int ListWnd::dragOver(int x, int y, RootWnd *sourceWnd) {
  int rt = LISTWND_PARENT::dragOver(x, y, sourceWnd);
  if (dragtimeron) return rt;

  POINT pos={x,y};
  screenToClient(&pos); 

  int item = hitTest(pos);
  if (item == LW_HT_BELOW || item == LW_HT_ABOVE) {
    dragtimeron = 1;
    dragskip = DRAGSKIP_START;
    dragskipcount = DRAGSKIP_START-1; // start right away
    setTimer(LISTWND_DRAG_TIMERID, LISTWND_DRAG_TIMERDELAY);
  }
  return rt;
}

void ListWnd::onDragTimer() {
  POINT pos;
  StdWnd::getMousePos(&pos);
  screenToClient(&pos); 
  int item = hitTest(pos);
  if (item == LW_HT_BELOW || item == LW_HT_ABOVE) {
    dragskipcount++;
    if (dragskipcount >= dragskip) {
      switch (item) {
        case LW_HT_BELOW:
          scrollDown();
          break;
        case LW_HT_ABOVE:
          scrollUp();
          break;
      }
      dragskipcount = 0;
      if (dragskip > 0) dragskip--;
    }
  } else {
    killTimer(LISTWND_DRAG_TIMERID);
    dragtimeron = 0;
  }
}

void ListWnd::timerCallback(int id) {
  switch (id) {
    case LISTWND_DRAG_TIMERID:
      onDragTimer();
      return;
  }
  LISTWND_PARENT::timerCallback(id);
}

void ListWnd::onSelectAll() {
}

int ListWnd::scrollAbsolute(int x) {
  scrollToX(x);
  return getScrollX();
}

int ListWnd::scrollRelative(int x) {
  scrollToX(getScrollX() + x);
  return getScrollX();
}

int ListWnd::getItemFocused() {
  if (lastItemFocused == NULL) return -1;
  if (itemList[lastItemFocusedPos] == lastItemFocused)
    return lastItemFocusedPos;
  else {
    lastItemFocusedPos = itemList.searchItem(lastItemFocused);
    return lastItemFocusedPos;
  }
}

int ListWnd::getFocusedItem() {
  return getItemFocused(); // for now, need to move code over
}

void ListWnd::setItemFocused(int pos, int ensure_visible) {
  invalidateItem(lastItemFocusedPos);
  lastItemFocused = itemList[pos];
  lastItemFocusedPos = -1;
  if (lastItemFocused != NULL) lastItemFocusedPos = pos;
  if (lastItemFocusedPos != -1) {
    invalidateItem(lastItemFocusedPos);
    onItemSetFocused(pos);
    if (ensure_visible) ensureItemVisible(pos);
  }
  // quick and dirty way to prevent more than one item from
  // ever being selected?
  if (getPreventMultipleSelection()) deselectAll();
}

int ListWnd::getItemRect(int pos, RECT *r) {
  MEMSET(r, 0, sizeof(RECT));
  if (pos < 0 || pos >= itemList.getNumItems()) return 0;
  RECT cr=clientRect();
  r->left = -getScrollX() + X_SHIFT + cr.left;
  r->right = cr.left + getColumnsWidth();
  if (showicons) r->right += itemHeight;
  r->top = -getScrollY() + pos * (itemHeight + (wantColSepOnItems() ? COLSEPHEIGHT : 0)) + Y_SHIFT + cr.top;
  r->bottom = r->top + itemHeight;

  // clip!
  if (r->top > cr.bottom || r->bottom < cr.top) return 0;

  return 1;
}

int ListWnd::locateData(LPARAM data) { // linear search
  for (int i=0;i<itemList.getNumItems();i++) {
    if (itemList[i]->data == data)
      return i;
  }
  return -1;
}

int ListWnd::getItemFocused(int pos) {
  if (pos >= itemList.getNumItems()) return 0;
  return getItemFocused() == pos;
}

int ListWnd::getItemSelected(int pos) {
  listItem *item = itemList[pos];
  return (item && selItemList->isSelected(pos));
}

int ListWnd::hitTest(POINT pos, int drag) {
  RECT r;
  getClientRect(&r);
  if (pos.y < r.top) return LW_HT_ABOVE;
  if (getScrollY() > 0 && drag && pos.y < r.top + LISTWND_DRAG_MARGIN) return LW_HT_ABOVE;
  if (pos.y > r.bottom) return LW_HT_BELOW;
  if (getContentsHeight() > (getScrollY() + r.bottom-r.top) && drag && pos.y > r.bottom - LISTWND_DRAG_MARGIN) return LW_HT_BELOW;

  if (pos.x > r.left + getColumnsWidth() - getScrollX()) return LW_HT_DONTKNOW;
  if (pos.x < r.left + getScrollX()) return LW_HT_DONTKNOW;
  
  int i = (pos.y - r.top + getScrollY() - Y_SHIFT) / (itemHeight + (wantColSepOnItems() ? COLSEPHEIGHT : 0));
  if (i >= itemList.getNumItems()) return LW_HT_DONTKNOW;
  return i; 
}

int ListWnd::hitTest(int x, int y, int drag) {
  POINT pt={x, y};
  return hitTest(pt, drag);
}

void ListWnd::selectRect(int x1, int y1, int x2, int y2) {

}

void ListWnd::drawRect(int x1, int y1, int x2, int y2) {
  HDC dc = GetDC(gethWnd());
  RECT r={x1,y1,x2,y2};
  DrawFocusRect(dc, &r);
  ReleaseDC(gethWnd(), dc);
}

LPARAM ListWnd::getItemData(int pos) {
  // no need for bounds checking, the PtrList<> does it
  listItem *item = itemList[pos];
  return item ? item->data : NULL;
}

int ListWnd::getItemLabel(int pos, int subpos, char *txt, int textmax) {
  const char *t = getSubitemText(pos, subpos);
  if (t) {
    STRNCPY(txt, t, textmax);
    txt[textmax-1] = 0;
    return 1;
  }
  return 0;
}

void ListWnd::setItemLabel(int pos, const char *text) {
  if (pos >= itemList.getNumItems()) return;
  listItem *item = itemList[pos];
  if (!item) return;
  item->label = text;
  invalidateItem(pos);
}

void ListWnd::resort() {
  itemList.sort(TRUE);
  invalidate();
}

int ListWnd::getSortDirection() {
  return sortdir;
}

int ListWnd::getSortColumn() {
  return sortcol;
}

void ListWnd::setSortDirection(int dir) {
  sortdir=dir;
}

void ListWnd::setSortColumn(int col) {
  sortcol=col;
}

int ListWnd::sortCompareItem(listItem *p1, listItem *p2) {
  RecycleString l1=p1->label;
  RecycleString l2=p2->label;

  char slug[2] = {(char)0x7F, (char)0}; // a slug string, will always be too big.
  int i;

  if(sortcol!=0) {
    if (p1->subitems != NULL) {
      for (i=0;i<p1->subitems->getNumItems();i++) {
        listSubitemStruct *subitem = p1->subitems->enumItem(i);
        if (subitem->column == sortcol) {
          l1=subitem->label;
          break;
        }
      }
    } else {
      l1 = "";
    }
    if (p2->subitems != NULL) {
      for (i=0;i<p2->subitems->getNumItems();i++) {
        listSubitemStruct *subitem = p2->subitems->enumItem(i);
        if (subitem->column == sortcol) {
          l2=subitem->label;
          break;
        }
      }
    } else {
      l2 = "";
    }
  }

  if(!columnsList.enumItem(sortcol)->getNumeric()) {
    if(!sortdir) return(STRICMP(l1.getValue(), l2.getValue()));
    else return(STRICMP(l2.getValue(), l1.getValue()));
  } else {
    int a=ATOI(l1.getValue()),b=ATOI(l2.getValue());
    if(!sortdir) return a-b;
    else return b-a;
  }
}


int ListWnd::findItemByParam(LPARAM param) {
  for (int i=0;i<itemList.getNumItems();i++) {
    if (itemList[i]->data == param)
      return i;
  }
  return -1;
}

void ListWnd::setItemParam(int pos, LPARAM param) {
  if (pos >= itemList.getNumItems()) return;
  itemList[pos]->data = param;
  invalidateItem(pos);
}

int ListWnd::deleteByPos(int pos) {
  listItem *item = itemList[pos];
  if (item == NULL) return 0;

//  selItemList->setSelected(pos, FALSE); // If you do not delete the item from the selItemList, you corrupt it for all item positions downstream of this one.
  selItemList->deleteByPos(pos);

  itemList.removeByPos(pos);

  onItemDelete(item->data);

  deleteListItem(item); item=NULL;

  if (redraw) {
    invalidate();
    setSlidersPosition();
  }
  return 1;
}

void ListWnd::deleteAllItems() {
  int sav = getRedraw();
  deselectAll();
  setRedraw(FALSE);
  selItemList->deselectAll();	//force desel so as to avoid the MEMCPY
  while (itemList.getNumItems()) {
    deleteByPos(0);
  }
  setRedraw(sav);
//  setSlidersPosition();
}

void ListWnd::setSubItem(int pos, int subpos, const char *txt) {

  if (pos >= itemList.getNumItems()) return;
  listItem *item = itemList[pos];
  if (!item) return;
  if (!item->subitems)
    item->subitems = new PtrList<listSubitemStruct>;
  for (int i=0;i<item->subitems->getNumItems();i++) {
    listSubitemStruct *subitem = item->subitems->enumItem(i);
    if (subitem->column == subpos) {
      if (subitem->label) {
        FREE(subitem->label);
        subitem->label = NULL;
      }
      if (txt)
        subitem->label = STRDUP(txt);
      invalidateItem(pos);
      return;
    }
  }
  listSubitemStruct *subitem = (listSubitemStruct *) MALLOC(sizeof(listSubitemStruct));
  item->subitems->addItem(subitem);
  subitem->label = STRDUP(txt);
  subitem->column = subpos;
  invalidateItem(pos);
}

//fixmeSkinBitmap *ListWnd::getItemIcon(int pos) {
//fixme  if (pos >= itemList.getNumItems()) return NULL;
//fixme  listItem *item = itemList[pos];
//fixme  return item->icon;
//fixme}

//fixmevoid ListWnd::setItemIcon(int pos, const char *bitmapid) {
//fixme  if (pos >= itemList.getNumItems()) return;
//fixme  listItem *item = itemList[pos];
//fixme  item->icon = bitmapid;
//fixme  invalidateItem(pos);
//fixme}

const char *ListWnd::getSubitemText(int pos, int subpos) {
  if (pos >= itemList.getNumItems()) return NULL;
  listItem *item = itemList[pos];
  if (!item) return NULL;
  if (subpos == 0) {
    return item->label;
  }
  if (!item->subitems) return NULL;
  for (int i=0;i<item->subitems->getNumItems();i++) {
    listSubitemStruct *subitem = item->subitems->enumItem(i);
    if (subitem->column == subpos) {
      return subitem->label;
    }
  }
  return NULL;
}

listItem *ListWnd::createListItem() {
  listItem *item = listItem_freelist.getRecord();
  new(item) listItem(this);
//CUT  item->setList(this);
  return item;
}

void ListWnd::deleteListItem(listItem *item) {
  if (item == NULL) return;
  item->~listItem();
  listItem_freelist.freeRecord(item);
}

ListColumn *ListWnd::enumListColumn(int pos) {
  return columnsList[pos];
}

int ListWnd::getColumnPosByName(const char *name) {
  for (int i = 0; i < columnsList.getNumItems(); i++) {
    const char *name2 = columnsList[i]->getName();
    if (name2 == NULL) continue;
    if (STREQL(name, name2)) return i;
  }
  return -1;
}

int ListWnd::delColumnByPos(int pos) {
  if (pos < 0 || pos >= columnsList.getNumItems()) return 0;
  delete columnsList[pos];
  columnsList.removeByPos(pos);
  recalcHeaders();
  return 1;
}

void ListWnd::recalcHeaders() {
  if (getNumColumns() <= 0) return;
  
  int wid = 0, ndynamic = 0, i = 0;
  for (i = 0; i < getNumColumns(); i++) {
    ListColumn *col = enumListColumn(i);
    if (!col->isDynamic()) wid += col->getWidth()+COLUMNS_MARGIN-1;
    else ndynamic++;
  }
  if (ndynamic == 0) return;

  RECT r = clientRect();
  int wwidth = (r.right - r.left);

  int alloc_width = wwidth - wid;
  if (alloc_width <= 1) return;

  int each = alloc_width / ndynamic;
  int remainder = alloc_width % ndynamic;

  int first=1;
  for (i = 0; i < getNumColumns(); i++) {
    ListColumn *col = enumListColumn(i);
    if (col->isDynamic()) {
      int amt = each;
      if (first) amt += remainder;	// give column 0 the leftovers
      col->setWidth(amt);
      first = 0;
    }
  }

  //BU note: we could probably find a way to not invalidate everything
  invalidate();
}

void ListWnd::itemSelection(int itemnum, int selected) {
  notifySelChanged(itemnum, selected);
  onItemSelection(itemnum, selected);
}

void ListWnd::onItemSelection(int itemnum, int selected) {
  if (selected) {
#ifdef WASABI_API_ACCESSIBILITY
    Accessible *a = getAccessibleObject();
    if (a != NULL) 
      a->onGetFocus(itemnum);
#endif
  }
}

int ListWnd::doAddItem(const char *label, LPARAM lParam, int pos) {

  listItem *item = createListItem();
  item->label = label;
  item->data = lParam;

  itemList.addItem(item, pos, ITEMLIST_INC);
  lastAddedItem = item;

  if (redraw) {
    invalidate(); // todo: optimize to invalidate only if necessary
    setSlidersPosition();
  }

  if (isInited()) calcBounds();
  int p = (pos == POS_LAST) ? itemList.getNumItems()-1 : pos; 
  if (p <= selectionStart) selectionStart++;
  if (autosort) {
    itemList.sort();
    p = itemList.searchItem(item);
  }
  return p;
}

void ListWnd::setMinimumSize(int size) {
  if (size > 0) itemList.setMinimumSize(size);
}

int ListWnd::addItem(const char *label, LPARAM lParam) {
  return doAddItem(label, lParam, POS_LAST);
}

int ListWnd::insertItem(int pos, const char *label, LPARAM lParam) {
  return doAddItem(label, lParam, pos);
}

int ListWnd::getLastAddedItemPos() {
  if (lastAddedItem == NULL) return -1;
  return itemList.searchItem(lastAddedItem);
}

int ListWnd::getColumnsHeight() {
  return columnsHeight;
}

int ListWnd::getColumnsWidth() {
  int i, x=0;
  for (i=0;i<columnsList.getNumItems();i++) x+= columnsList[i]->getWidth();
  return x+1;
}

int ListWnd::insertColumn(ListColumn *col, int pos, int alignment) {
  ASSERT(col != NULL);
  ASSERT(pos >= -1);
  if (pos < 0) col->setIndex(columnsList.getNumItems());
  else col->setIndex(pos);
  col->setList(this);
  col->setAlignment(alignment);
  columnsList.addItem(col);
  if (pos >= 0) {
    columnsList.moveItem(columnsList.getNumItems()-1, pos);
  }
  if (redraw && isInited()) {
    invalidate();
    setSlidersPosition();
  }
  recalcHeaders();
  return columnsList.getNumItems();
}

void ListWnd::deleteAllColumns() {
  columnsList.deleteAll();
  if (redraw && isInited()) {
    invalidate();
    setSlidersPosition();
  }
  recalcHeaders();
}

int ListWnd::addColumn(const char *name, int width, int numeric, int align) {
  ListColumn *col = new ListColumn();
  col->setWidth(width);
  col->setLabel(name);
  col->setNumeric(numeric);
  col->setAlignment(align);
  return insertColumn(col, -1);
}

BOOL ListWnd::setRedraw(BOOL _redraw) {
  int prev = redraw;
  if (!redraw && _redraw) {
    invalidate();
    setSlidersPosition();
    notifySelChanged();
  }
  redraw = _redraw;
  return prev;
}

BOOL ListWnd::getRedraw() {
  return redraw;
}

int ListWnd::onChar(unsigned int c) {
  //CT> Commented this out so shortcuts work in playlist editor
  /*char b = TOUPPER(c);
  if (b >= 'A' && b <= 'Z') {
    jumpToNext(b);
    return 1;
  }*/

  return LISTWND_PARENT::onChar(c);
}

int ListWnd::onKeyDown(int keyCode) {
  switch (keyCode) {
    case VK_DOWN:
      next(selectonupdown);
      return 1;
    case VK_UP:
      previous(selectonupdown);
      return 1;
    case VK_PRIOR:
      pageup(selectonupdown);
      return 1;
    case VK_NEXT:
      pagedown(selectonupdown);
      return 1;
    case VK_HOME:
      home(selectonupdown);
      return 1;
    case VK_END:
      end(selectonupdown);
      return 1;
    case VK_DELETE:
      onDelete();
      return 1;
    case VK_RETURN: {
      int i=getItemFocused();
      setSelected(i, 0, 0);
      setSelected(i, 1, 1);
      if(i!=-1)
        onDoubleClick(i);
      return 1;
    }
  }
  return LISTWND_PARENT::onKeyDown(keyCode);
}

void ListWnd::next(int wantcb) {
  int from=getItemFocused();
/*  if (selItemList->getNumItems() > 0)
    for (int i=0;i<itemList.getNumItems();i++)
      if (getItemSelected(i)) {
        from = i;
        break;
      }*/
  int to = from + 1;
  if (to < itemList.getNumItems() && to >= 0) {
    setSelectionStart(to, wantcb);
    if (!fullyVisible(to)) {
      RECT c;
      getClientRect(&c);
      scrollToY((Y_SHIFT*2+(to+1)*(itemHeight+(wantColSepOnItems()?COLSEPHEIGHT:0)))-(c.bottom-c.top));
    }
    if (!wantcb) {
      char t[256]="";
      getItemLabel(to, 0, t, 255);
      foreach(tempselectnotifies)
        sendAction(tempselectnotifies.getfor(), "tempselectnotify", t);
      endfor;
    }
  }
}

void ListWnd::selectCurrent() {
  int from=getItemFocused();
  int to = from;
  if (to < itemList.getNumItems() && to >= 0) {
    setSelectionStart(to);
    if (!fullyVisible(to)) {
      RECT c;
      getClientRect(&c);
      scrollToY((Y_SHIFT*2+(to+1)*(itemHeight+(wantColSepOnItems()?COLSEPHEIGHT:0)))-(c.bottom-c.top));
    }
  }
}

void ListWnd::selectFirstEntry(int wantcb) {
  setSelectionStart(0, wantcb);
  ensureItemVisible(0);
}

void ListWnd::previous(int wantcb) {
  int from=0;
/*  if (selItemList->getNumItems() > 0)
    for (int i=0;i<itemList.getNumItems();i++)
      if (getItemSelected(i)) {
        from = i;
        break;
      }*/
  from = getItemFocused();
  int to = from - 1;
  if (to < itemList.getNumItems() && to >= 0) {
    setSelectionStart(to, wantcb);
    ensureItemVisible(to);
    if (!wantcb) {
      char t[256]="";
      getItemLabel(to, 0, t, 255);
      foreach(tempselectnotifies)
        sendAction(tempselectnotifies.getfor(), "tempselectnotify", t);
      endfor;
    }
  }
}

void ListWnd::pagedown(int wantcb) {
  int from=-1,to;
  if (selItemList->getNumSelected())
    for (int i=0;i<itemList.getNumItems();i++)
      if (getItemSelected(i)) {
        from = i;
        break;
      }
  if(from==-1) to = 0;
  else to = from + getLinesPerPage();
  to=MIN(to,itemList.getNumItems()-1);
  if(to>=0) {
    setSelectionStart(to, wantcb);
    if (!fullyVisible(to)) {
      RECT c;
      getClientRect(&c);
      scrollToY((Y_SHIFT*2+(to+1)*(itemHeight+(wantColSepOnItems()?COLSEPHEIGHT:0)))-(c.bottom-c.top));
    }
    if (!wantcb) {
      char t[256]="";
      getItemLabel(to, 0, t, 255);
      foreach(tempselectnotifies)
        sendAction(tempselectnotifies.getfor(), "tempselectnotify", t);
      endfor;
    }
  }
}

void ListWnd::pageup(int wantcb) {
  int from=-1,to;
  if (selItemList->getNumSelected())
    for (int i=0;i<itemList.getNumItems();i++)
      if (getItemSelected(i)) {
        from = i;
        break;
      }
  if(from==-1) to = 0;
  else to = from - getLinesPerPage();
  to=MAX(to,0);
  to=MIN(to,itemList.getNumItems()-1);
  if(to>=0) {
    setSelectionStart(to, wantcb);
    ensureItemVisible(to);
    if (!wantcb) {
      char t[256]="";
      getItemLabel(to, 0, t, 255);
      foreach(tempselectnotifies)
        sendAction(tempselectnotifies.getfor(), "tempselectnotify", t);
      endfor;
    }
  }
}

void ListWnd::home(int wantcb) {
  if(!itemList.getNumItems()) return;
  setSelectionStart(0, wantcb);
  ensureItemVisible(0);
  if (!wantcb) {
    char t[256]="";
    getItemLabel(0, 0, t, 255);
    foreach(tempselectnotifies)
      sendAction(tempselectnotifies.getfor(), "tempselectnotify", t);
    endfor;
  }
}

void ListWnd::end(int wantcb) {
  if(!itemList.getNumItems()) return;
  int i=itemList.getNumItems()-1;
  setSelectionStart(i, wantcb);
  if (!fullyVisible(i)) {
    RECT c;
    getClientRect(&c);
    scrollToY((Y_SHIFT*2+(i+1)*(itemHeight+(wantColSepOnItems()?COLSEPHEIGHT:0)))-(c.bottom-c.top));
  }
  if (!wantcb) {
    char t[256]="";
    getItemLabel(i, 0, t, 255);
    foreach(tempselectnotifies)
      sendAction(tempselectnotifies.getfor(), "tempselectnotify", t);
    endfor;
  }
}

void ListWnd::jumpToNext(char c) {
  if (doJumpToNext(c, FALSE)) return;
  doJumpToNext(c, TRUE);
}

int ListWnd::doJumpToNext(char c, BOOL fromtop) {
  int from = 0;
  if (!fromtop && selItemList->getNumSelected()) {
    for (int i=0;i<itemList.getNumItems();i++)
      if (getItemSelected(i)) {
        from = i+1;
        break;
      }
  }
  for (int j=from;j<itemList.getNumItems();j++) {
    listItem *item = itemList[j];
    if (item->label != NULL) {
      char z = TOUPPER(*(item->label));
      if (z == c) {
        setSelectionStart(j);
        ensureItemVisible(j);
        return 1;
      }
    }
  }
  return 0;
}

void ListWnd::reset() {
  columnsList.deleteAll();
  deleteAllItems();
}

void ListWnd::setShowColumnsHeaders(int show) {
  int prev = showColumnsHeaders;
  showColumnsHeaders = show;
  if (prev != show) {
    invalidate();
  }
}

int ListWnd::onContextMenu (int x, int y) {
  return notifyParent(ChildNotify::LISTWND_POPUPMENU, x, y);
}

void ListWnd::scrollUp(int lines) {
  scrollToY(MAX(0, getScrollY()-itemHeight*lines));
}

void ListWnd::scrollLeft(int lines) {
  scrollToX(MAX(0, getScrollX()-itemHeight*lines));
}

void ListWnd::scrollDown(int lines) {
  scrollToY(MIN(getMaxScrollY(), getScrollY()+itemHeight*lines));
}

void ListWnd::scrollRight(int lines) {
  scrollToX(MIN(getMaxScrollX(), getScrollX()+itemHeight*lines));
}

int ListWnd::onMouseWheelUp(int clicked, int lines) {
  lines *= StdWnd::mousewheelGetScrollLines();
  if (!clicked)
    scrollUp(lines);
  else
    scrollLeft(lines);
  return 1;
}

int ListWnd::onMouseWheelDown(int clicked, int lines) {
  lines *= StdWnd::mousewheelGetScrollLines();
  if (!clicked)
    scrollDown(lines);
  else
    scrollRight(lines);
  return 1;
}

int ListWnd::onColumnLabelClick(int col, int x, int y)
{
  if(lastcolsort==col) {
    setSortDirection(1);
    lastcolsort=-1;
  } else {
    setSortDirection(0);
    lastcolsort=col;
  }
  setSortColumn(col);
  resort();
  return 1;
}

COLORREF ListWnd::getTextColor(LPARAM lParam) {
  return textColor;
}

COLORREF ListWnd::getSelBgColor(LPARAM LParam) {
  return color_item_selected;
}

COLORREF ListWnd::getSelFgColor(LPARAM LParam) {
  COLORREF r = color_item_selected_fg;
  if (r == 0xFFFFFFFF)
    return color_item_selected_fg;
  return r;
}

COLORREF ListWnd::getBgColor() {
#ifdef WASABI_COMPILE_SKIN
  return bgcolor;
#else
  return LISTWND_PARENT::getBgColor();
#endif
}

COLORREF ListWnd::getFocusColor(LPARAM LParam) {
  return color_item_focused;
}

COLORREF ListWnd::getFocusRectColor(LPARAM lParam) {
  return color_item_focusrect;
}


void ListWnd::moveItem(int from, int to) {
  itemList.moveItem(from,to);
  invalidate();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ListColumn
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


ListColumn::ListColumn(const char *name, int isdynamic) :
  Named(name), dynamic(isdynamic)
{
  index = -1;
  width = COLUMNS_DEFAULT_WIDTH;
  list = NULL;
}

void ListColumn::setLabel(const char *newlabel) {
  setName(newlabel);
}

const char *ListColumn::getLabel() {
  return getName();
}

void ListColumn::setIndex(int newindex) {
  index = newindex;
}

int ListColumn::getIndex() {
  return index;
}

void ListColumn::setWidth(int _width) {
  width = _width;
  if (list && list->getRedraw()) {
    list->invalidate();
  }
}

int ListColumn::getWidth() {
  return width;
}

int ListColumn::customDrawHeader(Canvas *c, RECT *r) {
  int y = (r->bottom-r->top-c->getTextHeight()) / 2;
  c->textOutEllipsed(r->left, y, r->right-r->left, c->getTextHeight(), _(getName()));
  return 1;
}

void ListColumn::setDynamic(int isdynamic) {
  int prev = dynamic;
  dynamic = !!isdynamic;
  if (prev != dynamic && dynamic && list != NULL) list->recalcHeaders();
}

void ListColumn::setList(ListWnd *_list) {
  list = _list;
}

ListWnd *ListColumn::getList() {
  return list;
}

int ListWnd::wantAutoContextMenu() { 
  return 0; 
}

int ListWnd::onAcceleratorEvent(const char *name) {
  if(STRCASEEQL(name,"selectall")) {
    selectAll();
    return 1;
  }
  return 0;
}

int ListWnd::onAction(const char *action, const char *param, int x, int y, int p1, int p2, void *data, int datalen, RootWnd *source) {
  int r = LISTWND_PARENT::onAction(action, param, x, y, p1, p2, data, datalen, source);
  if (STRCASEEQLSAFE(action, "register_tempselectnotify")) {
    tempselectnotifies.addItem(source);
  }
  else if (STRCASEEQLSAFE(action, "up")) {
    previous(p1);
  }
  else if (STRCASEEQLSAFE(action, "down")) {
    next(p1);
  }
  else if (STRCASEEQLSAFE(action, "home")) {
    home(p1);
  }
  else if (STRCASEEQLSAFE(action, "end")) {
    end(p1);
  }
  else if (STRCASEEQLSAFE(action, "pageup")) {
    pageup(p1);
  }
  else if (STRCASEEQLSAFE(action, "pagedown")) {
    pagedown(p1);
  }
  else if (STRCASEEQLSAFE(action, "select_current")) {
    selectCurrent();
  }
  else if (STRCASEEQLSAFE(action, "doubleclick")) {
    int pos = getItemFocused();
    if (pos >= 0) onDoubleClick(pos);
    return 1;
  }
  return r;
}

int ListWnd::accessibility_getNumItems() {
  return getItemCount();
}

const char *ListWnd::accessibility_getItemName(int n) {
  char str[256]="";
  accessibleItemName = str;

  listItem *i = itemList.enumItem(n);
  if (i != NULL) {
    if (i->data != NULL || i->label == NULL) { 
      for (int j=0;j<getNumColumns();j++) {
        *str = 0;
        convertlParamColumn(j, n, i->data, str, 255);
        str[255] = 0;
        if (*str != 0) {
          if (!accessibleItemName.isempty())
            accessibleItemName += ", ";
          accessibleItemName += str;
        }
      }
    } else {
      accessibleItemName = i->label;
      for (int j=1;j<getNumColumns();j++) {
        const char *t = getSubitemText(n, j);
        if (t != NULL && *t != 0) {
          if (!accessibleItemName.isempty())
            accessibleItemName += ", ";
          accessibleItemName += t;
        }
      }
    }
  }
  return accessibleItemName;
}

int ListWnd::accessibility_getItemRole(int n) {
  return ROLE_SYSTEM_LISTITEM;
}

int ListWnd::accessibility_getItemState(int n) {
  int selected = getItemSelected(n);
  return STATE_SYSTEM_SELECTABLE | STATE_SYSTEM_FOCUSABLE | (selected ? STATE_SYSTEM_SELECTED : 0) | (preventMultipleSelection ? 0 : STATE_SYSTEM_MULTISELECTABLE) | (n == lastItemFocusedPos ? STATE_SYSTEM_FOCUSED : 0);
}

void ListWnd::accessibility_getItemRect(int n, RECT *r) {
  getItemRect(n, r);
}

int ListWnd::accessibility_getRole() {
  return ROLE_SYSTEM_LIST;
}

int ListWnd::accessibility_isLeaf() {
  return 1;
}

void ListWnd::setShowIcons(int icons) {
  showicons = icons;
  invalidate();
}

