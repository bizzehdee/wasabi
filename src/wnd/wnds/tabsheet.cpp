#include "precomp.h"
#include "tabsheet.h"

#include "bfc/std.h"
#include "wnd/notifmsg.h"
#include "wnd/wnds/buttwnd.h"
#include "api/skin/api_skin.h"

#include "buttbar.h"
#include "tabsheetbar.h"
//#include "api/script/c_script/c_text.h"
//#include "api/script/c_script/c_group.h"

TabSheet::TabSheet(int bbtype) {
  leftscroll = rightscroll = NULL;
  background = NULL;
  tabrowmargin = 0;
  active = NULL;
  type = bbtype;
  bb = NULL;
  tsb = NULL;
  content_margin_top = content_margin_right = content_margin_left = content_margin_bottom = 0;
  //contentwnd = NULL;

  if (bbtype == TABSHEET_GROUPS) {
    tsb = new TabSheetBar();
    tsb->setParent(this);
  } else {	// schweitn rulz
    bb = new ButtBar((bbtype == -1) ? ButtBar::NORMAL : bbtype);
    bb->setParent(this);
  }
}

TabSheet::~TabSheet() {
  delete bb;		// kills tabs and child wnds
  delete tsb;
  delete leftscroll;
  delete rightscroll;
  delete background;
  //delete contentwnd; 
}

void TabSheet::setButtonType(int _type) {
  type = _type;
/*
  if (contentwnd != NULL) {
    if (type == ButtBar::STACK)
      contentwnd->setContent("wasabi.tabsheet.content.noborder");
    else
      contentwnd->setContent("wasabi.tabsheet.content");
  }
*/
  if (_type == TABSHEET_GROUPS && bb != NULL) {
    PtrList<BaseWnd> l;
    foreach(tabs)
      l.addItem(tabs.getfor()->getBaseWnd());
      tabs.getfor()->setNoDeleteLinked(1);
    endfor;
    delete bb; bb = NULL;
    tabs.removeAll();
    tsb = new TabSheetBar();
    tsb->setParent(this);
    if (isInited())
      tsb->init(this);
    foreach(l)
      addChild(l.enumItem(foreach_index));
    endfor;
  } else if (_type != TABSHEET_GROUPS && tsb != NULL) {
    PtrList<BaseWnd> l;
    foreach(tabs)
      l.addItem(tabs.getfor()->getBaseWnd());
      tabs.getfor()->setNoDeleteLinked(1);
    endfor;
    delete tsb; tsb = NULL;
    tabs.removeAll();
    bb = new ButtBar((type == -1) ? ButtBar::NORMAL : type);
    bb->setParent(this);
    if (isInited())
      bb->init(this);
    foreach(l)
      addChild(l.enumItem(foreach_index));
    endfor;
  }
  if (bb != NULL) bb->setResizeMode(type);
}

void TabSheet::killChildren() {
  if (bb) {
    delete bb;		// kills tabs and child wnds
    bb = new ButtBar((type == -1) ? ButtBar::NORMAL : type);
    bb->setParent(this);
    bb->init(this);
  }
  if (tsb) {
    delete tsb;		// kills tabs and child wnds
    tsb = new TabSheetBar;
    tsb->setParent(this);
    tsb->init(this);
  }
}

int TabSheet::onInit() {
  TABSHEET_PARENT::onInit();
/*
  contentwnd = new BaseWnd();
  
  if (type == ButtBar::STACK)
    contentwnd->setContent("wasabi.tabsheet.content.noborder");
  else
    contentwnd->setContent("wasabi.tabsheet.content");
  
  contentwnd->setParent(this);
  contentwnd->init(this);
  //rootwndholder_setRootWnd(contentwnd);
*/
  if (leftscroll != NULL) {
    leftscroll->init(this);
    leftscroll->setParent(this);
  }
  if (rightscroll != NULL) {
    rightscroll->init(this);
    rightscroll->setParent(this);
  }

  // init the windows
  foreach(tabs)
    if (foreach_index != 0) tabs.getfor()->getBaseWnd()->setStartHidden(TRUE);
    tabs.getfor()->getBaseWnd()->init(this);
  endfor

  if (bb) {
    bb->setParent(this);
    bb->init(this);	// inits the tabs
  }
  if (tsb) {
    tsb->setParent(this);
    tsb->init(this);	// inits the tabs
  }

  if (tabs.getNumItems() > 0) {
    active = tabs[0]->getBaseWnd();
    //tabs[0]->setHilite(TRUE); // FG: FIX!
  }

  if (isPostOnInit())
    onResize();

  return 1;
}

void TabSheet::getClientRect(RECT *r) {
  TABSHEET_PARENT::getClientRect(r);
  if (bb)
    r->top += bb->getHeight();
  else
    r->top += tsb->getHeight();

  r->left += content_margin_left;
  r->top += content_margin_top;
  r->right -= content_margin_right;
  r->bottom -= content_margin_bottom;
}

void TabSheet::setBackgroundBmp(const char *name) {
  if (background) delete background;
  background = NULL;
  if (name && *name)
    background = new Bitmap(name);
}

Bitmap *TabSheet::getBackgroundBitmap() {
  return background;
}

int TabSheet::onPaint(Canvas *canvas) {

  PaintBltCanvas paintcanvas;
  if (canvas == NULL) {
    if (!paintcanvas.beginPaintNC(this)) return 0;
    canvas = &paintcanvas;
  }
  TABSHEET_PARENT::onPaint(canvas);
 
  RECT r;
  TABSHEET_PARENT::getClientRect(&r);

  if (bb)
    r.bottom = r.top + bb->getHeight();
  else if (tsb)
    r.bottom = r.top + tsb->getHeight();

  RECT br = r;
  if (leftscroll) br.left += leftscroll->getWidth();
  if (rightscroll) br.right -= rightscroll->getWidth();

  if (br.right <= br.left) return 1;

  if (background != NULL) {
#if 0
    int i, x = tilex;
    for (i = 0; ; i++) {
      tile->stretch(canvas, x, 0, tile->getWidth(), tabrowheight);
      x += tile->getWidth();
      if (x >= r.right) break;
    }
#else
#if 0
    if (background->getAlpha()) api->skin_renderBaseTexture(canvas, br);
    background->stretchToRectAlpha(canvas, &br);
#else
    background->stretchToRect(canvas, &br);
#endif
#endif
  } else {
#if 0
    r.top = 0;
    r.bottom = tabrowheight;
    r.left = tilex;
    r.right = tilex + tilew;
    canvas->fillRect(&r, RGB(64, 64, 64));
#else
#ifdef WASABI_COMPILE_SKIN
    WASABI_API_SKIN->skin_renderBaseTexture(NULL,canvas, &r,NULL);
#endif
#endif
  }

  return 1;
}

void TabSheet::setTabRowMargin(int newmargin) {
  ASSERT(newmargin >= 0);
  tabrowmargin = newmargin;
  onResize();
}

int TabSheet::addChild(BaseWnd *newchild, const char *tip) {

  ASSERT(newchild != NULL);

  int first=0;
  if (tabs.getNumItems() == 0) first = 1;

  if (isInited() && !newchild->isInited()) {
    if (!first) newchild->setStartHidden(TRUE);

    RootWnd *holder = this;
/*
    if (contentwnd != NULL) {
      if (contentwnd->getContentRootWnd() != NULL) {
        GuiObject *o = contentwnd->getContent()->guiobject_findObject("content");
        if (o != NULL)
          holder = o->guiobject_getRootWnd();
      }
    }
*/
    newchild->setParent(holder);
    newchild->init(holder);
  }

  if (bb) {
    TabButton *tab = new TabButton(newchild, this, tip);
    tabs.addItem(tab);
    bb->addChild(tab);
  } else if (tsb) {
    GroupTabButton *tab = new GroupTabButton(newchild, this, tip);
    tabs.addItem(tab);
    tsb->addChild(tab);
  }
  if (isInited()) {
    if (first) {
      activateChild(newchild);
    }
  }
  if (isPostOnInit()) onResize(); 
  return tabs.getNumItems()-1;
}

void TabSheet::activateChild(BaseWnd *newactive) {
  BaseWnd *prevactive = active;
  if (newactive == NULL) newactive = active;

  if (prevactive == newactive) return;	// not a switch

#if 0
  RECT r = clientRect();

  int w = r.right - r.left + 1;
  int h = r.bottom - r.top + 1;
#endif

  int prevpos=-1, nextpos=-1;

  for (int i = 0; i < tabs.getNumItems(); i++) {
    if (prevactive == tabs[i]->getBaseWnd()) prevpos = i;
    if (newactive == tabs[i]->getBaseWnd()) nextpos = i;
  }

  if (prevpos != -1) tabs[prevpos]->btn_setHilite(FALSE);
  if (nextpos < tabs.getNumItems()) tabs[nextpos]->btn_setHilite(TRUE);

#if 0
  // reveal tha new winder
  if (newactive!= NULL) newactive->setVisible(TRUE);

  enable(FALSE);
  if (prevactive!= NULL) prevactive->enable(FALSE);
  if (newactive!= NULL) newactive->enable(FALSE);

#define STEPS 6

  // find which window is now active
  for (int c = 0; c < STEPS; c++) {
    int x;
    if (prevpos > nextpos) x = (w * c) / STEPS;	// right to left
    else x = (w * (STEPS - c)) / STEPS;		// left to right
    int y = r.top;
    if (prevpos > nextpos) {
      if (newactive!= NULL) newactive->move(x - w, y);
      if (prevactive!= NULL) prevactive->move(x, y);
    } else {
      if (newactive!= NULL) newactive->move(x, y);
      if (prevactive!= NULL) prevactive->move(x - w, y);
    }
    if (newactive!= NULL) newactive->repaint();
    if (prevactive!= NULL) prevactive->repaint();
    Sleep(15);
  }
#endif

  if (newactive!= NULL) newactive->setVisible(TRUE);

  if (prevactive!= NULL) prevactive->setVisible(FALSE);

#if 0
  enable(TRUE);
  if (prevactive!= NULL) prevactive->enable(TRUE);
  if (newactive!= NULL) newactive->enable(TRUE);
#endif

  if (bb && newactive) bb->setGroupLabel(newactive->getName());
  active = newactive;
  onSetPage(nextpos);  
}

int TabSheet::onResize() {
  TABSHEET_PARENT::onResize();

  int x=0;

  if (!isInited()) return 1;

  RECT r = clientRect();

  // put buttbar at the top
  if (bb) bb->resize(r.left, r.top-bb->getHeight(), r.right-r.left, bb->getHeight()+1);
  if (tsb) tsb->resize(r.left, r.top-tsb->getHeight(), r.right-r.left, tsb->getHeight()+1);
/*
  // resize content group if it's there
  if (contentwnd) {
    contentwnd->resize(&r);
    // since its holder is not resizing its content, we need to do it ourselves
    foreach(tabs)
      BaseWnd *c = tabs.getfor()->getBaseWnd();
    
      if (c->getParent() != NULL && c->getParent() != this && c->getParent()->getParent() == contentwnd->getContentRootWnd()) {
        RECT r;
        c->getParent()->getClientRect(&r);
        c->resize(&r);
      } else {
        // if we're holding it directly, resize it to our rect
        c->resize(&r);
      }
      
    endfor
  }
*/
  foreach(tabs)
    BaseWnd *c = tabs.getfor()->getBaseWnd();
    c->resize(&r);
  endfor

  invalidate();
  if (leftscroll)
    leftscroll->invalidate();
  if (rightscroll)
    rightscroll->invalidate();

  return 1;
}

BaseWnd *TabSheet::enumChild(int child) {
  TabButtonBase *tb = tabs[child];
  if (tb == NULL) return NULL;
  return tb->getBaseWnd();
}

int TabSheet::getNumChild() {
  return tabs.getNumItems();
}

void TabSheet::setCurPage(int page) {
  BaseWnd *e = enumChild(page);
  if (e != NULL) activateChild(e);
}

TabButtonBase *TabSheet::enumButton(int i) {
  if (i < tabs.getNumItems())
    return tabs[i];
  else
    return NULL;
}

int TabSheet::childNotify(RootWnd *child, int msg, int param1, int param2) {
  if (msg == ChildNotify::NAMECHANGED) {
    const char *id = child->getRootWndName();
    foreach(tabs)
      RootWnd *w = tabs.getfor()->getBaseWnd();
      if (w == child || w == child->getParent()) {
        const char *name = child->getRootWndName();
        tabs.getfor()->btn_setText(name && *name ? name : "[?]");
      }
    endfor;
  }/*
  if (msg == ChildNotify::GROUPRELOAD && child == contentwnd) {
    foreach(tabs)
      RootWnd *holder = this;
    
      if (contentwnd->getContentRootWnd() != NULL) {
        GuiObject *o = contentwnd->getContent()->guiobject_findObject("content");
        if (o != NULL)
          holder = o->guiobject_getRootWnd();
      }
      
      tabs.getfor()->getBaseWnd()->reparent(holder);
    endfor;
  }*/
  return TABSHEET_PARENT::childNotify(child, msg, param1, param2);
}


void TabSheet::setContentMarginLeft(int cm) {
  content_margin_left = cm;
  if (isInited())
    onResize();
}

void TabSheet::setContentMarginTop(int cm) {
  content_margin_top = cm;
  if (isInited())
    onResize();
}

void TabSheet::setContentMarginRight(int cm) {
  content_margin_right = cm;
  if (isInited())
    onResize();
}

void TabSheet::setContentMarginBottom(int cm) {
  content_margin_bottom = cm;
  if (isInited())
    onResize();
}

// TabButton

TabButtonBase::TabButtonBase(BaseWnd *linkwnd, TabSheet *par, const char *tip) :
  linked(linkwnd), parent(par) {
  nodeletelinked = 0;
  ASSERT(linked != NULL);
}

TabButtonBase::~TabButtonBase() {
  if (!nodeletelinked) delete linked;
}

int TabButton::onInit() {
  TABBUTTON_PARENT::onInit();
  m_down=false;
  setButtonText(linked->getNameSafe("[?]"));
  return 1;
}

void TabButton::onLeftPush(int x, int y) {
  ASSERT(parent != NULL);
  ASSERT(linked != NULL);
  parent->activateChild(linked);
}
/*
int TabButton::onLeftButtonDown(int x, int y) {
  m_down = true;
  return 0;
}

int TabButton::onLeftButtonUp(int x, int y) {
  if( m_down ) {
    onLeftPush(x,y);
  }
  m_down = false;
  return 0;
}
*/
void TabButton::btn_setHilite(int tf) {
  setHilite(tf);
}

void TabButton::btn_setText(const char *text) {
  setButtonText(text);
}

// GroupTabButton

void GroupTabButton::grouptoggle_onLeftPush() {
  //GROUPTABBUTTON_PARENT::grouptoggle_onLeftPush();
  ASSERT(parent != NULL);
  ASSERT(linked != NULL);
  parent->activateChild(linked);
}

void GroupTabButton::btn_setHilite(int tf) {
  setStatus(tf ? STATUS_ON : STATUS_OFF);
}

void GroupTabButton::btn_setText(const char *text) { /*
  for (int i=0;i<getNumGroups();i++) {
    GuiObject *grp = enumGroups(i)->getContent();
    if (grp != NULL) {
      GuiObject *o = grp->guiobject_findObject("text");
      if (o != NULL) {
        C_Text txt(o->guiobject_getScriptObject());
        txt.setText(text);
      }
    }
  } */
}

int GroupTabButton::onInit() {
  int rt = GROUPTABBUTTON_PARENT::onInit();
  btn_setText(linked->getNameSafe("[?]"));
  return rt;
}

