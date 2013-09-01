#include "precomp.h"

#include "labelwnd.h"

//cut? #include "xlatstr.h"
#define _(x) (x)

#ifdef WASABI_COMPILE_PAINTSETS
#include "../studio/paintsets.h"
#endif

#include <wnd/canvas.h>
//#include "../bfc/skinclr.h"
#include <wnd/notifmsg.h>

#include <bfc/attrib/attribs.h>

//static SkinColor labelfg("wasabi.labelwnd.foreground");
//static SkinColor labelbg("wasabi.labelwnd.background", "Text backgrounds");
static _color labelfg("wasabi.labelwnd.foreground");
static _color labelbg("wasabi.labelwnd.background", "Text backgrounds");

#define DEF_LABEL_HEIGHT 0
#define DEF_LABEL_FONTSIZE 16

LabelWnd::LabelWnd() {
  show_label = FALSE;
  labelsize = DEF_LABEL_FONTSIZE;
  labelHeight = 0;
  margin=0;
  setVirtual(0);
}

void LabelWnd::getClientRect(RECT *r) {
  LABELWND_PARENT::getClientRect(r);
  r->top += getLabelHeight();
}

int LabelWnd::onResize() {
  LABELWND_PARENT::onResize();
  invalidateLabel();
/*  if (getLabelHeight() <= 0) return 0;
  RECT r,ir;
  LABELWND_PARENT::getClientRect(&r);
  LABELWND_PARENT::getNonClientRect(&ir);
  ir.bottom = ir.top + getLabelHeight()+margin;
  invalidateRect(&ir);*/
  return 1;
}

int LabelWnd::onPaint(Canvas *canvas) {
  if (getLabelHeight() <= 0) return LABELWND_PARENT::onPaint(canvas);

  PaintBltCanvas paintcanvas;
  if (canvas == NULL) {
    if (!paintcanvas.beginPaintNC(this)) return 0;
    canvas = &paintcanvas;
  }

  RECT r;
  LabelWnd::getNonClientRect(&r);

  if (canvas->isFixedCoords()) { // handle possible double buffer
    // convert to canvas coords
    r.right -= r.left; r.left = 0;
    r.bottom -= r.top; r.top = 0;
  }

  r.bottom = r.top + getLabelHeight();

  if (margin) {
    r.left+=margin;
    r.right-=margin;
    r.bottom+=margin*2;
  }

  LABELWND_PARENT::onPaint(canvas);
  int got_focus = gotFocus() || forceFocus();

#ifdef WASABI_COMPILE_SKIN
  if (wantRenderBaseTexture()) {
    WASABI_API_SKIN->skin_renderBaseTexture(getBaseTextureWindow(), canvas, &r, this);
  }
#endif

#ifdef WASABI_COMPILE_PAINTSETS
  WASABI_API_WND->paintset_render(Paintset::LABEL, canvas, &r, got_focus ? 255 : 64);
#endif

  canvas->setTextOpaque(FALSE);
  canvas->pushTextSize(getLabelHeight()-1);
  const char *name = getName();
  if (name == NULL || *name == '\0') name = "Label";
#define LEFTMARGIN 3
  canvas->pushTextColor(labelbg);
  canvas->textOut(r.left+LEFTMARGIN+1, r.top+1, _(name));
  canvas->popTextColor();
  canvas->pushTextColor(labelfg);
  canvas->textOut(r.left+LEFTMARGIN, r.top, _(name));
  canvas->popTextColor();
  canvas->popTextSize();

  return 1;
}

void LabelWnd::onSetName() {
  LABELWND_PARENT::onSetName();
  // make sure label gets repainted
  if (isInited()) {
    RECT r;
    LabelWnd::getNonClientRect(&r);
    r.bottom = r.top + getLabelHeight();
    invalidateRect(&r);
  }
}

//CUTint LabelWnd::childNotify(RootWnd *child, int msg, int param1, int param2) {
//CUT  switch (msg) {
//CUT    case CHILD_WINDOWSHADE_CAPABLE: return show_label;
//CUT    case CHILD_WINDOWSHADE_ENABLE: return TRUE;
//CUT  }
//CUT  return LABELWND_PARENT::childNotify(child, msg, param1, param2);
//CUT}

int LabelWnd::showLabel(int show) {
  show_label = show;
  setFontSize(-1);
  if (isPostOnInit()) {
    onResize();
  }
  return 1;
}

int LabelWnd::getLabelHeight() {
  return show_label ? labelHeight : 0;
}

void LabelWnd::setMargin(int newmargin) {
  margin = newmargin;
  RECT r;
  getNonClientRect(&r);
  r.bottom = getLabelHeight()+margin;
  invalidateRect(&r);
}

int LabelWnd::onGetFocus() {
  LABELWND_PARENT::onGetFocus();
  invalidateLabel();
  return 1;
}

int LabelWnd::onKillFocus() {
  LABELWND_PARENT::onKillFocus();
  invalidateLabel();
  return 1;
}

void LabelWnd::invalidateLabel() {
  if (labelHeight <= 0) return;
  RECT ncr;
  RECT cr;
  RECT lr;
  LabelWnd::getNonClientRect(&ncr);
  LabelWnd::getClientRect(&cr);
  Std::rectSubtract(&lr, &ncr, &cr);
  invalidateRect(&lr);
}

int LabelWnd::wantFocus() {
  return (labelHeight > 0);
}

void LabelWnd::reloadResources() {
  LABELWND_PARENT::reloadResources();
  if (isPostOnInit())
    onResize();
  invalidateLabel();
}

int LabelWnd::setFontSize(int size) {
  LABELWND_PARENT::setFontSize(size);
  SysCanvas sysc;
#ifndef WASABINOMAINAPI
  sysc.pushTextSize(labelsize+api->metrics_getDelta());
#else
  //MULTIAPI-FIXME: not handling delta
  sysc.pushTextSize(labelsize);
#endif
  labelHeight = sysc.getTextHeight() + 1;
  sysc.popTextSize();
  invalidate();
  if (isPostOnInit()) onResize();
  return 1;
}

