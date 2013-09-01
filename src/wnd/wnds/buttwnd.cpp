#include "precomp.h"
#include "buttwnd.h"
#include "wnd/canvas.h"
#include "wnd/region.h"
#include "wnd/notifmsg.h"

//#include "../../studio/api.h"
//#include "../msgbox.h"

#define DEFAULT_BUTTON_HEIGHT 20

ButtonWnd::ButtonWnd(const char *button_text) {
  if (button_text != NULL) setName(button_text);
  currgn = NULL;
  hirgn = NULL;
  normalrgn = NULL;
  pushedrgn = NULL;
  activatedrgn = NULL;
  base_texture = NULL;
  xShift=0;
  yShift=0;
  textsize = DEFAULT_BUTTON_TEXT_SIZE;
  textjustify = BUTTONJUSTIFY_CENTER;
  activated = 0;
  userhilite = 0;
  userdown = 0;
  use_base_texture = 0;
  center_bitmap = 0;
  enabled = 1;
  checked=0;
  autodim=0;
  borders = 1;
  setBorderStyle("button_normal");
  iwantfocus = 1;
  color_text = "studio.button.text";
  color_hilite = "studio.button.hiliteText";
  color_dimmed = "studio.button.dimmedText";         
  checkbmp = "studio.button.checkmark";  
  inactivealpha = 255;
  activealpha = 255;
  hilited=0;
  setRectRgn(1);
  retcode = 0;//MSGBOX_ABORTED;
}

ButtonWnd::~ButtonWnd() {
  delete normalrgn;
  delete pushedrgn;
  delete hirgn;
  delete activatedrgn;
}

int ButtonWnd::onMouseMove(int x, int y) {
  hilited=0;

  Region *old = currgn;

  if (getDown()) { // button is down
    if (pushedrgn)
      currgn = pushedrgn;
    else
      currgn = normalrgn;
  } else { // button is not down
    if (hirgn) 
      currgn = hirgn;
    else
      currgn = normalrgn;
    if (ptInRegion(x, y))
      hilited=1;
    else
      currgn = normalrgn;
    }

  if (old != currgn) invalidateWindowRegion();

  return BUTTONWND_PARENT::onMouseMove(x, y);
}

Region *ButtonWnd::getRegion() {
  if (borders) return NULL;
  return currgn;
}

void ButtonWnd::setModalRetCode(int r) { retcode = r; }
int ButtonWnd::getModalRetCode() const { return retcode; }

void ButtonWnd::onLeaveArea() {
  BUTTONWND_PARENT::onLeaveArea();
  hilited=0;
  if (hirgn) invalidate();
}

/*BOOL ButtonWnd::mouseInRegion(int x, int y) {
  POINT pos={x,y};
  POINT p2=pos;

  RECT r;
  getClientRect(&r);
  pos.x-=r.left;
  pos.y-=r.top;

  return (((!currgn || rectrgn) && PtInRect(&r, p2)) || (currgn && currgn->ptInRegion(&pos)));
}*/

int ButtonWnd::setBitmaps(const char *_normal, const char *_pushed, const char *_hilited, const char *_activated) {

  if (_normal) { delete normalrgn; normalrgn = NULL; }
  if (_pushed) { delete pushedrgn; pushedrgn = NULL; }
  if (_hilited) { delete hirgn; hirgn = NULL; }
  if (_activated) { delete activatedrgn; activatedrgn = NULL; }

  if (_normal) {
    normalbmp = _normal;
    normalrgn = new RegionI(&normalbmp);
    currgn = normalrgn;
  }

  if (_pushed) {
    pushedbmp = _pushed;
    pushedrgn = new RegionI(&pushedbmp);
  }

  if (_hilited) {
    hilitebmp = _hilited;
    hirgn = new RegionI(&hilitebmp);
  }

  if (_activated) {
    activatedbmp = _activated;
    activatedrgn = new RegionI(&activatedbmp);
  }

  return 1;
}

void ButtonWnd::freeResources() {
  BUTTONWND_PARENT::freeResources();
  delete normalrgn;
  delete pushedrgn;
  delete hirgn;
  delete activatedrgn;
  pushedrgn=NULL;
  normalrgn=NULL;
  hirgn=NULL;
  activatedrgn=NULL;
  currgn=NULL;
}

void ButtonWnd::reloadResources() {
  BUTTONWND_PARENT::reloadResources();
  //if (normalbmp.getBitmap())
    normalrgn = new RegionI(&normalbmp, 0, 0);
  //if (pushedbmp.getBitmap())
    pushedrgn = new RegionI(&pushedbmp, 0, 0);
  //if (hilitebmp.getBitmap())
    hirgn = new RegionI(&hilitebmp, 0, 0);
  //if (activatedbmp.getBitmap())
    activatedrgn = new RegionI(&activatedbmp, 0, 0);
  currgn = normalrgn;
}

int ButtonWnd::setBitmapCenter(int centerit) {
  return center_bitmap = !!centerit;
}

int ButtonWnd::setBitmaps(HINSTANCE hInst, int _normal, int _pushed, int _hilited, int _activated, const char *_colorgroup){
  if (_normal) { delete normalrgn; normalrgn = NULL; }
  if (_pushed) { delete pushedrgn; pushedrgn = NULL; }
  if (_hilited) { delete hirgn; hirgn = NULL; }
	if (_activated) { delete activatedrgn; activatedrgn = NULL; }

  if (_colorgroup == NULL)
    _colorgroup = colorgroup;

  if (_normal) {
    //normalbmp.setHInstanceBitmapColorGroup(_colorgroup);
    //normalbmp.setHInstance(hInst);
    normalbmp = Bitmap(hInst, _normal, _colorgroup);
    normalrgn = new RegionI(&normalbmp, 0, 0);
  }

  if (_pushed) {
    //pushedbmp.setHInstanceBitmapColorGroup(_colorgroup);
    //pushedbmp.setHInstance(hInst);
    pushedbmp = Bitmap(hInst, _pushed, _colorgroup);
    pushedrgn = new RegionI(&pushedbmp, 0, 0);
  }

  if (_hilited) {
    //hilitebmp.setHInstanceBitmapColorGroup(_colorgroup);
    //hilitebmp.setHInstance(hInst);
    hilitebmp = Bitmap(hInst, _hilited, _colorgroup);
    hirgn = new RegionI(&hilitebmp, 0, 0);
  }

  if (_activated) {
    //activatedbmp.setHInstanceBitmapColorGroup(_colorgroup);
    //activatedbmp.setHInstance(hInst);
    activatedbmp = Bitmap(hInst, _activated, _colorgroup);
    activatedrgn = new RegionI(&activatedbmp, 0, 0);
  }

  return 1;
}

void ButtonWnd::setUseBaseTexture(int useit) {
  use_base_texture = useit;
  invalidate();
}

void ButtonWnd::setBaseTexture(Bitmap *bmp, int x, int y, int tile) {
  base_texture = bmp;
  use_base_texture = TRUE;
  tile_base_texture=tile;
  xShift=x;
  yShift=y;
  invalidate();
}

int ButtonWnd::setButtonText(const char *text, int size) {
  textsize = size;
  ASSERT(textsize > 0);
  setName(text);
  invalidate();
  return 1;
}

const char * ButtonWnd::getButtonText() {
  return getName();
}

void ButtonWnd::setTextJustification(ButtonJustify jus) {
  textjustify = jus;
  invalidate();
}

int ButtonWnd::getWidth() {
  int addl=0;
  if (checked) {
    addl=checkbmp.getWidth()+3;
  }
  if (!rightbmp.isInvalid())
    addl+=rightbmp.getWidth()+3;
  if (normalbmp.isInvalid()) {
    BltCanvas blt(10,10);
    blt.pushTextSize(textsize);
#if 0
    char *lstr=(char*)MALLOC(STRLEN(getName())+4);
    STRCPY(lstr,getName() ? getName() : "");
    if (STRSTR(lstr,"\t")) STRCAT(lstr,"   ");
#else
    StringPrintf lstr("j%sj", getNameSafe(""));
    if (STRCHR(lstr, '\t')) lstr.cat("   ");
#endif
    int a=max((blt.getTextWidth(lstr)*11)/10,8)+addl;
//    FREE(lstr);
    blt.popTextSize();
    return a;
  }
  return normalbmp.getWidth()+addl;
}

int ButtonWnd::getHeight() {
  int minh=0;
  if (checked>0)
    minh=checkbmp.getHeight();
  if (!rightbmp.isInvalid())
    minh=max(rightbmp.getHeight(),minh);
  if (normalbmp.isInvalid()) {
    BltCanvas blt(10,10);
    blt.pushTextSize(textsize);
    int r = max(max((blt.getTextHeight(getName())*11)/10,minh),4);
    blt.popTextSize();
    return r;
  }
  return max(normalbmp.getHeight(),minh);
}

void ButtonWnd::enableButton(int _enabled) {
  _enabled = !!_enabled;
  if (enabled != _enabled) invalidate();
  enabled = _enabled;
  onEnable(enabled);
}

int ButtonWnd::getEnabled() const {
  return enabled;
}

int ButtonWnd::onPaint(Canvas *canvas) {
  PaintBltCanvas paintcanvas;
  Bitmap *bmp;
  RECT r;
  int labelxoffs=0;
  int offset = (enabled&&(userdown||getDown())) ? 1 : 0;

  if (checked) labelxoffs+=3+checkbmp.getWidth();

  if (canvas == NULL) {
    if (!paintcanvas.beginPaint(this)) return 0;
    canvas = &paintcanvas;
  }
  BUTTONWND_PARENT::onPaint(canvas);

  bmp = &normalbmp;
  if (pushedbmp.getBits() && (getDown() || userdown)) bmp = &pushedbmp;
  else if ((getHilite() || userhilite) && hilitebmp.getBits()) bmp = &hilitebmp;
  else if (activatedbmp.getBits() && activated) bmp = &activatedbmp;

  getClientRect(&r);

  RECT nr = r;
//  RECT fcr = r;
  if (use_base_texture) {
    if (!base_texture)
      renderBaseTexture(canvas, r);
    else {
      RECT cr;
      cr.left = xShift;
      cr.top = yShift;
      cr.right = cr.left + (r.right-r.left);
      cr.bottom = cr.top + (r.bottom-r.top);
      if (tile_base_texture) base_texture->blitTile(canvas, &r,xShift,yShift);
      else base_texture->stretchToRectAlpha(canvas, &cr, &r, getPaintingAlpha());
    }
  } else {
    if (borders) {
      int sysobj = -1;
      if (!enabled)
        sysobj = dsoDisabled;
      else
        sysobj = (userdown || getDown()) ? dsoPushed : dsoNormal;
      if (sysobj != -1) canvas->drawSysObject(&nr, sysobj, getPaintingAlpha());
    }
  }

  if (checked>0) {
    RECT ar;
    int c=(r.top+r.bottom)/2;
    ar.left=r.left;
    ar.top=c-checkbmp.getHeight()/2;
    ar.bottom=ar.top+checkbmp.getHeight();
    ar.right=r.left+checkbmp.getWidth();
    checkbmp.stretchToRectAlpha(canvas,&ar,getPaintingAlpha());
  }
  if (!rightbmp.isInvalid()) {
    RECT ar;
    int c=(r.top+r.bottom)/2;
    ar.top=c-rightbmp.getHeight()/2;
    ar.bottom=ar.top+rightbmp.getHeight();
    ar.right=r.right;
    ar.left=ar.right-rightbmp.getWidth();
    
    int alpha = getPaintingAlpha();
    if (!getEnabled()) alpha /= 2;
    rightbmp.stretchToRectAlpha(canvas, &ar, alpha);
  }
  
  if (!bmp->isInvalid()) {
    RECT br = r;
    if (center_bitmap) {
      int w = (r.right - r.left) - getWidth() - labelxoffs;
      int h = (r.bottom - r.top) - getHeight();
      br.top = r.top + h/2 + offset;
      br.bottom = br.top + getHeight();
      br.left = r.left + w/2 + labelxoffs + offset;
      br.right = br.left + getWidth() - (!rightbmp.isInvalid()?rightbmp.getWidth()+3:0);
    } else {
      br.left += labelxoffs;
      br.right -= (!rightbmp.isInvalid()?rightbmp.getWidth()+3:0);
    }
    int alpha2;
    if (hilitebmp.isInvalid() && enabled && autodim) {
      alpha2=128;
      if (getHilite() || userhilite) alpha2=255;
    } else alpha2 = enabled ? 255 : 64;
    bmp->stretchToRectAlpha(canvas, &br,autodim ? (getPaintingAlpha()+alpha2)>>1 : getPaintingAlpha());
  }

  if (getName() != NULL) {
    canvas->setTextOpaque(FALSE);
    //canvas->setTextVariable(TRUE); //FGTEMP!
    canvas->pushTextSize(textsize);
    if (!enabled)
      canvas->setTextColor(color_dimmed);
    else if (userhilite)
      canvas->setTextColor(color_hilite);
    else
      canvas->setTextColor(color_text);
    int h=(r.bottom-r.top-canvas->getTextHeight(getName()))/2;
    if (h<0) h=0;

    r.left += offset + labelxoffs;
    r.right += offset - (!rightbmp.isInvalid()?rightbmp.getWidth()+3:0);
    r.top += offset+h;
    r.bottom = r.top+canvas->getTextHeight(getName());

    if (textjustify == BUTTONJUSTIFY_CENTER)
      canvas->textOutCentered(&r, getName());
    else if (textjustify == BUTTONJUSTIFY_LEFT)
    {
      if (!STRSTR(getName(),"\t"))
        canvas->textOutEllipsed(r.left, r.top, r.right-r.left, r.bottom-r.top, getName());
      else
      {
        char *lstr=STRDUP(getName());
        char *p=STRSTR(lstr,"\t");
        if (p) *p++=0;
        else p="";
        int tw=canvas->getTextWidth(p);
        canvas->textOutEllipsed(r.left, r.top, r.right-r.left-tw, r.bottom-r.top, lstr);
        canvas->textOutEllipsed(r.right-tw, r.top, tw, r.bottom-r.top, p);
        FREE(lstr);
      }
    }

    canvas->popTextSize();
  }

/*
  if (enabled && gotFocus() && wantFocus()) { // SKIN ME
    nr.left+=3;
    nr.right-=3;
    nr.top+=3;
    nr.bottom-=3;
    DrawFocusRect(canvas->getHDC(), &nr);
  }*/

  return 1;
}

int ButtonWnd::onChar(unsigned int c) {
  if (c == 13) {
  
  } else return BUTTONWND_PARENT::onChar(c);
  return 1;
}

void ButtonWnd::onLeftPush(int x, int y) {
  notifyParent(ChildNotify::BUTTON_LEFTPUSH);
}
void ButtonWnd::onRightPush(int x, int y) {
  notifyParent(ChildNotify::BUTTON_RIGHTPUSH);
}
void ButtonWnd::onLeftDoubleClick(int x, int y) {
  notifyParent(ChildNotify::BUTTON_LEFTDOUBLECLICK);
}
void ButtonWnd::onRightDoubleClick(int x, int y) {
  notifyParent(ChildNotify::BUTTON_RIGHTDOUBLECLICK);
}

void ButtonWnd::setHilite(int h) {
  h = !!h;
  if (userhilite != h) {
    userhilite = h;
    invalidate();
  }
}

int ButtonWnd::getHilite() {
  return userhilite || hilited;
}

int ButtonWnd::getPushed() const {
  return userdown;
}

void ButtonWnd::setPushed(int p) {
  p = !!p;
  if (userdown != p)
  {
    userdown=p;
    invalidate();
  }
}

int ButtonWnd::onResize() {
invalidate();
return 1;
}

void ButtonWnd::setCheckBitmap(char *checkbm) {
  checkbmp = checkbm;
}

int ButtonWnd::setRightBitmap(char *bitmap) {
  rightbmp=bitmap;
  return 1;
}

void ButtonWnd::setActivatedButton(int a) {
  if (activated != a) {
    activated = a;
    invalidate();
    onActivateButton(activated);
  }
}

void ButtonWnd::setActivatedNoCallback(int a) {
  // also force invalidate.
  activated = a;
  invalidate();
}

int ButtonWnd::onActivateButton(int active) {
  return 1;
}

int ButtonWnd::getActivatedButton() const {
  return activated;
}

void ButtonWnd::setBorders(int b) {
  b = !!b;
  if (borders != b) {
    borders = b;
    setRectRgn(borders);
    invalidate();
  }
}

void ButtonWnd::setBorderStyle(const char *style) {
  if (style == NULL) style = "";
  if (borderstyle.iscaseequal(style)) return;

  borderstyle = style;

  using namespace DrawSysObj;
  static struct {
    const char *style;
    int normal, pushed, disabled;
  } chart[] = {
    { "button_normal", BUTTON, BUTTON_PUSHED, BUTTON_DISABLED },
    { "osbutton_normal", OSBUTTON, OSBUTTON_PUSHED, OSBUTTON_DISABLED },
    { "osbutton_close", OSBUTTON_CLOSE, OSBUTTON_CLOSE_PUSHED, OSBUTTON_CLOSE_DISABLED },
    { "osbutton_minimize", OSBUTTON_MINIMIZE, OSBUTTON_MINIMIZE_PUSHED, OSBUTTON_MINIMIZE_DISABLED },
    { "osbutton_maximize", OSBUTTON_MAXIMIZE, OSBUTTON_MAXIMIZE_PUSHED, OSBUTTON_MAXIMIZE_DISABLED },
    { NULL, BUTTON, BUTTON_PUSHED, BUTTON_DISABLED },
  };
  dsoNormal = dsoPushed = dsoDisabled = -1;
  for (int i = 0; ; i++) {
    if (chart[i].style == NULL) return;
    if (STRCASEEQL(chart[i].style, style)) {
      borderstyle = style;
      dsoNormal = chart[i].normal;
      dsoPushed = chart[i].pushed;
      dsoDisabled = chart[i].disabled;
    }
  }

  invalidate();
}

const char *ButtonWnd::getBorderStyle() {
  return borderstyle;
}

void ButtonWnd::setInactiveAlpha(int a) {
  inactivealpha=a;
}

void ButtonWnd::setActiveAlpha(int a) {
  activealpha=a;
}

int ButtonWnd::onGetFocus() {
  BUTTONWND_PARENT::onGetFocus();
//  invalidate();
  return 1;
}

int ButtonWnd::onKillFocus() {
  BUTTONWND_PARENT::onKillFocus();
//  invalidate();
  return 1;
}

void ButtonWnd::setColors(const char *text, const char *hilite, const char *dimmed) {
  color_text=text;
  color_hilite=hilite;
  color_dimmed=dimmed;
}

int ButtonWnd::onEnable(int is) {
  return BUTTONWND_PARENT::onEnable(is);
}

int ButtonWnd::getPreferences(int what) {
  switch (what) {
    case SUGGESTED_W: {
      if (!normalBmpStr.isempty()) return normalbmp.getWidth();
      return getWidth();
    }
    case SUGGESTED_H: {
      if (!normalBmpStr.isempty()) return normalbmp.getHeight();
      return getHeight();
    }
  }
  return BUTTONWND_PARENT::getPreferences(what);
}

int ButtonWnd::onInit() {
  int r = BUTTONWND_PARENT::onInit();
  currgn = normalrgn;
  return r;
}
