#ifndef _WASABI_LISTWND_H
#define _WASABI_LISTWND_H

#include <bfc/string/string.h>
#include <bfc/freelist.h>
#include <wnd/scbkgwnd.h>

#define POS_LAST -1

#define LW_HT_DONTKNOW	(-1)
#define LW_HT_ABOVE	(-10)
#define LW_HT_BELOW	(-20)

#define COL_LEFTALIGN 0
#define COL_CENTERALIGN 1
#define COL_RIGHTALIGN 2

class listItem;
class ListWnd;
class CompareListItem;


class ListColumn : public Named {
friend class ListWnd;
public:
  ListColumn(const char *name=NULL, int isdynamic=FALSE);
  virtual ~ListColumn() { }

  int getWidth();
  
  void setWidth(int newwidth);
  const char *getLabel();
  
  void setLabel(const char *newlabel);
  
  virtual int customDrawHeader(Canvas *c, RECT *cr);

  virtual int onHeaderClick() { return 0; }//return 1 if you override

  virtual int onColumnLeftClick(int pos) { return 0; }//return 1 if you override
  
  int getNumeric() { return numeric; }

  
  void setDynamic(int isdynamic);
  
  int isDynamic() { return dynamic; }

  void setAlignment(int _align) { align = _align; }
  int getAlignment() { return align; }

protected:  
  
  void setIndex(int i);
  
  int getIndex();
  
  void setList(ListWnd *list);
  
  ListWnd *getList();
  
  void setNumeric(int n) { numeric=n; }

private:
  int width;
  int index;
  int numeric;
  int dynamic;
  ListWnd *list;
  int align;
};

class SelItemList;

#define LISTWND_PARENT ScrlBkgWnd
class ListWnd : public LISTWND_PARENT {

friend class ListColumn;
friend class SelItemList;

public:
  ListWnd();
  
  virtual ~ListWnd();

  
  virtual int onInit();
  
  virtual int onPostOnInit();
  
  virtual int onPaint(Canvas *canvas);
  
  virtual int onResize();
  
  virtual int onLeftButtonDown(int x, int y);
  virtual int onLeftButtonUp(int x, int y);
  
  virtual int onMiddleButtonDown(int x, int y);
  virtual int onMiddleButtonUp(int x, int y);

  virtual int onRightButtonDown(int x, int y);
  virtual int onRightButtonUp(int x, int y);
  
  virtual int onMouseMove(int x, int y);
  
  virtual int onLeftButtonDblClk(int x, int y);
  
  virtual int onChar(unsigned int c);
  
  virtual int onKeyDown(int keyCode);
  virtual int onContextMenu (int x, int y);
  
  virtual int wantAutoContextMenu();
  
  virtual int onMouseWheelUp(int click, int lines);
  virtual int onMouseWheelDown(int click, int lines);
  
  virtual int wantAutoDeselect() { return wantautodeselect; }
  
  virtual void setWantAutoDeselect(int want) { wantautodeselect = want; }
  
  
  void onSetVisible(int show);

  
  void setAutoSort(BOOL dosort);
  
  void setOwnerDraw(BOOL doownerdraw);
  
  virtual void timerCallback(int id);

  
  void next(int wantcb=1);
  void selectCurrent();
  void selectFirstEntry(int wantcb=1);
  
  void previous(int wantcb=1);
  
  void pagedown(int wantcb=1);
  
  void pageup(int wantcb=1);
  
  void home(int wantcb=1);
  
  void end(int wantcb=1);
  
  
  void setItemCount(int c);
  
  void reset();

  void setShowColumnsHeaders(int show);
  
  int addColumn(const char *name, int width, int numeric=0, int align=COL_LEFTALIGN);	// adds to end
  
  ListColumn *getColumn(int n);
  
  int getNumColumns();
  
  int getColumnWidth(int col);
  
  BOOL setRedraw(BOOL redraw);	// returns prev state
  
  BOOL getRedraw();
  
  void setMinimumSize(int size);
  
  virtual int addItem(const char *label, LPARAM lParam=0);
  
  virtual int insertItem(int pos, const char *label, LPARAM lParam=0);
  
  virtual int getLastAddedItemPos();
  
  virtual void setSubItem(int pos, int subpos, const char *txt);
  
  virtual void deleteAllItems();
  
  virtual int deleteByPos(int pos);
  
  int getNumItems(void);
  
  virtual int getItemLabel(int pos, int subpos, char *text, int textmax);
  
  virtual void setItemLabel(int pos, const char *text);
  
  virtual LPARAM getItemData(int pos);
  
  virtual int getItemRect(int pos, RECT *r);
  
  virtual int getItemSelected(int pos);	// returns 1 if selected
  
  virtual int getItemFocused(int pos);	// returns 1 if focused
  
  virtual int getItemFocused();         // returns focused item, DEPRECATED
  virtual int getFocusedItem();         // returns focused item
  
  void setItemFocused(int pos, int ensure_visible=TRUE);
  virtual void onItemSetFocused(int pos) { }	// only set, not clicked
  
  void ensureItemVisible(int pos1, int pos2=-1);
  
  void invalidateColumns();
  
  virtual int scrollAbsolute(int x);
  
  virtual int scrollRelative(int x);
  
  virtual void scrollLeft(int lines=1);
  
  virtual void scrollRight(int lines=1);
  
  virtual void scrollUp(int lines=1);
  
  virtual void scrollDown(int lines=1);
  virtual const char *getSubitemText(int pos, int subpos);
  
  int getFirstItemSelected();
  
  int getNextItemSelected(int lastpos);	// next item AFTER given pos

  
  virtual int selectAll(int cb=1);	// force all items selected
  
  virtual int deselectAll(int cb=1); // force all items to be deselected
  
  virtual int invertSelection(int cb=1);	// invert all selections

  
  virtual int hitTest(POINT pos, int drag=0);

  virtual int hitTest(int x, int y, int drag=0);
  
  virtual int invalidateItem(int pos);
  
  virtual int locateData(LPARAM data);

  // -1 if we've never been drawn yet
  
  int getFirstItemVisible() const { return firstItemVisible; }
  
  int getLastItemVisible() const { return lastItemVisible; }

  void setItemHeight(int h);
  void setItemExtraHeight(int top=0, int bot=0);	// call before setFontSize()

  virtual int setFontSize(int size);
  
  virtual int getFontSize();
  
  virtual void jumpToNext(char c);
  
  int wantFocus() { return 1; }

  
  void scrollToItem(int pos);

  
  virtual void resort();
  
  int getSortDirection();
  
  int getSortColumn();
  
  void setSortColumn(int col);
  
  void setSortDirection(int dir);
  
  int findItemByParam(LPARAM param);
  
  void setItemParam(int pos, LPARAM param);
  void setItemData(int pos, LPARAM param) { setItemParam(pos, param); }
  
  int getItemCount() { return getNumItems(); }

  
  void setSelectionStart(int pos, int wantcb=1);
  
  virtual void setSelectionEnd(int pos);
  
  void setSelected(int pos, int selected, int cb=1);
  
  void toggleSelection(int pos, int setfocus=TRUE, int cb=1);

  
  virtual int getHeaderHeight();

  // this sort function just provides string/numeric comparison
  // if you need more types, just override and provide your own
  
  virtual int sortCompareItem(listItem *p1, listItem *p2);


  int getPreventMultipleSelection() {	return preventMultipleSelection; }

  int setPreventMultipleSelection(int val) {	return preventMultipleSelection = val; }

  
  void moveItem(int from, int to);

  
  virtual int onAcceleratorEvent(const char *name);

  // override this to turn the LPARAM into a text
  virtual const char *convertlParam(LPARAM lParam) { return NULL; }
  virtual void convertlParamColumn(int col, int pos, LPARAM lParam, char *str, int maxlen) { };

protected:
  // return 1 if you override this
  
  virtual int ownerDraw(Canvas *canvas, int pos, RECT *r, LPARAM lParam, int selected, int focused) { return 0; };

  virtual int ownerDrawColumn(Canvas *canvas, int pos, const RECT *r, int column, LPARAM lParam, int selected, int focused, const char *txt) { return 0; }
  
  virtual void onPreItemDraw(Canvas *canvas, int pos, const RECT *r, LPARAM lParam, int selected, int focused) { }
  
  virtual void onPostItemDraw(Canvas *canvas, int pos, const RECT *r, LPARAM lParam, int selected, int focused) { };
  
  virtual COLORREF getTextColor(LPARAM lParam);
  virtual int getTextBold(LPARAM lParam) { return 0; }
  virtual int getColumnBold(LPARAM lParam, int col) { return -1; }
  virtual int getTextItalic(LPARAM lParam) { return 0; }
  
  virtual COLORREF getSelBgColor(LPARAM lParam);
  
  virtual COLORREF getSelFgColor(LPARAM lParam);
  
  virtual COLORREF getBgColor();
  
  virtual COLORREF getFocusColor(LPARAM lParam);
  virtual COLORREF getFocusRectColor(LPARAM lParam);
  virtual int needFocusRect(LPARAM lParam) { return 0; }
  
  virtual COLORREF getColumnSepColor();

  
  virtual int wantColSepOnItems();
  
  virtual int getXShift();

public:
  int insertColumn(ListColumn *col, int pos=-1, int alignment=COL_LEFTALIGN);// -1 is add to end
//  void deleteColumn(int pos);
  void deleteAllColumns();

  void setHoverSelect(int a) { hoverselect = a; }
  int getHoverSelect() { return hoverselect; }

  void setSelectOnUpDown(int i) { selectonupdown = i; }
  int getSelectOnUpDown() { return selectonupdown; }
  virtual int onAction(const char *action, const char *param=NULL, int x=-1, int y=-1, int p1=0, int p2=0, void *data=NULL, int datalen=0, RootWnd *source=NULL);

  /** Will only work with simple text lists, be forwarned!!!  */
  int getItemHeight() { return itemHeight; }

  virtual int accessibility_getNumItems();
  virtual const char *accessibility_getItemName(int n);
  virtual int accessibility_getItemRole(int n);
  virtual int accessibility_getItemState(int n);
  virtual void accessibility_getItemRect(int n, RECT *r);
  virtual int accessibility_getRole();
  virtual int accessibility_isLeaf();

protected:
  
  virtual int getColumnsHeight();
  
  virtual int getColumnsWidth();
  
  virtual int getContentsWidth();
  
  virtual int getContentsHeight();
  
  virtual void drawBackground(Canvas *canvas);
  
  void drawColumnHeaders(Canvas *c);
  
  void drawItems(Canvas *canvas);
  
  void updateScrollX();
  
  void updateScrollY();
  
  int doJumpToNext(char c, BOOL fromTop);
  
  int fullyVisible(int pos);

  
  virtual int onBeginDrag(int iItem);
  
  virtual int dragOver(int x, int y, RootWnd *sourceWnd);

  
  virtual void onSelectAll();	// hit Control-A
  
  virtual void onDelete() { }	// hit 'delete'
  
  virtual void onItemDelete(LPARAM lparam) { }
  
  virtual void onDoubleClick(int itemnum) { }	// double-click on an item
  // this is called with the selected item#
  
  virtual void onLeftClick(int itemnum) { } // left-click

  // the second time you click on an already-focused item
  virtual void onSecondLeftClick(int itemnum) { }

  // this is called once for the item under cursor on click
  
  virtual void onMiddleClick(int itemnum) { } // middle-click on item

  virtual int onRightClick(int itemnum) { return 0; } // right-click on item

  // override this to be notified of item selections & deselections
  
  virtual void onItemSelection(int itemnum, int selected);

  
  virtual int onColumnDblClick(int col, int x, int y) { return 0; }
  
  virtual int onColumnLabelClick(int col, int x, int y);

  
  void selectRect(int x1, int y1, int x2, int y2);
  
  void drawRect(int x1, int y1, int x2, int y2);

  // interface to Freelist
  
  listItem *createListItem();
  
  void deleteListItem(listItem *item);

  
  ListColumn *enumListColumn(int pos);
  
  int getColumnPosByName(const char *name);
  
  int delColumnByPos(int pos);

  void setShowIcons(int icons);
//fixme  SkinBitmap *getItemIcon(int item);
//fixme  void setItemIcon(int pos, const char *bitmapid);

//CUT  int itemVisibleDelta(int pos);

protected:
  int item_invalidate_border;
  BOOL showColumnsHeaders;

  
  void recalcHeaders();
  
  void itemSelection(int itemnum, int selected);

  int listwnd_middle_button_selects;

  int listwnd_want_smoothscroll;

private:
  
  int doAddItem(const char *label, LPARAM lParam, int pos);
  
  int hitTestColumns(POINT p, int *origin=NULL);

  int hitTestColumnClient(int x);
  
  
  int hitTestColumnsLabel(POINT p);
  
  void drawXorLine(int x);
  
  void calcNewColWidth(int col, int x);
  
  void calcBounds();
  
  void onDragTimer();

  void notifySelChanged(int item=-1, int sel=-1);
  virtual int wantResizeCols() { return 1; }

  int autosort, ownerdraw;
  int textsize;
  int itemHeight, itemExtraHeightTop, itemExtraHeightBot;
  BOOL metrics_ok;
  BOOL redraw;
  int columnsHeight;
  int dragtimeron;

  PtrList<ListColumn> columnsList;
  PtrListQuickSorted<listItem,CompareListItem> itemList;

  int firstItemVisible;
  int lastItemVisible;

  listItem *lastItemFocused;
  int lastItemFocusedPos;

  listItem *lastAddedItem;
  SelItemList *selItemList;

  int dragskip;
  int dragskipcount;
  int selectionStart;
  int colresize;
  POINT colresizept;
  BOOL resizing_col;
  int colresizeo;

  BOOL processbup;
  BOOL bdown;
  BOOL nodrag;
  int bdownx, bdowny;
  BOOL firstComplete, lastComplete;

  int rectselecting;
  POINT selectStart;
  POINT selectLast;

  int sortdir, sortcol, lastcolsort;

  int preventMultipleSelection;
	
  Freelist<listItem> listItem_freelist;
  int wantautodeselect;

  int hoverselect;
  int selectonupdown;
  PtrList<RootWnd> tempselectnotifies;
  String accessibleItemName;
  int showicons;
};

#endif
