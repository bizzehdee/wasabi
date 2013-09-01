#include "precomp.h"

#include "treewnd.h"

#include <wnd/canvas.h>
#include <bfc/stack.h>
#include <wnd/scrollbar.h>
//fixme #include <bfc/skinclr.h>
#include <wnd/notifmsg.h>
//CUT #include <studio/api.h>
#include <wnd/accessible.h>

#define DEF_TEXT_SIZE 14
#define CHILD_INDENT itemHeight
#define X_SHIFT 2
#define Y_SHIFT 2
#define DRAG_THRESHOLD 4

#define TIMER_EDIT_DELAY 1000
#define TIMER_EDIT_ID    1249

///////////////////////////////////////////////////////////////////////////////
// TreeWnd
///////////////////////////////////////////////////////////////////////////////

namespace {
  _color textcolor("wasabi.tree.text", "#ffffff");
  _color drophilitecolor("wasabi.tree.hiliteddrop", "#0000ff");
  _color selectedcolor("wasabi.tree.selected", "#00ff00");
};

int CompareTreeItem::compareItem(TreeItem *p1, TreeItem *p2) {
  return p1->getTree()->compareItem(p1, p2);
}

TreeWnd::TreeWnd() : hilitedColor(NULL, "#ff00ff") {
/*  tabClosed = NULL;
  tabOpen = NULL;
  linkTopBottom = NULL;
  linkTopRight = NULL;
  linkTopRightBottom = NULL;
  linkTabTopBottom = NULL;
  linkTabTopRight = NULL;
  linkTabTopRightBottom = NULL; */
  curSelected = NULL;
  mousedown_item = NULL;
  hitItem = NULL;
  draggedItem = NULL;
  tipitem = NULL;
  edited = NULL;
  editwnd = NULL;
  metrics_ok = FALSE;
  setSorted(TRUE);
  setFontSize(DEF_TEXT_SIZE);
  redraw = TRUE;
  prevbdownitem = NULL;
  autoedit=0;
  autocollapse=1;
#ifdef WASABI_COMPILE_SKIN
  drawlines=1;
#else
  drawlines=0;
#endif

  tabClosed = "wasabi.tree.tab.closed";
  tabOpen = "wasabi.tree.tab.open";
  linkTopBottom = "wasabi.tree.link.top.bottom";
  linkTopRight = "wasabi.tree.link.top.right";
  linkTopRightBottom = "wasabi.tree.link.top.rightBottom";
  linkTabTopBottom = "wasabi.tree.link.tab.top.bottom";
  linkTabTopRight = "wasabi.tree.link.tab.top.right";
  linkTabTopRightBottom = "wasabi.tree.link.tab.top.rightBottom";
}

TreeWnd::~TreeWnd() {
  // delete all root items
  deleteAllItems();
  drawList.removeAll();
}

int TreeWnd::onInit() {
  TREEWND_PARENT::onInit();

  setBgBitmap("wasabi.tree.background");
  setLineHeight(itemHeight);

  return 1;
}

void TreeWnd::setRedraw(BOOL r) {
  int old = redraw;
  redraw = r;
  if (!old && redraw)
    invalidate();
}

void TreeWnd::setDrawlines(int draw)  {
  drawlines = draw;
}


int TreeWnd::getDrawlines()  {
  return drawlines;
}



int TreeWnd::onPaint(Canvas *canvas) {

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
  TREEWND_PARENT::onPaint(canvas);

/* uncomment if you add columns or anything that should be not be drawn over by onPaint in which case you'll have to clip->subtract(your_region)
  Region *clip = new RegionI();
  canvas->getClipRgn(clip); */

  RECT r;
  getNonClientRect(&r);

  int y = -getScrollY()+Y_SHIFT+r.top;
  int x = -getScrollX()+X_SHIFT;

  canvas->pushTextColor(textcolor);
  canvas->setTextOpaque(FALSE);
  canvas->pushTextSize(getFontSize());

  firstItemVisible = NULL;
  lastItemVisible = NULL;

  ensureMetricsValid();

  //drawSubItems(canvas, x, &y, items, r.top, r.bottom, 0);
  drawItems(canvas);

  canvas->popTextColor();
  canvas->popTextSize();

  canvas->selectClipRgn(NULL); // reset cliping region - NEEDED;

//  delete clip; uncomment if necessary

  return 1;
}

void TreeWnd::drawItems(Canvas *canvas) {

  RECT r, c, ir;
  RegionI *orig=NULL;
  getClientRect(&r);
  if (!canvas->getClipBox(&c)) {
    getClientRect(&c);
    orig = new RegionI(&c);
  } else
    orig = new RegionI(canvas);

  int first = ((c.top-r.top) + getScrollY() - Y_SHIFT) / itemHeight;
  int last = ((c.bottom-r.top) + getScrollY() - Y_SHIFT) / itemHeight + 1;
  POINT pt;
  TreeItem *item;
  BOOL hastab;

  for (int i=first;i<=last;i++) {

    if (i >= drawList.getNumItems()) break;

    item = drawList[i];
    if (!item) continue;
    item->getCurRect(&ir);
    pt.x = r.left + X_SHIFT+item->getIndent()*itemHeight - getScrollX();//ir.left;
    pt.y = ir.top;

    // if we need the +/- icon and any of the link lines, draw them
	  if (item->needTab()) {
      RECT _r={pt.x-itemHeight, pt.y, pt.x, pt.y+itemHeight};
      Bitmap * tab = item->isCollapsed() ? &tabClosed : &tabOpen;
	    if ( tab->valid() )
        tab->stretchToRectAlpha(canvas, &_r);
	    hastab=TRUE;
	  } else hastab = FALSE;

	  int indent = item->getIndent();

    if ( drawlines )
	  for (int j=0;j<indent+1;j++) {
      RECT _r={pt.x-itemHeight*(j+1), pt.y, pt.x-itemHeight*j, pt.y+itemHeight};
      int l = getLinkLine(item, j);
		  if (l == (LINK_RIGHT | LINK_TOP)) {
		    ((hastab && j == 0) ? linkTabTopRight : linkTopRight).stretchToRectAlpha(canvas, &_r);
		  }
		  if (l == (LINK_RIGHT | LINK_TOP | LINK_BOTTOM)) {
		    ((hastab && j == 0) ? linkTabTopRightBottom : linkTopRightBottom).stretchToRectAlpha(canvas, &_r);
		  }
		  if (l == (LINK_BOTTOM | LINK_TOP)) {
		    ((hastab && j == 0) ? linkTabTopBottom : linkTopBottom).stretchToRectAlpha(canvas, &_r);
		  }
	  }

    item->customDraw(canvas, pt, itemHeight, (pt.x+getScrollX())-r.left-X_SHIFT, r);
  }

  delete orig;
}

TreeItem *TreeWnd::hitTest(int x, int y) {
  POINT pt={x,y};
  return hitTest(pt);
}

TreeItem *TreeWnd::hitTest(POINT pt) {
  RECT r, ir;
  getClientRect(&r);

  int first = (getScrollY() - Y_SHIFT) / itemHeight;
  int last = ((r.bottom-r.top) + getScrollY() - Y_SHIFT) / itemHeight + 1;
  TreeItem *item;

  for (int i=first;i<=last;i++) {

    if (i >= drawList.getNumItems()) break;

    item = drawList.enumItem(i);
    
    if (item) {
      item->getCurRect(&ir);
      if (Std::pointInRect(&ir, pt) && item->isHitTestable())
        return item;
    }
  }

  return NULL;  
}

void TreeWnd::getMetrics(int *numItemsShown, int *mWidth) {
  *mWidth=0;
  *numItemsShown=0;
  drawList.removeAll();
  countSubItems(drawList, &items, X_SHIFT, numItemsShown, mWidth, 0);
}

void TreeWnd::countSubItems(PtrList<TreeItem> &drawlist, TreeItemList *_list, int indent, int *count, int *maxwidth, int z) {

  int w;
  TreeItemList &list = *_list;

  for (int i=0;i<list.getNumItems();i++) {

    TreeItem *nextitem = list[i];

    w = nextitem->getItemWidth(itemHeight, indent-X_SHIFT);
    if (indent+w > *maxwidth) *maxwidth = w+indent;

    int j = indent-(nextitem->needTab() ? itemHeight : 0);
    int k;
    k = indent + w /*+ itemHeight*/;

    nextitem->setCurRect(j, Y_SHIFT+(*count * itemHeight), k, Y_SHIFT+((*count+1) * itemHeight), z);
    (*count)++;

    drawlist.addItem(nextitem);

    if (nextitem->isExpanded())
      countSubItems(drawlist, nextitem->subitems, indent+CHILD_INDENT, count, maxwidth, z+1);
  }
}

void TreeWnd::timerCallback(int c) {
  switch (c) {
    case TIMER_EDIT_ID:
      prevbdownitem = NULL;
      killTimer(TIMER_EDIT_ID);
      break;
    default:
      TREEWND_PARENT::timerCallback(c);
  }
}

int TreeWnd::onLeftButtonDown(int x, int y) {

  if (edited) {
    delete editwnd; editwnd = NULL;
    endEditLabel(editbuffer);
  }

  POINT pt={x,y};
  TreeItem *item = hitTest(pt);
 
  if (item) {
    mousedown_item = item;
    mousedown_anchor.x = pt.x;
    mousedown_anchor.y = pt.y;
    mousedown_dragdone = FALSE;
    // only do expand/collapse if was already selected
    setCurItem(item, autocollapse?(curSelected == item):0, FALSE);
    beginCapture();
  }

  return 1;
}

int TreeWnd::onLeftButtonUp(int x, int y) {
  if (getCapture())
    endCapture();
  TREEWND_PARENT::onLeftButtonUp(x, y);
  POINT pt={x,y};
  TreeItem *item = hitTest(pt);
  if (autoedit && item == mousedown_item && item == prevbdownitem) 
    setCurItem(item, FALSE, TRUE);
  else
    if (autoedit) {
      prevbdownitem = getCurItem();
      setTimer(TIMER_EDIT_ID, TIMER_EDIT_DELAY);
    }

  mousedown_item = NULL;
  return 1;
}

int TreeWnd::onRightButtonUp(int x, int y){
  TREEWND_PARENT::onRightButtonUp(x, y);
  POINT pos={x,y};

  TreeItem *ti = hitTest(pos);
  if (ti != NULL) {
    selectItem(ti);
    if (onPreItemContextMenu(ti, x, y) == 0) {
      int ret = ti->onContextMenu(x, y);
      onPostItemContextMenu(ti, x, y, ret);
      return ret;
    }
    return 1;
  } else {
    return onContextMenu(x, y);
  }
}

int TreeWnd::onMouseMove(int x, int y) {

  TREEWND_PARENT::onMouseMove(x, y);

  POINT pt={x,y};

  if (mousedown_item) {
    if (!mousedown_dragdone && (ABS(pt.x - mousedown_anchor.x) > DRAG_THRESHOLD || ABS(pt.y - mousedown_anchor.y) > DRAG_THRESHOLD)) {
      mousedown_dragdone = TRUE;
      if (getCapture())
        endCapture();
      onBeginDrag(mousedown_item);
    }
  } else {
    TreeItem *item = hitTest(pt);
    if (item) {
      if (tipitem != item) {
        tipitem = item;
        RECT r;
        RECT c;
        getClientRect(&c);
        item->getCurRect(&r);
        const char *tt = item->getTip();
        if (tt != NULL && *tt != '\0')
          setLiveTip(tt);
        else if (r.right > c.right || r.bottom > c.bottom || r.top < c.top || r.left < c.left)
          setLiveTip(item->getLabel());
        else
          setLiveTip(NULL);
      }
    } else {
      setLiveTip(NULL);
    }
  }

  return 1;
}

int TreeWnd::onLeftButtonDblClk(int x, int y) {
  TreeItem *item = hitTest(x, y);
  if (item == NULL) return 0;
  return item->onLeftDoubleClick();
}

int TreeWnd::onRightButtonDblClk(int x, int y) {
  TreeItem *item = hitTest(x, y);
  if (item == NULL) return 0;
  return item->onRightDoubleClick();
}

void TreeWnd::setLiveTip(const char *tip) {
  if (!tip) {
    setTip(oldtip);
    oldtip = "";
    return;
  }
  oldtip = TREEWND_PARENT::getTip();
  setTip(tip);
}

int TreeWnd::onBeginDrag(TreeItem *treeitem) {
  char title[WA_MAX_PATH]="";
  // item calls addDragItem()
  if (!treeitem->onBeginDrag(title)) return 0;
  ASSERT(draggedItem == NULL);
  draggedItem = treeitem;
  if (*title != 0) setSuggestedDropTitle(title);
  handleDrag();
  return 1;
}

int TreeWnd::dragEnter(RootWnd *sourceWnd) {
  // uh... we don't know yet, but we can accept drops in general
  hitItem = NULL;
  return 1;
}

int TreeWnd::dragOver(int x, int y, RootWnd *sourceWnd) {
  POINT pos={x,y};
  screenToClient(&pos);
  TreeItem *prevItem;

  prevItem = hitItem;
  hitItem = hitTest(pos);

  // no dropping on yourself! :)
  if (hitItem == draggedItem) hitItem = NULL;

  // unselect previous item
  if (prevItem != hitItem && prevItem != NULL) {
    unhiliteDropItem(prevItem);
    repaint(); // commit invalidation of unhilited item so no trouble with scrolling
    prevItem->dragLeave(sourceWnd);
  }


  RECT r;
  getClientRect(&r);
  if (pos.y < r.top + 16) {
    if (getScrollY() >= 0) {
      scrollToY(MAX(0, getScrollY()-itemHeight));
    }
  } else if (pos.y > r.bottom - 16) {
    if (getScrollY() < getMaxScrollY()) {
      scrollToY(MIN(getMaxScrollY(), getScrollY()+itemHeight));
    }
  }

  if (hitItem != NULL) {
    // hilight it
    if (prevItem != hitItem) {
      hiliteDropItem(hitItem);
      repaint(); // commit invalidation of hilited so no trouble with scrolling
    }
  } 

  if (hitItem == NULL) return defaultDragOver(x, y, sourceWnd);

  // ask the item if it can really accept such a drop
  return hitItem->dragOver(sourceWnd);
}

int TreeWnd::dragLeave(RootWnd *sourceWnd) {
  if (hitItem != NULL) {
    unhiliteDropItem(hitItem);
    hitItem->dragLeave(sourceWnd);
  }
  hitItem = NULL;
  return 1;
}

int TreeWnd::dragDrop(RootWnd *sourceWnd, int x, int y) {
  int res;
  if (hitItem == NULL) return defaultDragDrop(sourceWnd, x, y);
  // unhilite the dest
  unhiliteDropItem(hitItem);
  // the actual drop
  res = hitItem->dragDrop(sourceWnd);
  if (res) {
    onItemRecvDrop(hitItem);
  }
  hitItem = NULL;
  return res;
}

int TreeWnd::dragComplete(int success) {
  int ret;
  ASSERT(draggedItem != NULL);
  ret = draggedItem->dragComplete(success);
  draggedItem = NULL;
  return ret;
}

void TreeItem::setTip(const char *tip) {
  tooltip = tip;
}

const char *TreeItem::getTip() {
  return tooltip;
}

void TreeWnd::hiliteDropItem(TreeItem *item) {
  if (item)
    item->setHilitedDrop(TRUE);
}

void TreeWnd::hiliteItem(TreeItem *item) {
  if (item)
    item->setHilited(TRUE);
}

void TreeWnd::selectItem(TreeItem *item) {
  setCurItem(item, FALSE);
}

void TreeWnd::selectItemDeferred(TreeItem *item) {
  postDeferredCallback(DC_SETITEM, (int)item);
}

void TreeWnd::delItemDeferred(TreeItem *item) {
  postDeferredCallback(DC_DELITEM, (int)item);
}

void TreeWnd::unhiliteItem(TreeItem *item) {
  if (item)
    item->setHilited(FALSE);
}

void TreeWnd::unhiliteDropItem(TreeItem *item) {
  if (item)
    item->setHilitedDrop(FALSE);
}

void TreeWnd::setCurItem(TreeItem *item, BOOL expandCollapse, BOOL editifselected) {
  if (curSelected && curSelected != item) {
    onDeselectItem(curSelected);
    curSelected->setSelected(FALSE);
  }
  if (item) {
    curSelected = item;
    onSelectItem(curSelected);
    item->setSelected(TRUE, expandCollapse, editifselected);
    setSlidersPosition();
  }
}

// Returns the current tree width in pixels
int TreeWnd::getContentsWidth() {
  ensureMetricsValid();
  return maxWidth;
}

// Returns the current tree height in pixels
int TreeWnd::getContentsHeight() {
  ensureMetricsValid();
  return maxHeight;
}

void TreeWnd::ensureMetricsValid() {
  if (metrics_ok) return;
  int n;
  getMetrics(&n, &maxWidth);
  maxWidth += X_SHIFT*2;
  maxHeight = n*itemHeight+Y_SHIFT*2;
  metrics_ok = TRUE;
  setSlidersPosition();
}

// Gets notification from sliders
int TreeWnd::childNotify(RootWnd *child, int msg, int param1, int param2) {
  switch (msg) {
    case ChildNotify::EDITWND_ENTER_PRESSED:
      if (child == editwnd && editwnd != NULL) {
        endEditLabel(editbuffer);
        return 1;
      }
      break;
    case ChildNotify::EDITWND_CANCEL_PRESSED:
      if (child == editwnd && editwnd != NULL) {
        cancelEditLabel();
        return 1;
      }
      break;
    case ChildNotify::EDITWND_DATA_MODIFIED:
      if (child == editwnd && editwnd != NULL) {
        editUpdate();
        return 1;
      }
      break;
  }

  return TREEWND_PARENT::childNotify(child, msg, param1, param2);
}

void TreeWnd::editUpdate() {
  ASSERT(edited != NULL && editwnd != NULL);
  if (!edited || !editwnd) return;
  int w = editwnd->getTextLength()+16;
  RECT i, r, e;
  edited->getCurRect(&i);
  getClientRect(&r);
  editwnd->getClientRect(&e);
  e.left += i.left;
  e.right += i.left;
  e.top += i.top;
  e.bottom += i.top;
  e.right = i.left+w;
  e.right = MIN<int>(r.right - X_SHIFT, e.right);
  editwnd->resize(&e);
  editwnd->invalidate();
}

TreeItem *TreeWnd::addTreeItem(TreeItem *item, TreeItem *par, int _sorted, int haschildtab) {

  ASSERT(item != NULL);
  ASSERTPR(item->getTree() == NULL, "can't transplant TreeItems");
  ASSERTPR(item->getLabel() != NULL, "tree items must have a label to be inserted");

  item->setSorted(_sorted);
  item->setChildTab(haschildtab ? TAB_AUTO : TAB_NO/*&& par != NULL*/);
  item->setTree(this);
  item->linkTo(par);

  if (par == NULL)
    items.addItem(item);

  all_items.addItem(item);

  metrics_ok = FALSE;

  if (redraw)
    invalidate();

  item->onTreeAdd();

  return item;
}

int TreeWnd::removeTreeItem(TreeItem *item) {
  ASSERT(item != NULL);
  ASSERT(item->getTree() == this);
  if (item->isSelected()) item->setSelected(FALSE);
  if (curSelected == item) curSelected = NULL;
//CUT  item->deleteSubitems();
  TreeItem *par = item->getParent();
  if (!par) { // is root item ?
    ASSERT(items.haveItem(item));
    items.removeItem(item);
  } else {
    if (!par->removeSubitem(item))
      return 0;
  }
  all_items.removeItem(item);
  metrics_ok = FALSE;
  drawList.removeItem(item);
  if (redraw)
    invalidate();

  item->setTree(NULL);
  item->onTreeRemove();

  if (par != NULL) par->onChildItemRemove(item);
  
  return 1;
}

void TreeWnd::moveTreeItem(TreeItem *item, TreeItem *newparent) {
  ASSERT(item != NULL);
  ASSERTPR(item->getTree() == this, "can't move between trees (fucks up Freelist)");
  removeTreeItem(item);
  addTreeItem(item, newparent, item->subitems->getAutoSort(), item->childTab);
}

void TreeWnd::deleteAllItems() {
  BOOL save_redraw = redraw;
  setRedraw(FALSE);
  
  TreeItem *item;
  while ((item = enumRootItem(0)) != NULL)
    delete item;

  setRedraw(save_redraw);
}

void TreeWnd::setSorted(int dosort) {
  items.setAutoSort(dosort);
}

BOOL TreeWnd::getSorted() {
  return items.getAutoSort();
}

void TreeWnd::sortTreeItems() {
  items.sort(TRUE);
  metrics_ok = FALSE;
  if (redraw)
    invalidate();
}

TreeItem *TreeWnd::getSibling(TreeItem *item) {
  for (int i=0;i<items.getNumItems();i++) {
    if (items[i] == item) {
      if (i == items.getNumItems()-1) return NULL;
      return items[i+1];
    }
  }
  return NULL;
}

void TreeWnd::setAutoCollapse(BOOL doautocollase) {
  autocollapse=doautocollase;
}

int TreeWnd::onContextMenu(int x, int y) {
  POINT pos={x,y};
  screenToClient(&pos);
  TreeItem *ti = hitTest(pos);
  if (ti != NULL) {
    selectItem(ti);
    return ti->onContextMenu(x, y);
  }
  return 0;
}

int TreeWnd::onDeferredCallback(int param1, int param2) {
  switch (param1) {
    case DC_SETITEM:
      setCurItem((TreeItem *)param2, FALSE);
    return 1;
    case DC_DELITEM:
      delete (TreeItem *)param2;
    return 1;
    case DC_EXPAND:
      expandItem((TreeItem *)param2);
    return 1;
    case DC_COLLAPSE:
      collapseItem((TreeItem *)param2);
    return 1;
  }
  return 0;
}

int TreeWnd::getNumRootItems() {
  return items.getNumItems();
}

TreeItem *TreeWnd::enumRootItem(int which) {
  return items[which];
}

void TreeWnd::invalidateMetrics() {
  metrics_ok = FALSE;
}

int TreeWnd::getLinkLine(TreeItem *item, int level) {

  ASSERT(item != NULL);

  int l = 0;
  int r = 0;

  if (item->parent == NULL)
    return 0;

  TreeItem *cur=item;

  while (cur->getParent() && l < level) {
    cur = cur->getParent();
    l++;
  }

  if (cur->getSibling()) r |= LINK_BOTTOM | LINK_TOP;
  if (level == 0) r |= LINK_RIGHT;
  if (level == 0 && cur->getParent()) r |= LINK_TOP;

  return r;

}

int TreeWnd::onMouseWheelDown(int clicked, int lines) {
  if (!clicked)
    scrollToY(MIN(getMaxScrollY(), getScrollY()+itemHeight));
  else
    scrollToX(MIN(getMaxScrollX(), getScrollX()+itemHeight));
  return 1;
}

int TreeWnd::onMouseWheelUp(int clicked, int lines) {
  if (!clicked)
    scrollToY(MAX(0, getScrollY()-itemHeight));
  else
    scrollToX(MAX(0, getScrollX()-itemHeight));
  return 1;
}

int TreeWnd::expandItem(TreeItem *item) {
  ASSERT(item != NULL);

  return item->expand();
}

void TreeWnd::expandItemDeferred(TreeItem *item) {
  postDeferredCallback(DC_EXPAND, (int)item);
}

int TreeWnd::collapseItem(TreeItem *item) {
  ASSERT(item != NULL);

  return item->collapse();
}

void TreeWnd::collapseItemDeferred(TreeItem *item) {
  postDeferredCallback(DC_COLLAPSE, (int)item);
}

TreeItem *TreeWnd::getCurItem() {
  return curSelected;
}

int TreeWnd::getItemRect(TreeItem *item, RECT *r) {
  ASSERT(item != NULL);

  return item->getCurRect(r);
}

void TreeWnd::editItemLabel(TreeItem *item) {

  if (edited) {
    edited->setEdition(FALSE);
    edited->invalidate();
  }


  ASSERT(item != NULL);
  if (item == NULL) return;

  if (item->onBeginLabelEdit()) return;
  item->setEdition(TRUE);
  edited = item;

  editwnd = new EditWnd();
  editwnd->setModal(TRUE);
  editwnd->setAutoSelect(TRUE);
  editwnd->setStartHidden(TRUE);
  editwnd->init(getOsModuleHandle(), getOsWindowHandle());
  editwnd->setParent(this);
  RECT r;
  edited->getCurRect(&r);
  RECT cr;
  getClientRect(&cr);
  r.right = cr.right;
  if (r.bottom - r.top < 24) r.bottom = r.top + 24;
  editwnd->resize(&r);
  STRCPY(editbuffer, edited->getLabel());
  editwnd->setBuffer(editbuffer, 255);
  editUpdate();
  editwnd->setVisible(TRUE);
}

void TreeWnd::endEditLabel(const char *newlabel) {
  editwnd = NULL; // editwnd self destructs
  if (edited->onEndLabelEdit(newlabel))
    edited->setLabel(newlabel);
  edited->setEdition(FALSE);
  edited->invalidate();
  onLabelChange(edited);
  edited = NULL;
  invalidateMetrics();
  setSlidersPosition();
}

void TreeWnd::cancelEditLabel(int destroyit) {
  ASSERT(edited != NULL);
  if (!edited) return;

  if (destroyit)
    delete editwnd;

  editwnd = NULL; // editwnd self destructs (update> except if destroyit for cancelling from treewnd)
  edited->setEdition(FALSE);
  edited->invalidate();
  edited = NULL;
}

void TreeWnd::setAutoEdit(int ae) {
  autoedit = ae;
}

int TreeWnd::getAutoEdit() {
  return autoedit;
}

TreeItem *TreeWnd::getByLabel(TreeItem *item, const char *name) {
  TreeItem *ti;
  // handle root-level searching
  if (item == NULL) {
    int n = getNumRootItems();
    for (int i = 0; i < n; i++) {
      ti = enumRootItem(i);
      if (STREQL(name, ti->getLabel())) return ti;
      ti = getByLabel(ti, name);
      if (ti) return ti;
    }
    return NULL;
  }

  // check the given item
  if (STREQL(name, item->getLabel())) return item;

  // depth first search
  ti = item->getChild();
  if (ti != NULL) {
    ti = getByLabel(ti, name);
    if (ti != NULL) return ti;
  }

  // recursively check siblings
  ti = item->getSibling();
  if (ti != NULL) ti = getByLabel(ti, name);

  return ti;
}

int TreeWnd::onGetFocus() {
  int r = TREEWND_PARENT::onGetFocus();

#if 0
DebugString("yay got focus");
  TreeItem *ti = getCurItem();
  if (ti != NULL) {
    ti->setSelected(FALSE);
    selectItemDeferred(ti);
  }
#endif

  return r;
}

int TreeWnd::onKillFocus() {

  TREEWND_PARENT::onKillFocus();
  mousedown_item=NULL;
/*  if (edited)
    cancelEditLabel();*/
#if 0
DebugString("no mo focus");
#endif
    
  return 1;
}

int TreeWnd::onChar(unsigned int c) {
  int r = 0;

  if (c == 27) {
    if (edited)
      cancelEditLabel(1);
  }

  if (curSelected != NULL && (r = curSelected->onChar(c)) != 0) return r;

  char b = TOUPPER(c);
  if (b >= 'A' && b <= 'Z') {
    jumpToNext(b);
    r = 1;
  }

  return r ? r : TREEWND_PARENT::onChar(c);
}

int TreeWnd::getNumVisibleChildItems(TreeItem *c) {
  int nb=0;
  for(int i=0;i<c->getNumChildren();i++) {
    TreeItem *t=c->getNthChild(i);
    if(t->hasSubItems() && t->isExpanded())
      nb+=getNumVisibleChildItems(t);
    nb++;
  }
  return nb;
}

int TreeWnd::getNumVisibleItems() {
  int nb=0;
  for(int i=0;i<items.getNumItems();i++) {
    TreeItem *t=items.enumItem(i);
    if(t->hasSubItems() && t->isExpanded())
      nb+=getNumVisibleChildItems(t);
    nb++;
  }
  return nb;
}

TreeItem *TreeWnd::enumVisibleChildItems(TreeItem *c, int n) {
  int nb=0;
  for(int i=0;i<c->getNumChildren();i++) {
    TreeItem *t=c->getNthChild(i);
    if(nb==n) return t;
    if(t->hasSubItems() && t->isExpanded()) {
      TreeItem *t2=enumVisibleChildItems(t, n-nb-1);
      if(t2) return t2;
      nb+=getNumVisibleChildItems(t);
    }
    nb++;
  }
  return NULL;
}

TreeItem *TreeWnd::enumVisibleItems(int n) {
  int nb=0;
  for(int i=0;i<items.getNumItems();i++) {
    TreeItem *t=items.enumItem(i);
    if(nb==n) return t;
    if(t->hasSubItems() && t->isExpanded()) {
      TreeItem *t2=enumVisibleChildItems(t, n-nb-1);
      if(t2) return t2;
      nb+=getNumVisibleChildItems(t);
    }
    nb++;
  }
  return NULL;
}

int TreeWnd::findChildItem(TreeItem *c, TreeItem *i, int *nb) {
  for(int j=0;j<c->getNumChildren();j++) {
    TreeItem *t=c->getNthChild(j); (*nb)++;
    if (t == i) return *nb;
    if(t->hasSubItems() && t->isExpanded()) {
      int n = findChildItem(t, i, nb);
      if (n != -1) return *nb;
    }
  }
  return -1;
}

int TreeWnd::findItem(TreeItem *i) {
  int nb=-1;
  for(int j=0;j<items.getNumItems();j++) {
    TreeItem *t=items.enumItem(j); nb++;
    if (t == i) return nb;
    if(t->hasSubItems() && t->isExpanded()) {
      int n = findChildItem(t, i, &nb);
      if (n != -1) return nb;
    }
  }
  return -1;
}


TreeItem *TreeWnd::enumAllItems(int n) {
  return all_items[n];
}

int TreeWnd::onKeyDown(int keycode) {
  switch(keycode) {
  case 113: {
    TreeItem *item = getCurItem();
    if (item)
      item->editLabel();
    return 1;
    }
  case VK_UP: {
    TreeItem *t=getCurItem();
    int l=getNumVisibleItems();
    if (t == NULL) {
      if (l > 0) setCurItem(enumVisibleItems(getNumVisibleItems()-1), FALSE, FALSE);
    } else {
      for(int i=0;i<l;i++)
        if(enumVisibleItems(i)==t) {
          if(i-1>=0) {
            TreeItem *t2=enumVisibleItems(i-1);
            if(t2) setCurItem(t2,FALSE,FALSE);
          }
        }
      }
    return 1;
    }
  case VK_DOWN: {
    TreeItem *t=getCurItem();
    int l=getNumVisibleItems();
    if (t == NULL) {
      if (l > 0) setCurItem(enumVisibleItems(0), FALSE, FALSE);
    } else {
      for(int i=0;i<l;i++)
        if(enumVisibleItems(i)==t) {
          TreeItem *t2=enumVisibleItems(i+1);
          if(t2) setCurItem(t2,FALSE,FALSE);
        }
      }
    return 1;
    }
  case VK_PRIOR: {
    TreeItem *t=getCurItem();
    int l=getNumVisibleItems();
    for(int i=0;i<l;i++)
      if(enumVisibleItems(i)==t) {
        int a=MAX(i-5,0);
        TreeItem *t2=enumVisibleItems(a);
        if(t2) setCurItem(t2,FALSE,FALSE);
      }
    return 1;
    }
  case VK_NEXT: {
    TreeItem *t=getCurItem();
    int l=getNumVisibleItems();
    for(int i=0;i<l;i++)
      if(enumVisibleItems(i)==t) {
        int a=MIN(i+5,l-1);
        TreeItem *t2=enumVisibleItems(a);
        if(t2) setCurItem(t2,FALSE,FALSE);
      }
    return 1;
    }
  case VK_HOME: {
    TreeItem *t=enumVisibleItems(0);
    if(t) setCurItem(t,FALSE,FALSE);
    return 1;
    }
  case VK_END: {
    TreeItem *t=enumVisibleItems(getNumVisibleItems()-1);
    if(t) setCurItem(t,FALSE,FALSE);
    return 1;
    }
  case VK_LEFT: {
    TreeItem *t=getCurItem();
    if(t) t->collapse();
    return 1;
    }
  case VK_RIGHT: {
    TreeItem *t=getCurItem();
    if(t) t->expand();
    return 1;
    }
  }
  
  return TREEWND_PARENT::onKeyDown(keycode);
}

void TreeWnd::jumpToNext(char c) {
  firstFound=FALSE;
  if (jumpToNextSubItems(&items, c)) return;
  firstFound=TRUE;
  jumpToNextSubItems(&items, c);
}

int TreeWnd::jumpToNextSubItems(TreeItemList *list, char c) {

  for (int i=0;i<list->getNumItems();i++) {

    TreeItem *nextitem = list->enumItem(i);
    const char *l = nextitem->getLabel();
    char b = l ? TOUPPER(*l) : 0;
    if (b == c && firstFound) {
      selectItem(nextitem);
      nextitem->ensureVisible();
      return 1;
    }

    if (nextitem->isSelected()) firstFound = TRUE;

    if (nextitem->isExpanded())
      if (jumpToNextSubItems(nextitem->subitems, c)) return 1;
  }
  return 0;
}

void TreeWnd::ensureItemVisible(TreeItem *item) {
  ASSERT(item != NULL);

  // walk the parent tree to make sure item is visible
  for (TreeItem *cur = item->getParent(); cur; cur = cur->getParent()) {
    if (cur->isCollapsed()) cur->expand();
  }

  RECT r;
  RECT c;
  item->getCurRect(&r);
  getClientRect(&c);
  if (r.top < c.top || r.bottom > c.bottom) {
    if (r.top + (c.bottom - c.top) <= getContentsHeight())
      scrollToY(r.top);
    else {
      scrollToY(getContentsHeight()-(c.bottom-c.top));
    }
  }
}

void TreeWnd::setHilitedColor(const char *colorname) {
  // we have to store it in a String because SkinColor does not make a copy
  hilitedColorName = colorname;
  hilitedColor = hilitedColorName;
}

COLORREF TreeWnd::getHilitedColor() {
  return hilitedColor;
}

int TreeWnd::compareItem(TreeItem *p1, TreeItem *p2) {
  int r = STRCMP(p1->getLabel(), p2->getLabel());
  if (r == 0) return CMP3(p1, p2);
  return r;
}

int TreeWnd::setFontSize(int newsize) {
  TREEWND_PARENT::setFontSize(newsize);
  if (newsize >= 0) textsize = newsize;
  SysCanvas c;
  c.pushTextSize(getFontSize());
  itemHeight = c.getTextHeight();
  c.popTextSize();
  redraw = 1;
  metrics_ok = 0;
  invalidate();
  return 1;
}

int TreeWnd::getFontSize() {
#ifndef WASABINOMAINAPI
  return textsize + api->metrics_getDelta();
#else
  //MULTIAPI-FIXME: not handling delta
  return textsize;
#endif
}

int TreeWnd::accessibility_getNumItems() {
  return getNumVisibleItems();
}

const char *TreeWnd::accessibility_getItemName(int n) {
  TreeItem *i = enumVisibleItems(n);
  if (!i) return NULL;
  return i->getLabel();
}

int TreeWnd::accessibility_getItemRole(int n) {
  return ROLE_SYSTEM_OUTLINEITEM;
}

int TreeWnd::accessibility_getItemState(int n) {
  TreeItem *i = enumVisibleItems(n);
  if (i == NULL) return 0;
  int selected = i->isSelected() ? STATE_SYSTEM_SELECTED : 0;
  int f = 0;
  if (i->hasSubItems()) {
    if (i->isCollapsed()) f |= STATE_SYSTEM_COLLAPSED;
    if (i->isExpanded()) f |= STATE_SYSTEM_EXPANDED;
  }
  return STATE_SYSTEM_SELECTABLE| selected | f;
}

void TreeWnd::accessibility_getItemRect(int n, RECT *r) {
  TreeItem *i = enumVisibleItems(n);
  if (i == NULL) return;
  getItemRect(i, r);
}

int TreeWnd::accessibility_getRole() {
  return ROLE_SYSTEM_OUTLINE;
}

const char *TreeWnd::accessibility_getItemValue(int in) {
  TreeItem *i = enumVisibleItems(in);
  if (i == NULL) return NULL;
  int n = 0;
  while (n < 0xFFFF && i->getParent()) { n++; i = i->getParent(); }
  accValue.printf("%d", n);
  return accValue;
}

int TreeWnd::accessibility_isLeaf() {
  return 1;
}

void TreeWnd::onSelectItem(TreeItem *i) {
#ifdef WASABI_API_ACCESSIBILITY
  Accessible *a = getAccessibleObject();
  if (a != NULL) 
    a->onGetFocus(findItem(i));
#endif
}

void TreeWnd::onDeselectItem(TreeItem *i) {
}


////////////////////////////////////////////////////////////////////////////////////
// TreeItem
////////////////////////////////////////////////////////////////////////////////////

TreeItem::TreeItem(const char *label) {
  parent=NULL;
  subitems = new TreeItemList();
  MEMZERO(&curRect, sizeof(RECT));
  childTab = TAB_AUTO;
  tree = NULL;
  expandStatus = STATUS_COLLAPSED;
  icon = NULL;
  _z = 0;

  if (label != NULL) setLabel(label);

  selected = FALSE;
  hilitedDrop = FALSE;
  hilited = FALSE;
  being_edited = FALSE;

  setSorted(TRUE);
}

TreeItem::~TreeItem() {
  // the subitem will call parent tree which will remove item from our list
  deleteSubitems();

  // remove from parent tree
  if (tree) tree->removeTreeItem(this);

  delete subitems;

  delete icon;
}

void TreeItem::deleteSubitems() {
  while (subitems->getNumItems() > 0) {
    delete subitems->enumItem(0);
  }
}

void TreeItem::setSorted(int issorted) {
  subitems->setAutoSort(issorted);
}

void TreeItem::setChildTab(int haschildtab) {
  childTab = haschildtab;
}

void TreeItem::linkTo(TreeItem *par) {
  parent = par;

  if (par == NULL) return;

  par->addSubItem(this);
}

void TreeItem::addSubItem(TreeItem *item) {
  subitems->addItem(item);
}

int TreeItem::removeSubitem(TreeItem *item) {
  if (subitems->searchItem(item) == -1) return 0;
  subitems->removeItem(item);
  if (tree->redraw)
    tree->invalidate();
  return 1;
}

TreeItem *TreeItem::getChild() {
  return subitems->getFirst();
}

TreeItem *TreeItem::getChildSibling(TreeItem *item) { // locate item in children and return its sibling
  for (int i=0;i<subitems->getNumItems();i++) {
    if (subitems->enumItem(i) == item) {
      if (i == subitems->getNumItems()-1) return NULL;
      return subitems->enumItem(i+1);
    }
  }
return NULL;
}

TreeItem *TreeItem::getSibling() { // returns next item
  if (!parent)
    return tree->getSibling(this);
  else
    return parent->getChildSibling(this);
}

void TreeItem::setTree(TreeWnd *newtree) {
  tree = newtree;
  // recursively reset tree for children, if any
  for (int i = 0; ; i++) {
    TreeItem *item = getNthChild(i);
    if (item == NULL) break;
    item->setTree(tree);
  }
}

void TreeItem::ensureVisible() {
  if (tree) tree->ensureItemVisible(this);
}

const char *TreeItem::getLabel() {
  return label;
}

void TreeItem::setLabel(const char *newlabel) {
  label = newlabel;
  if (newlabel) {
    if (tree)
      tree->invalidateMetrics();
    invalidate();
  }
}

int TreeItem::customDraw(Canvas *canvas, const POINT &pt, int txtHeight, int indentation, const RECT &clientRect) {

  if (being_edited) return 0;

  Bitmap *icon = getIcon();

  int cw = clientRect.right - clientRect.left;

  int iconw = MIN(icon && !icon->isInvalid() ? icon->getWidth() : 0, cw);
  iconw = MIN(iconw, txtHeight);

  if (isSelected() || isHilitedDrop()) {
    RECT r;
    r.left = pt.x;
    r.top = pt.y;
    //r.right = r.left + canvas->getTextWidth(label)+2+(icon ? txtHeight : 0);
    r.right = r.left + canvas->getTextWidth(label)+2+iconw;
    r.bottom = r.top + txtHeight;
    canvas->fillRect(&r, isHilitedDrop() ? drophilitecolor : selectedcolor);
  }

  if (isHilited()) {
    RECT r;
    r.left = pt.x;
    r.top = pt.y;
    r.right = r.left + canvas->getTextWidth(label)+2+iconw;
    r.bottom = r.top + txtHeight;
    canvas->drawRect(&r, 1, tree->getHilitedColor());
  }

  POINT d=pt;

  if (icon && !icon->isInvalid()) {
    RECT i;
    i.left = pt.x+1;
    i.right = i.left + iconw;
    int lh = MIN(icon->getHeight(), txtHeight);
    i.top = pt.y + (txtHeight - lh) / 2;
    i.bottom = i.top + lh;
    icon->stretchToRectAlpha(canvas, &i);
    d.x += iconw;
  }

  canvas->textOut(d.x+1, d.y, label);

  return canvas->getTextWidth(label)+2+iconw;
}

int TreeItem::getItemWidth(int txtHeight, int indentation) {
  Bitmap *icon = getIcon();
  if (!label) return (icon && !icon->isInvalid() ? txtHeight : 0);
  SysCanvas c;
  c.pushTextSize(getTree()->getFontSize());
  int width = c.getTextWidth(label)+2;
  c.popTextSize();
  width += (icon && !icon->isInvalid() ? txtHeight : 0);
  return width;
}

int TreeItem::getNumChildren() {
  return subitems->getNumItems();
}

TreeItem *TreeItem::getNthChild(int nth) {
  if (nth >= subitems->getNumItems()) return NULL;
  return subitems->enumItem(nth);
}

void TreeItem::setCurRect(int x1, int y1, int x2, int y2, int z) {
  curRect.left = x1;
  curRect.right = x2;
  curRect.top = y1;
  curRect.bottom = y2;
  _z = z;
}

int TreeItem::getIndent() {
  return _z;
}

BOOL TreeItem::hasSubItems() {
  return subitems->getNumItems()>0;
}

BOOL TreeItem::needTab() {
  return (childTab == TAB_YES || (childTab == TAB_AUTO && hasSubItems()));
}

BOOL TreeItem::isExpanded() {
  if (!hasSubItems()) return FALSE;
  return expandStatus == STATUS_EXPANDED;
}

BOOL TreeItem::isCollapsed() {
  if (!hasSubItems()) return TRUE;
  return expandStatus == STATUS_COLLAPSED;
}

int TreeItem::getCurRect(RECT *r) {
  r->left = curRect.left-tree->getScrollX();
  r->top = curRect.top-tree->getScrollY();
  r->right = curRect.right-tree->getScrollX();
  r->bottom = curRect.bottom-tree->getScrollY();
  RECT c;
  tree->getClientRect(&c);
  r->top += c.top;
  r->bottom += c.top;
  r->left += c.left;
  r->right += c.left;
  return 1;
}

TreeWnd *TreeItem::getTree() const {
  return tree;
}

void TreeItem::setSelected(int isSelected, BOOL expandCollapse, BOOL editifselected) {
  int wasselected = selected;
  selected = !!isSelected;

  if (selected != wasselected) {
    invalidate();
    tree->repaint();
    if (selected) {
      onSelect();
      ASSERT(tree != NULL);
      tree->onItemSelected(this);
    } else {
      onDeselect();
      ASSERT(tree != NULL);
      tree->onItemDeselected(this);
    }
  } else {
    if (selected && editifselected) {
      editLabel();
    }
  }

  if (expandCollapse) {
    if (isCollapsed())
      expand();
    else
      collapse();
  }
}

void TreeItem::invalidate() {
  if (tree) {
    RECT r;
    getCurRect(&r);
    tree->invalidateRect(&r);
  }
}

BOOL TreeItem::isSelected() {
  return selected;
}

int TreeItem::collapse() {
  int old = expandStatus;

  if (hasSubItems()) {
    if (expandStatus == STATUS_COLLAPSED)
      return 0;
    expandStatus = STATUS_COLLAPSED;
    RECT c;
    tree->getClientRect(&c);
    RECT r;
    getCurRect(&r);
    r.bottom = c.bottom;
    r.left = c.left;
    r.right = c.right;
    if (tree) {
      tree->invalidateRect(&r);
      tree->invalidateMetrics();
    }
  }

  onCollapse();

  return old != expandStatus;
}

int TreeItem::expand() {
  int old = expandStatus;

  if (hasSubItems()) {
    if (expandStatus == STATUS_EXPANDED)
      return 0;
    expandStatus = STATUS_EXPANDED;
    RECT c;
    tree->getClientRect(&c);
    RECT r;
    getCurRect(&r);
    r.bottom = c.bottom;
    r.left = c.left;
    r.right = c.right;
    if (tree) {
      tree->invalidateRect(&r);
      tree->invalidateMetrics();
    }
  }

  onExpand();

  return old != expandStatus;
}

int TreeItem::onContextMenu(int x, int y) { 
  return 0; 
}

void TreeItem::setHilitedDrop(BOOL ishilitedDrop) {
  int washilighted = hilitedDrop;
  hilitedDrop = !!ishilitedDrop;

  if (washilighted != hilitedDrop)
    invalidate();
}

void TreeItem::setHilited(BOOL ishilited) {
  int washilighted = hilited;
  hilited = !!ishilited;

  if (washilighted != hilited)
    invalidate();
}

TreeItem *TreeItem::getParent() {
  return parent;
}

BOOL TreeItem::isHilitedDrop() {
  return hilitedDrop;
}

BOOL TreeItem::isHilited() {
  return hilited;
}

void TreeItem::setIcon(Bitmap *newicon) {
  if (icon) {
    delete icon;
    icon = NULL;
  }
  icon = newicon;
  invalidate();
}

Bitmap *TreeItem::getIcon() {
  return icon;
}

void TreeItem::sortItems() {
  subitems->sort();
}

void TreeItem::editLabel() {
  if (!tree) return;
  tree->editItemLabel(this);
}

int TreeItem::onBeginLabelEdit() {
  return 1; // disable editing by default
}

int TreeItem::onEndLabelEdit(const char *newlabel) {
  return 1; // accept new label by default
}

void TreeItem::setEdition(BOOL isedited) {
  being_edited = !!isedited;
  invalidate();
}

BOOL TreeItem::getEdition() {
  return being_edited;
}

BOOL TreeItem::isSorted() {
  ASSERT(subitems == NULL);
  return subitems->getAutoSort();
}

