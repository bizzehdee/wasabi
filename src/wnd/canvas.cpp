#include "precomp.h"

#include <bfc/std.h>
#include <bfc/std_wnd.h>
#include <bfc/rect.h>
#ifdef WIN32
#include <ddraw.h>
#endif

#include "canvas.h"
#include "bltcanvas.h"
#include "region.h"
#include "basewnd.h"

#include <draw/blending.h>
#include <draw/bitmap.h>

#ifdef WASABI_COMPILE_PAINTSETS
#include "../../../studio/paintsets.h"
#endif

#include <bfc/assert.h>
//CUT? #ifndef _NOSTUDIO
//CUT? #include "../../../studio/api.h"
//CUT? #endif

#ifdef WASABI_COMPILE_FONTS
#include <api/font/api_font.h>
#include <api/font/fontdef.h>
#endif

#include <bfc/util/profiler.h>

#ifdef PERSISTENT_STRETCH_TEMP_CANVAS
THREADSTORAGE BltCanvas *blit_tmpcanvas = NULL;
#endif

#define CBCLASS Canvas
START_DISPATCH;
  CB(GETHDC, getHDC);
  CB(GETROOTWND, getRootWnd);
  CB(GETBITS, getBits);
  VCB(GETOFFSETS, getOffsets);
  CB(ISFIXEDCOORDS, isFixedCoords);
  CB(GETDIM, getDim);
  CB(GETTEXTFONT, getTextFont);
  CB(GETTEXTSIZE, getTextSize);
  CB(GETTEXTBOLD, getTextBold);
  CB(GETTEXTOPAQUE, getTextOpaque);
  CB(GETTEXTUNDERLINE, getTextUnderline);
  CB(GETTEXTITALIC, getTextItalic);
  CB(GETTEXTALIGN, getTextAlign);
  CB(GETTEXTCOLOR, getTextColor);
  CB(GETTEXTBKCOLOR, getTextBkColor);
  CB(GETTEXTAA, getTextAntialias);
  CB(GETCLIPBOX, getClipBox);
  CB(GETTEXTANGLE, getTextAngle);
END_DISPATCH;
#undef CBCLASS

//NONPORTABLE

Canvas::Canvas() {
  textangle = 0;
  hdc = NULL;
  bits = NULL;
  srcwnd = NULL;
  fcoord = FALSE;
  xoffset = yoffset = 0;
#if defined(WIN32)
  align = DT_LEFT;
#elif defined(XWINDOW)  
  raster_x = raster_y = 0;
  align = ALIGN_LEFT;
#endif
  tsize = 12;
  tbold = 0;
  taa = 0;
  topaque = 0;
  tunderline = 0;
  titalic = 0;
  width=height=pitch=0;
  tcolor = RGB(0,0,0);
  bktcolor = RGB(255,255,255);
  tfont = new String; // using dynamic tfont here coz we need to manage em with stack, so stacking fonts won't take sizeof(String) and their destruction will not fuxor everything
  tfont->setValue(WASABI_DEFAULT_FONTNAME);
#ifdef WIN32
  defpen = NULL;
  curpen = NULL;
#endif
}

Canvas::~Canvas() {
  delete tfont; tfont = NULL;
  if (!fontstack.isempty()) {
    DebugString("Font stack not empty in Canvas::~Canvas !");
    while (!fontstack.isempty()) {
      String *s;
      fontstack.pop(&s);
      delete s;
    }
  }
#ifdef WIN32
  if (getHDC() && defpen != NULL) {
    SelectObject(getHDC(), defpen);
    DeleteObject(curpen);
  }
#endif
  if (!boldstack.isempty() || !opstack.isempty() || !alstack.isempty() || !colorstack.isempty() || !sizestack.isempty() || !aastack.isempty())
    DebugString("Font property stack not empty in Canvas::~Canvas !");
  if (!penstack.isempty())
    DebugString("Pen stack not empty in Canvas::~Canvas !");
}

void Canvas::setBaseWnd(BaseWnd *b) {
  srcwnd=b;
}

HDC Canvas::getHDC() {
  return hdc;
}

RootWnd *Canvas::getRootWnd() {
  return srcwnd;
}

void *Canvas::getBits() {
  return bits;
}

BOOL Canvas::getDim(int *w, int *h, int *p) {
  if (w) *w=width;
  if (h) *h=height;
  if (p) *p=pitch;
  return FALSE;
}

void Canvas::getOffsets(int *x, int *y) {
  if (x != NULL) *x = getXOffset();
  if (y != NULL) *y = getYOffset();
}

BOOL Canvas::isFixedCoords() {
  return fcoord;
}

BaseWnd *Canvas::getBaseWnd() {
  return srcwnd;
}

void Canvas::fillRect(const RECT *r, COLORREF color, int alpha) {
#if defined(WIN32)
  if (alpha != -1) {
    int w, h, pitch;
    getDim(&w, &h, &pitch);
    if (w == 0 || h == 0 ||pitch == 0) {
      ASSERT(r != NULL);
      HBRUSH brush;
      if (color == RGB(0,0,0)) {
        FillRect(hdc, r, (HBRUSH)GetStockObject(BLACK_BRUSH));
        return;
      }
      RECT rr = *r;
      offsetRect(&rr);

      brush = CreateSolidBrush(color);
      FillRect(hdc, &rr, brush);
      DeleteObject(brush);
    } else {
      unsigned int blah = (unsigned int)alpha;
      ASSERT(r != NULL);
      color = RGBTOBGR(color);
      color = (color & 0xFFFFFF) | (alpha << 24);
#ifndef NO_PREMUL
      BltCanvas::premultiply(&color, 1);
#endif
      int ox, oy;
      getOffsets(&ox, &oy);
      RECT _r = *r;
      _r.right = MIN<int>(r->right, w);
      _r.bottom = MIN<int>(r->bottom, h);
      _r.left = MAX<int>(r->left, 0);
      _r.top = MAX<int>(r->top, 0);
      int l = _r.bottom - _r.top;
      if (l <= 0) return;
      pitch /= 4;
      if (pitch <= 0) return;
      ARGB32 *p = (ARGB32 *)bits + (ox + _r.left + (oy + _r.top)*pitch);
      int n = _r.right - _r.left;
      if (n <= 0) return;
      while (l--) {
        for (int i = 0; i < n; i++)
          p[i] = Blenders::BLEND_ADJ2(p[i], color);
        p += pitch;
      }
    }
  } else {

    ASSERT(r != NULL);
    HBRUSH brush;
    if (color == RGB(0,0,0)) {
      FillRect(hdc, r, (HBRUSH)GetStockObject(BLACK_BRUSH));
      return;
    }
    RECT rr = *r;
    offsetRect(&rr);

    brush = CreateSolidBrush(color);
    FillRect(hdc, &rr, brush);
    DeleteObject(brush);
  }

#elif defined(XWINDOW)
  XSetForeground( XWindow::getDisplay(), hdc->gc, color );
  Rect wr(r);
  XFillRectangle( XWindow::getDisplay(), hdc->d, hdc->gc, 
                  wr.left, wr.top, wr.width(), wr.height() );
#else
#error port me!
#endif // platform
}

void Canvas::drawRect(const RECT *r, int solid, COLORREF color, int alpha) {
#if defined(WIN32)
#if 1
  int w, h, pitch;
  getDim(&w, &h, &pitch);
  if (alpha == -1 || w == 0 || h == 0 || pitch == 0) {
    HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    HPEN oldpen, pen;
    pen = CreatePen(solid ? PS_SOLID : PS_DOT, 0, color & 0xFFFFFF);
    oldpen = (HPEN)SelectObject(hdc, pen);
    ASSERT(r != NULL);
    RECT rr = *r;
    offsetRect(&rr);
    Rectangle(hdc, rr.left, rr.top, rr.right, rr.bottom);
    SelectObject(hdc, oldpen);
    SelectObject(hdc, oldbrush);
    DeleteObject(pen);
  } else {
    unsigned int blah = (unsigned int)alpha;
    color = RGBTOBGR(color);
    color = (color & 0xFFFFFF) | (blah << 24);
#ifndef NO_PREMUL
    BltCanvas::premultiply(&color, 1);
#endif
    int ox, oy;
    getOffsets(&ox, &oy);
    RECT _r = *r;
    _r.right = MIN<int>(r->right, w);
    _r.bottom = MIN<int>(r->bottom, h);
    int _l = r->bottom - r->top;
    int m = _r.bottom - _r.top;
    int l = _l;
    pitch /= 4;
    int *p = (int *)bits + ox + r->left + (oy + r->top)*pitch;
    int n = r->right - r->left;
    int maxn = _r.right - _r.left;
    while (l-- && m--) {
      int _n = maxn;
      if (l == _l-1 || !l) {
        if (solid) {
          while (_n--) {
            *p = Blenders::BLEND_ADJ2(*p, color);
            p++;
          }
        } else {
          while (_n--) {
            if (_n % 2) *p = Blenders::BLEND_ADJ2(*p, color);
            p++;
          }
        }
        p += n - maxn;
      } else {
        if (solid || l % 2)
          *p = Blenders::BLEND_ADJ2(*p, color);
        p+=n-1;
        if (n == maxn && (solid || l % 2)) 
          *p = Blenders::BLEND_ADJ2(*p, color);
        p++;
      }
      p+=pitch-n;
    }
  }
#else
  HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
  HPEN oldpen, pen;
  pen = CreatePen(solid ? PS_SOLID : PS_DOT, 0, color);
  oldpen = (HPEN)SelectObject(hdc, pen);
  ASSERT(r != NULL);
  RECT rr = *r;
  offsetRect(&rr);
  Rectangle(hdc, rr.left, rr.top, rr.right, rr.bottom);
  SelectObject(hdc, oldpen);
  SelectObject(hdc, oldbrush);
  DeleteObject(pen);
#endif
#elif defined(XWINDOW)
  XGCValues gcv;
  int line_save;

  XGetGCValues( XWindow::getDisplay(), hdc->gc, GCLineStyle, &gcv );
  line_save = gcv.line_style;
  gcv.line_style = (solid?LineSolid:LineOnOffDash);
  gcv.foreground = color;
  XChangeGC( XWindow::getDisplay(), hdc->gc, GCLineStyle | GCForeground, &gcv );

  XDrawRectangle( XWindow::getDisplay(), hdc->d, hdc->gc, 
                  r->left, r->top, 
                  r->right - r->left, r->bottom - r->top );

  gcv.line_style = line_save;
  XChangeGC( XWindow::getDisplay(), hdc->gc, GCLineStyle, &gcv );
#else
#error port me!
#endif // platform

}

void Canvas::drawSunkenRect(const RECT *r, COLORREF hilitecolor, COLORREF shadowcolor) {
//Portable
  pushPen(shadowcolor);
  lineDraw(r->left, r->top, r->right-1, r->top);
  lineDraw(r->left, r->top, r->left, r->bottom-1);
  popPen();
  pushPen(hilitecolor);
  lineDraw(r->left, r->bottom-1, r->right-1, r->bottom-1);
  lineDraw(r->right-1, r->bottom-1, r->right-1, r->top);
  popPen();
}

void Canvas::fillRegion(Region *rgn, COLORREF color) {
#if defined(WIN32)
  if(color = RGB(0,0,0)) {
    FillRgn(hdc, rgn->getOSHandle(), (HBRUSH)GetStockObject(BLACK_BRUSH));
    return;
  }
  
  HBRUSH brush = CreateSolidBrush(color);
  FillRgn(hdc, rgn->getOSHandle(), brush);
  DeleteObject(brush);
#elif defined(XWINDOW)
  for(int i = 0; i < rgn->getNumRects(); i++) {
    RECT r;
    rgn->enumRect(i, &r);
    Rect wr(r);
    XFillRectangle(XWindow::getDisplay(), hdc->d, hdc->gc, wr.left, wr.top, wr.width(), wr.height());
  }
#else
#error port me!
#endif // platform
}

void Canvas::setTextAlign(int al) {
#if defined(WIN32)
  switch(al) {
    case ALIGN_LEFT: align=DT_LEFT; break;
    case ALIGN_CENTER: align=DT_CENTER; break;
    case ALIGN_RIGHT: align=DT_RIGHT; break;
  }
#elif defined(XWINDOW)
  align = al;
#else
#error port me!
#endif // platform
}

int Canvas::getTextAlign() {
  return align;
}

void Canvas::pushTextAlign(int al) {
  alstack.push(align);
  align = al;
}

int Canvas::popTextAlign() {
  if (alstack.isempty()) return 0;
  int old = align;
  alstack.pop(&align);
  return old;
}

void Canvas::setTextOpaque(int opaque) {
  topaque = opaque;
}

int Canvas::getTextOpaque() {
  return topaque;
}

void Canvas::pushTextOpaque(int op) {
  opstack.push(topaque);
  topaque = op;
}

int Canvas::popTextOpaque() {
  if (opstack.isempty()) return 0;
  int old = topaque;
  opstack.pop(&topaque);
  return old;
}

void Canvas::setTextUnderline(int underlined) {
  tunderline = underlined;
}

int Canvas::getTextUnderline() {
  return tunderline;
}

void Canvas::pushTextUnderline(int op) {
  unstack.push(tunderline);
  tunderline = op;
}

int Canvas::popTextUnderline() {
  if (unstack.isempty()) return 0;
  int old = tunderline;
  unstack.pop(&tunderline);
  return old;
}

void Canvas::setTextItalic(int italic) {
  titalic = italic;
}

int Canvas::getTextItalic() {
  return titalic;
}

void Canvas::pushTextItalic(int op) {
  itstack.push(titalic);
  titalic = op;
}

int Canvas::popTextItalic() {
  if (itstack.isempty()) return 0;
  int old = titalic;
  itstack.pop(&titalic);
  return old;
}

void Canvas::setTextBold(int b) {
  tbold = b;
}

int Canvas::getTextBold() {
  return tbold;
}

void Canvas::pushTextBold(int b) {
  boldstack.push(tbold);
  tbold = b;
}

int Canvas::popTextBold() {
  if (boldstack.isempty()) return 0;
  int old = tbold;
  boldstack.pop(&tbold);
  return old;
}

void Canvas::setTextAntialias(int aa) {
  taa = aa;
}

int Canvas::getTextAntialias() {
  return taa;
}

void Canvas::pushTextAntialias(int aa) {
  aastack.push(taa);
  taa = aa;
}

int Canvas::popTextAntialias() {
  if (aastack.isempty()) return 0;
  int old = taa;
  aastack.pop(&taa);
  return old;
}

void Canvas::setTextAngle(int angle) {
  textangle = angle;
}

int Canvas::getTextAngle() {
  return textangle;
}

void Canvas::pushPen(COLORREF color) {
  pushPen(PENSTYLE_SOLID, 1, color);
}

void Canvas::pushPen(int style, int width, COLORREF color) {
  ASSERT(getHDC() != NULL);
  penstyle = style;
  penwidth = width;
  pencolor = color;
  penstruct s;
#ifdef WIN32
  curpen = CreatePen(style, width, color);
  HPEN oldpen = (HPEN)SelectObject(getHDC(), curpen);
#endif
  s.style = style;
  s.width = width;
  s.color = color;
#ifdef WIN32
  s.hpen = oldpen;
#endif
  penstack.push(s);
}

void Canvas::popPen() {
  ASSERT(getHDC() != NULL);
  if (penstack.isempty()) return;
  penstruct s;
  penstack.pop(&s);
#ifdef WIN32
  SelectObject(getHDC(), s.hpen);
  DeleteObject(curpen);
#endif
}

int Canvas::getPenStyle() {
  return penstyle;
}

COLORREF Canvas::getPenColor() {
  return pencolor;
}


int Canvas::getPenWidth() {
  return penwidth;
}

void Canvas::setTextColor(COLORREF color) {
  tcolor = color;
}

COLORREF Canvas::getTextColor() {
  return tcolor;
}

void Canvas::pushTextColor(COLORREF color) {
  colorstack.push(tcolor);
  tcolor = color;
}

COLORREF Canvas::popTextColor() {
  if (colorstack.isempty()) return RGB(0,0,0);
  COLORREF oldcolor = tcolor;
  colorstack.pop(&tcolor);
  return oldcolor;
}

void Canvas::setTextBkColor(COLORREF color) {
  bktcolor = color;
}

COLORREF Canvas::getTextBkColor() {
  return bktcolor;
}

void Canvas::pushTextBkColor(COLORREF color) {
  bkcolorstack.push(bktcolor);
  bktcolor = color;
}

COLORREF Canvas::popTextBkColor() {
  if (bkcolorstack.isempty()) return RGB(0,0,0);
  COLORREF bkoldcolor = bktcolor;
  bkcolorstack.pop(&bktcolor);
  return bkoldcolor;
}

void Canvas::setTextSize(int points) {
  tsize = points;
}

int Canvas::getTextSize() {
  return tsize;
}

void Canvas::pushTextSize(int point) {
  sizestack.push(tsize);
  tsize = point;
}

int Canvas::popTextSize() {
  if (sizestack.isempty()) return 0;
  int oldsize = tsize;
  sizestack.pop(&tsize);
  return oldsize;
}

void Canvas::setTextFont(const char *font_id_name) {
  tfont->setValue(font_id_name);
}

const char *Canvas::getTextFont() {
  return tfont->getValue();
}

void Canvas::pushTextFont(const char *font_id_name) {
  fontstack.push(tfont);
  tfont = new String;
  tfont->setValue(font_id_name);
}

const char *Canvas::popTextFont() {
  if (fontstack.isempty()) return NULL;
  String *old = tfont;
  fontstack.pop(&tfont);
  Canvas::retfontname[0] = 0;
  if (!old->isempty())
    old->strncpyTo(Canvas::retfontname, sizeof(Canvas::retfontname));
  delete old;
  return Canvas::retfontname;
}

void Canvas::moveTo(int x, int y) {
#if defined(WIN32)
  MoveToEx(hdc, x, y, NULL);
#elif defined(XWINDOW)
  raster_x = x;
  raster_y = y;
#else
#error port me!
#endif // platform
}

void Canvas::lineTo(int x, int y) {
#if defined(WIN32)
  LineTo(hdc, x, y);
#elif defined(XWINDOW)
  XDrawLine( XWindow::getDisplay(), hdc->d, hdc->gc, raster_x, raster_y, x, y );
  raster_x = x;
  raster_y = y;
#else
#error port me!
#endif // platform
}

void Canvas::lineDraw(int fromX, int fromY, int toX, int toY) {
#if defined(WIN32)
  MoveToEx(hdc, fromX, fromY, NULL);
  LineTo(hdc, toX, toY);
#elif defined(XWINDOW)
  XDrawLine( XWindow::getDisplay(), hdc->d, hdc->gc, fromX, fromY, toX, toY );
  raster_x = toX;
  raster_y = toY;
#else
#error port me!
#endif // platform
}

void Canvas::drawSysObject(const RECT *r, int sysobj, int alpha) {
#if defined(WIN32)
#ifndef _NOSTUDIO
  RECT i_dont_trust_ms_with_my_rect = *r;
  switch (sysobj) {
#ifdef WASABI_COMPILE_PAINTSETS
    case DrawSysObj::BUTTON:
      WASABI_API_WND->paintset_render(Paintset::BUTTONUP, this, r, alpha);
    break;
    case DrawSysObj::BUTTON_PUSHED:
      WASABI_API_WND->paintset_render(Paintset::BUTTONDOWN, this, r, alpha);
    break;
    case DrawSysObj::BUTTON_DISABLED:
      WASABI_API_WND->paintset_render(Paintset::BUTTONDISABLED, this, r, alpha);
    break;
#endif
#ifdef WIN32
    case DrawSysObj::OSBUTTON: {
      DrawFrameControl(getHDC(), &i_dont_trust_ms_with_my_rect, DFC_BUTTON, DFCS_BUTTONPUSH);
    }
    break;
    case DrawSysObj::OSBUTTON_PUSHED: {
      DrawFrameControl(getHDC(), &i_dont_trust_ms_with_my_rect, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_PUSHED);
    }
    break;
    case DrawSysObj::OSBUTTON_DISABLED: {
      DrawFrameControl(getHDC(), &i_dont_trust_ms_with_my_rect, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_INACTIVE);
    }
    break;

    case DrawSysObj::OSBUTTON_CLOSE: {
      DrawFrameControl(getHDC(), &i_dont_trust_ms_with_my_rect, DFC_CAPTION, DFCS_CAPTIONCLOSE);
    }
    break;
    case DrawSysObj::OSBUTTON_CLOSE_PUSHED: {
      DrawFrameControl(getHDC(), &i_dont_trust_ms_with_my_rect, DFC_CAPTION, DFCS_CAPTIONCLOSE|DFCS_PUSHED);
    }
    break;
    case DrawSysObj::OSBUTTON_CLOSE_DISABLED: {
      DrawFrameControl(getHDC(), &i_dont_trust_ms_with_my_rect, DFC_CAPTION, DFCS_CAPTIONCLOSE|DFCS_INACTIVE);
    }
    break;

    case DrawSysObj::OSBUTTON_MINIMIZE: {
      DrawFrameControl(getHDC(), &i_dont_trust_ms_with_my_rect, DFC_CAPTION, DFCS_CAPTIONMIN);
    }
    break;
    case DrawSysObj::OSBUTTON_MINIMIZE_PUSHED: {
      DrawFrameControl(getHDC(), &i_dont_trust_ms_with_my_rect, DFC_CAPTION, DFCS_CAPTIONMIN|DFCS_PUSHED);
    }
    break;
    case DrawSysObj::OSBUTTON_MINIMIZE_DISABLED: {
      DrawFrameControl(getHDC(), &i_dont_trust_ms_with_my_rect, DFC_CAPTION, DFCS_CAPTIONMIN|DFCS_INACTIVE);
    }
    break;

    case DrawSysObj::OSBUTTON_MAXIMIZE: {
      DrawFrameControl(getHDC(), &i_dont_trust_ms_with_my_rect, DFC_CAPTION, DFCS_CAPTIONMAX);
    }
    break;
    case DrawSysObj::OSBUTTON_MAXIMIZE_PUSHED: {
      DrawFrameControl(getHDC(), &i_dont_trust_ms_with_my_rect, DFC_CAPTION, DFCS_CAPTIONMAX|DFCS_PUSHED);
    }
    break;
    case DrawSysObj::OSBUTTON_MAXIMIZE_DISABLED: {
      DrawFrameControl(getHDC(), &i_dont_trust_ms_with_my_rect, DFC_CAPTION, DFCS_CAPTIONMAX|DFCS_INACTIVE);
    }
    break;
#else
#error port me!
#endif
    break;
  }
#endif
#elif defined(XWINDOW)
  //TODO: once WASABI_API_WND comes back, figure out what to do. Hint: see linux_canvas.cpp from old SDK.
  DebugString("portme -- Canvas::drawSysObject()");
#else
#error port me!
#endif
}

//CUT #ifdef WASABI_COMPILE_FONTS

void Canvas::textOut(int x, int y, const char *txt) {
#ifdef WASABI_COMPILE_FONTS
#ifndef _NOSTUDIO
  WASABI_API_FONT->font_textOut(this, WA_FONT_TEXTOUT_NORMAL, x, y, 0, 0, txt);
#endif
#endif
}

void Canvas::textOut(int x, int y, int w, int h, const char *txt) {
#ifdef WASABI_COMPILE_FONTS
#ifndef _NOSTUDIO
  WASABI_API_FONT->font_textOut(this, WA_FONT_TEXTOUT_RECT, x, y, w, h, txt);
#endif
#endif
}

void Canvas::textOutEllipsed(int x, int y, int w, int h, const char *txt) {
#ifdef WASABI_COMPILE_FONTS
#ifndef _NOSTUDIO
  WASABI_API_FONT->font_textOut(this, WA_FONT_TEXTOUT_ELLIPSED, x, y, w, h, txt);
#endif
#endif
}

void Canvas::textOutWrapped(int x, int y, int w, int h, const char *txt) {
#ifdef WASABI_COMPILE_FONTS
#ifndef _NOSTUDIO
  WASABI_API_FONT->font_textOut(this, WA_FONT_TEXTOUT_WRAPPED, x, y, w, h, txt);
#endif
#endif
}

void Canvas::textOutWrappedPathed(int x, int y, int w, const char *txt) {
#ifdef WASABI_COMPILE_FONTS
#ifndef _NOSTUDIO
  WASABI_API_FONT->font_textOut(this, WA_FONT_TEXTOUT_WRAPPEDPATHED, x, y, w, 0, txt);
#endif
#endif
}

void Canvas::textOutCentered(const RECT *r, const char *txt) {
#ifdef WASABI_COMPILE_FONTS
#ifndef _NOSTUDIO
  WASABI_API_FONT->font_textOut(this, WA_FONT_TEXTOUT_CENTERED, r->left, r->top, r->right-r->left, r->bottom-r->top, txt);
#endif
#endif
}

int Canvas::getTextWidth(const char *text) {
#ifdef WASABI_COMPILE_FONTS
#ifndef _NOSTUDIO
  int ret=-1;
  if (WASABI_API_FONT->font_getInfo(this, tfont->getValue(), WA_FONT_GETINFO_WIDTH, text, &ret, NULL)) return ret;
  return -1; // error
#else
  return 0;
#endif
#endif
  return 0;
}

int Canvas::getTextHeight(const char *text) {
#ifdef WASABI_COMPILE_FONTS
#ifndef _NOSTUDIO
  int ret=-1;
  if (WASABI_API_FONT->font_getInfo(this, tfont->getValue(), WA_FONT_GETINFO_HEIGHT, text, NULL, &ret)) return ret;
  return -1; // error
#else
  return 0;
#endif
#endif
  return 0;
}

void Canvas::getTextExtent(const char *txt, int *w, int *h) {
#ifdef WASABI_COMPILE_FONTS
#ifndef _NOSTUDIO
  WASABI_API_FONT->font_getInfo(this, tfont->getValue(), WA_FONT_GETINFO_WIDTHHEIGHT, txt, w, h);
#endif
#else
  if (w) *w = 0;
  if (h) *h = 0;
#endif
}

//CUT #endif // WASABI_COMPILE_FONTS

void Canvas::offsetRect(RECT *r) {
  ASSERT(r != NULL);
  Rect wr(r);
  wr.left += xoffset;
  wr.right += xoffset;
  wr.top += yoffset;
  wr.bottom += yoffset;
  (*r) = (RECT)wr;
}

#ifdef XWINDOW
#define INTERNAL_HACK
#endif

void Canvas::selectClipRgn(Region *r) {
#if defined(WIN32)
  SelectClipRgn(hdc, r ? r->getOSHandle() : NULL);
#elif defined(XWINDOW)
  HDC dc = getHDC();
  if ( dc->clip ) {
    XDestroyRegion( dc->clip );
    dc->clip = NULL;
  }
  if ( r && !r->isEmpty() ) {
    dc->clip = XCreateRegion();
    XUnionRegion( dc->clip, r->getOSHandle(), dc->clip );
#ifdef INTERNAL_HACK
    int n = r->getNumRects();
    XRectangle *xr = (XRectangle *)MALLOC( n * sizeof( XRectangle ) );
    
    RECT rct;
    for( int i = 0; i < n; i++ ) {
      r->enumRect( i, &rct );
      xr[i].x = rct.left; xr[i].y = rct.top;
      xr[i].width = rct.right - rct.left;
      xr[i].height = rct.bottom - rct.top;
    }

    XSetClipRectangles( XWindow::getDisplay(), dc->gc, 0, 0, xr, n, Unsorted );
    FREE( xr );
#else
    XSetRegion( XWindow::getDisplay(), dc->gc, dc->clip );
#endif
  } else {
    XSetClipMask( XWindow::getDisplay(), dc->gc, None );
  }
#else
#error port me!
#endif // platform

}

int Canvas::getClipBox(RECT *r) {
  if (hdc == NULL) {
    RECT box = Std::makeRectWH(0,0,width,height);
    *r = box;
  }
#if defined(WIN32)
  RECT dummy;
  if (!r) r = &dummy;
  return GetClipBox(hdc, r);
#elif defined(XWINDOW)
  HDC dc = getHDC();
  RECT box = Std::makeRectWH(0,0,width,height);

  if ( ! dc->clip ) {
    *r = box;

    return SIMPLEREGION;
  }
  
  RECT clipr;
#ifdef WASABI_WIN32RECT
  XRectangle xr;
  XClipBox( dc->clip, &xr ); 
  clipr = Std::makeRectWH(xr.x, xr.y, xr.width, xr.height);
#else
  XClipBox( dc->clip, &clipr );
#endif

  Std::rectIntersect( r, &box, &clipr );

  Rect wclipr(clipr);
  if ( XEmptyRegion( dc->clip ) )
    return NULLREGION;
  else if ( XRectInRegion( dc->clip, wclipr.left, wclipr.top, wclipr.width(), wclipr.height() ) == RectangleIn )
    return SIMPLEREGION;
  else
    return COMPLEXREGION;
#else
#error port me!
#endif // platform
}

int Canvas::getClipRgn(Region *r) {
  ASSERT(r != NULL);
#if defined(WIN32)
  return GetClipRgn(hdc, r->getOSHandle());
#elif defined(XWINDOW)
  r->empty();

  HDC dc = getHDC();
  if ( ! dc->clip )
    return 0;
  
  //CUT r->empty();
  XUnionRegion( r->getOSHandle(), dc->clip, r->getOSHandle() );
  return 1;
#else
#error port me!
#endif // platform
}

//FG> added blit canvas to canvas
void __fastcall Canvas::blit(int srcx, int srcy, CanvasBase *dest, int dstx, int dsty, int dstw, int dsth, int alpha, Blender *blender) { 
  if (alpha != -1) {
    int srcimg_w, srcimg_h, srcimg_p;
    getDim(&srcimg_w,&srcimg_h,&srcimg_p);
    WrapperBitmap tmpbmp((ARGB32 *)getBits(), srcimg_w, srcimg_h);
    RECT s, d;
    s.left = srcx;
    s.top = srcy;
    s.right = srcx+dstw;
    s.bottom = srcy+dsth;
    d.left = dstx;
    d.top = dsty;
    d.right = d.left + dstw;
    d.bottom = d.top + dsth;
    tmpbmp.blitToRect(dest, &s, &d, alpha, blender);
    return;
  } 
#if defined(WIN32)
  char *srcbits=(char *)getBits();
  char *destbits=(char *)dest->getBits();
  RECT clipr;
  int reg = dest->getClipBox(&clipr);
  if (srcbits && destbits && (reg == SIMPLEREGION || reg == NULLREGION || reg == ERROR))
  {
    int srcimg_w,srcimg_h,srcimg_p;
    getDim(&srcimg_w,&srcimg_h,&srcimg_p);
    int dstimg_w,dstimg_h,dstimg_p;
    dest->getDim(&dstimg_w,&dstimg_h,&dstimg_p);
    
    if (srcx < 0) { dstx-=srcx; dstw+=srcx; srcx=0; }
    if (srcy < 0) { dsty-=srcy; dsth+=srcy; srcy=0; }
    if (srcx+dstw >= srcimg_w) dstw=srcimg_w-srcx;
    if (srcy+dsth >= srcimg_h) dsth=srcimg_h-srcy;

    if (dstx < clipr.left) { srcx+=clipr.left-dstx; dstw-=clipr.left-dstx; dstx=clipr.left; }
    if (dsty < clipr.top) { srcy+=clipr.top-dsty; dsth-=clipr.top-dsty; dsty=clipr.top; }

    if (dstx+dstw >= clipr.right) dstw=clipr.right-dstx;
    if (dsty+dsth >= clipr.bottom) dsth=clipr.bottom-dsty;

    if (dstw<=0 || dsth<=0) return;

    int y;
    int yl=dsty+dsth;
    for (y = dsty; y < yl; y++)
    {
      MEMCPY32(destbits+y*dstimg_p+dstx*4,srcbits+srcy*srcimg_p+srcx*4,dstw);
      srcy++;
    }
  }
  else
  {
    //GdiFlush();
    BitBlt(dest->getHDC(), dstx, dsty, dstw, dsth, getHDC(), srcx, srcy, SRCCOPY);
  }
#elif defined(XWINDOW)
  if ( dstw <= 0 || dsth <= 0 )
    return;

  HDC dest_hdc = dest->getHDC();

  // Clip to bounds of the src window
  if ( srcx + dstw > width ) dstw = width - srcx;
  if ( srcy + dsth > height ) dsth = height - srcy;

  if ( ( getBits() &&  dest->getBits()) ||
       (!getBits() && !dest->getBits()) ) {
    XCopyArea( XWindow::getDisplay(), hdc->d, dest_hdc->d, dest_hdc->gc, srcx, srcy,
               dstw, dsth, dstx, dsty );
    return;
  }

  // Not both on server or local, check the depth
  int *srcbits = (int *)getBits();

  if ( srcbits ) {
    Window r;
    int x, y;
    unsigned int w, h, b, depth;
    XGetGeometry( XWindow::getDisplay(), dest_hdc->d, &r,
                  &x, &y, &w, &h, &b, &depth );

    switch ( depth ) {
    case 32: // Matches internal representation
    case 24:
      XCopyArea( XWindow::getDisplay(), hdc->d, dest_hdc->d, dest_hdc->gc,
                 srcx, srcy, dstw, dsth, dstx, dsty );
      break;
    case 16: {
      short int *dstbits = (short int *)malloc( dstw * dsth * 2 );
      XImage *img = XCreateImage( XWindow::getDisplay(), XWindow::DefaultVis(),
                                  16, ZPixmap, 0, (char *)dstbits,
                                  dstw, dsth, 16, dstw * 2 );
      
      for( int i = 0; i < dsth; i++ )
        for( int j = 0; j < dstw; j++ ) {
          int val = srcbits[ (srcx + j) + (srcy + i) * width ];
          dstbits[ i * dstw + j ] = (val & 0xf80000) >> 8 | 
            (val & 0xfc00) >> 5 | (val & 0xf8) >> 3;
        }
      
      XPutImage( XWindow::getDisplay(), dest_hdc->d, dest_hdc->gc, img,
                 0, 0, dstx, dsty, dstw, dsth );
      XDestroyImage( img );
    }
      break;
    case 8: {
      static int inited = 0;
      static XColor clrs[256];
      static unsigned char bestcol[32768];
      
      if ( ! inited ) {
        for( int i = 0; i < 256; i++ )
          clrs[i].pixel = i;
        
        XQueryColors( XWindow::getDisplay(), DefaultColormap( XWindow::getDisplay(), XWindow::getScreenNum() ), clrs, 256 );
        
        for( int i = 0; i < 32768; i++ ) {
          int min = -1;
          int mindist = -1;
          
          for ( int j = 0; j < 256; j++ ) {
            int rdist = (clrs[j].red >> 11) - (i >> 10);
            int gdist = (clrs[j].green >> 11) - ((i >> 5) & 0x1f);
            int bdist = (clrs[j].blue >> 11) - (i & 0x1f);
            
            int dist = rdist * rdist + bdist * bdist + gdist * gdist;
            
            if ( min == -1 || dist < mindist ) {
              min = j;
              mindist = dist;
            }
          }
          
          bestcol[i] = min;
        }
        
        inited = 1;
      }
      
      unsigned char *dstbits = (unsigned char *)malloc( dstw * dsth );
      XImage *img = XCreateImage( XWindow::getDisplay(), XWindow::DefaultVis(),
                                  8, ZPixmap, 0, (char *)dstbits,
                                  dstw, dsth, 8, dstw );
      
      for( int i = 0; i < dsth; i++ )
        for( int j = 0; j < dstw; j++ ) {
          int val = srcbits[ (srcx + j) + (srcy + i) * width ];
          val = (val & 0xf80000) >> 9 | (val & 0xf800) >> 6 | (val & 0xf8) >> 3;
          dstbits[ i * dstw + j ] = bestcol[ val ];
        }
      
      XPutImage( XWindow::getDisplay(), dest_hdc->d, dest_hdc->gc, img,
                 0, 0, dstx, dsty, dstw, dsth );
      XDestroyImage( img );
    }
      break;
    default:
      DebugString( "writeme -- non 32,24,16,8bpp blit...\n" );
      break;
    }
  } else {
    int *destbits = (int *)dest->getBits();
    int dstpitch;
    dest->getDim( NULL, NULL, &pitch );
    XImage *img;

    Window r;
    int x, y;
    unsigned int w, h, b, depth;
    XGetGeometry( XWindow::getDisplay(), hdc->d, &r, 
                  &x, &y, &w, &h, &b, &depth );

    switch ( depth ) {
    case 32: // Matches internal representation
    case 24:
      XCopyArea( XWindow::getDisplay(), hdc->d, dest_hdc->d, dest_hdc->gc,
                 srcx, srcy, dstw, dsth, dstx, dsty );
      break;
    case 16:
      img = XGetImage( XWindow::getDisplay(), hdc->d, srcx, srcy, dstw, dsth,
                       AllPlanes, ZPixmap );

      for( int i = 0; i < dsth; i++ )
        for( int j = 0; j < dstw; j++ ) {
          int val = ((short *)img->data)[ j + i * w * 2 ];
          destbits[ dstx + j + (dsty + i) * dstpitch ] =
            (val & 0xf800) << 24 | (val & 0x7e) << 16 | (val & 0x1f);
        }

      XDestroyImage( img );
      break;
    case 8:{
      static int inited = 0;
      static XColor clrs[256];
      static int colors[256];
      
      if ( ! inited ) {
        for( int i = 0; i < 256; i++ )
          clrs[i].pixel = i;
        
        XQueryColors( XWindow::getDisplay(), DefaultColormap( XWindow::getDisplay(), XWindow::getScreenNum() ), clrs, 256 );

        for( int i = 0; i < 256; i++ )
          colors[i] = (clrs[i].red & 0xff00) << 8 | (clrs[i].green & 0xff00) |
            (clrs[i].blue >> 8);

        inited = 1;
      }


      img = XGetImage( XWindow::getDisplay(), hdc->d, srcx, srcy, dstw, dsth,
                       AllPlanes, ZPixmap );

      for( int i = 0; i < dsth; i++ )
        for( int j = 0; j < dstw; j++ ) {
          destbits[ dstx + j + (dsty + i) * dstpitch ] = 
            colors[ img->data[ j + i * w ]];
        }

      XDestroyImage( img );
    }
      break;
    default:
      DebugString( "writeme -- non 32,24bpp back blit\n" );
      break;
    }
  }
#else
#error port me!
#endif // platform
}

// src* are in fixed point
void __fastcall Canvas::stretchblit(int srcx, int srcy, int srcw, int srch, CanvasBase *dest, int dstx, int dsty, int dstw, int dsth, int alpha, Blender *blender) { 
  //GdiFlush();
  int done=0;
  void *srcdib=getBits();

  if (!dstw || !dsth || !srcw || !srch) return;
  if (srcdib)
  {
    int srcdib_w, srcdib_h, srcdib_p;
    getDim(&srcdib_w,&srcdib_h,&srcdib_p);
    int nofilter=0;//FUCKO :) _intVal(WASABI_API_CONFIG->config_enumRootCfgItem(0), "NoWindowStretchFilter");

    int cw, ch, cp;
    BltCanvas *tmpcanvas = makeTempCanvas(dstw, dsth);
    void *dstdib = tmpcanvas->getBits();
    tmpcanvas->getDim(&cw, &ch, &cp);

    {
      // scaling up
      if ((dstw<<16) >= srcw && (dsth<<16) >= srch)
      {
        int y;
        int SY, dX, dY;
        int Xend=(srcdib_w-2)<<16;
        SY=srcy;
        dX=srcw/dstw;
        dY=srch/dsth;

        int xstart=0;
        int xp=srcx>>16;
        if (xp < 0) 
        {
          xstart=-xp;
          srcx+=xstart*dX;
        }

        int xend=dstw;
        xp=(srcx+(dX*(xend-xstart)))>>16;
        if (xp > srcdib_w)
        {
          xend=xstart+srcdib_w - (srcx>>16);
        }
    
        for (y = 0; y < dsth; y ++)
        {
          int yp=(SY>>16);
          if (yp >= 0)
          {
            int x;
            int SX=srcx;
            unsigned int *out=(unsigned int*)dstdib + xstart + y*cw;
            int end=yp >= srcdib_h-1;
            if (nofilter || end) 
            {
              if (end) yp=srcdib_h-1;
              unsigned int *in=(unsigned int*) ((char *)srcdib + yp*srcdib_p);
              for (x = xstart; x < xend; x ++) // quick hack to draw last line
              {
                *out++=in[SX>>16];
                SX+=dX;
              }
              if (end) break;
            }
            else
            {
              unsigned int *in=(unsigned int*) ((char *)srcdib + yp*srcdib_p);

              #undef BLEND_JOB
              #define BLEND_JOB(version) \
              for (x = xstart; x < xend; x ++) \
              { \
                if (SX>Xend) *out++=Blenders::BLEND4##version(in+(Xend>>16),srcdib_w,0xffff,SY); \
                else *out++=Blenders::BLEND4##version(in+(SX>>16),srcdib_w,SX,SY); \
                SX+=dX; \
              }
              MAKE_BLEND_JOB;
            }
          }
          SY+=dY;
        }
        // end of scaling up
      }
      else // we are scaling down -- THIS IS SLOW AND MAY BREAK THINGS. :)
      {
        int y;
        int SY, dX, dY;
        SY=srcy;
        dX=srcw/dstw;
        dY=srch/dsth;

        int xstart=0;
        int xp=srcx>>16;
        if (xp < 0) 
        {
          xstart=-xp;
          srcx+=xstart*dX;
        }

        int xend=dstw;
        xp=(srcx+(dX*(xend-xstart)))>>16;
        if (xp > srcdib_w)
        {
          xend=xstart+srcdib_w - (srcx>>16);
        }
    
        for (y = 0; y < dsth; y ++)
        {
          // start and end of y source block
          int vStart = SY;
          int vEnd = SY + dY;

          int x;
          int SX=srcx;
          unsigned char *out=(unsigned char *)((unsigned int*)dstdib + xstart + y*cw);
                    
          for (x = xstart; x < xend; x ++)
          {
            int uStart = SX;
            int uEnd = SX + dX;
            // calculate sum of rectangle.
  
            int cnt=0;
            __int64 accum[4]={0,};
            int v,u;
            for (v=vStart; v < vEnd; v+=65536)
            {
              unsigned int vscale=65535;

              if (v == vStart) 
              {
                vscale=65535-(v&0xffff);
              }
              else if (vEnd-v < 65536)
              {
                vscale=(vEnd-v)&0xffff;
              }
              
              int vp = v >> 16;
              unsigned char *in=(unsigned char*) ((char *)srcdib + vp*srcdib_p + 4*(uStart>>16));
              for (u = uStart; u < uEnd; u += 65536)
              {
                unsigned int uscale=vscale;
                if (u == uStart) 
                {
                  uscale*=65535-(u&0xffff);
                  uscale >>= 16;
                }
                else if (uEnd-u < 65536)
                {
                  uscale*=(uEnd-u)&0xffff;
                  uscale >>= 16;
                }
                cnt+=uscale;
                if (uscale==65535)
                {
                  accum[0] += (in[0]<<16)-in[0];
                  accum[1] += (in[1]<<16)-in[1];
                  accum[2] += (in[2]<<16)-in[2];
                  accum[3] += (in[3]<<16)-in[3];
                }
                else
                {
                  accum[0] += in[0]*uscale;
                  accum[1] += in[1]*uscale;
                  accum[2] += in[2]*uscale;
                  accum[3] += in[3]*uscale;
                }
                in+=4;
              }
            }
            if (!cnt) cnt++;

            out[0]=(unsigned char)(accum[0]/cnt);
            out[1]=(unsigned char)(accum[1]/cnt);
            out[2]=(unsigned char)(accum[2]/cnt);
            out[3]=(unsigned char)(accum[3]/cnt);
            out+=4;

            SX+=dX;
          }
          SY+=dY;
        }
        // end of scaling down
      }

#ifndef NO_MMX
      Blenders::BLEND_MMX_END();
#endif
      tmpcanvas->blit(0, 0, dest, dstx, dsty, dstw, dsth, alpha, blender); 
      releaseTempCanvas(tmpcanvas);
    }
  }
}

void __fastcall Canvas::shiftblit(int srcx, int srcy, int srcw, int srch, CanvasBase *dstcanvas, float dstx, float dsty, int alpha, Blender *blender) {
  if (alpha == 0) return;

  if (dstx == (int)dstx && dsty == (int)dsty) {
    blit(srcx, srcy, dstcanvas, (int)dstx, (int)dsty, srcw, srch, alpha, blender);
    return;
  }

  // if dest is < 0, don't copy the chunk of pixels up tp 0
  if (dstx < 0) { srcx += -dstx; srcw -= -dstx; dstx = 0; }
  if (dsty < 0) { srcy += -dsty; srch -= -dsty; dsty = 0; }
  // if source is < 0, don't copy the chunk of pixels up tp 0
  if (srcx < 0) { dstx += -srcx; srcw -= -srcx; srcx = 0; }
  if (srcy < 0) { dsty += -srcy; srch -= -srcy; srcy = 0; }

  // if there isnt anything left to copy, abort
  if (srcw <= 0 || srch <= 0) return;

  int dch, dcw, dcp;
  dstcanvas->getDim(&dcw, &dch, &dcp);
  dcp >>= 2;

  // if destination coordinate above dest canvas size, abort
  if (dstx >= dcw || dsty >= dch) return; 

  int sch, scw, scp;
  getDim(&scw, &sch, &scp);
  scp >>= 2;

  // if source coordinate above source canvas size, abort
  if (srcx >= scw || srcy >= sch) return;

  // compute maximum number of pixels we can copy by taking the minimum value between requested width, remaining dest pixels, and remaining source pixels
  int maxh = MIN(srch, dch-(int)dsty);
  int maxw = MIN(srcw, dcw-(int)dstx);
  maxh = MIN(maxh, sch-srcy);
  maxw = MIN(maxw, scw-srcx);
  
  // get pointers
  ARGB32 *srcbits = (ARGB32 *)getBits();
  ARGB32 *dstbits = (ARGB32 *)dstcanvas->getBits();

  // if either canvas has no bits, abort, could use a temporary canvas instead but for now we'll only support dibs
  if (!srcbits || !dstbits) return;

  // make some temporary storage space for the blend buffers
  BltCanvas *tmpcanvas = makeTempCanvas(maxw+1, maxh+1);
  ARGB32 *tmpbits = (ARGB32 *)tmpcanvas->getBits();
  int tw, th, tp;
  tmpcanvas->getDim(&tw, &th, &tp);
  tp >>= 2;

  // compute weights
  float frac_x = dstx-(int)dstx;
  float frac_y = dsty-(int)dsty;
  float mfrac_x = 1.0f-frac_x;
  float mfrac_y = 1.0f-frac_y;
  int p1 = fastfrnd((mfrac_x * mfrac_y) * 255.0f);
  int p2 = fastfrnd((frac_x * mfrac_y) * 255.0f);
  int p3 = fastfrnd((mfrac_x * frac_y) * 255.0f);
  int p4 = fastfrnd((frac_x * frac_y) * 255.0f);

  // main loop
  for (int y=0;y<maxh;y++) {
    int n = maxw;
    ARGB32 *dst1 = tmpbits + y*tp;
    ARGB32 *dst2 = dst1 + tp;
    ARGB32 *dst1p1 = dst1+1;
    ARGB32 *dst2p1 = dst2+1;
    ARGB32 *in = srcbits + (srcy+y)*scp+srcx;
    while (n--) {
      ARGB32 pix = *in;
      #ifdef NO_PREMUL
      int a = pix>>24;
      int v;
      v = Blenders::alphatable[pix&0xFF][a];
      v |= Blenders::alphatable[(pix>>8)&0xFF][a]<<8;
      v |= Blenders::alphatable[(pix>>16)&0xFF][a]<<16;
      pix = (a<<24)|v;
      #endif
      *dst1 = Blenders::BLEND_PREMULTIPLIED_ADD(*dst1, pix, p1);
      *dst1p1 = Blenders::BLEND_PREMULTIPLIED_ADD(*dst1p1, pix, p2);
      *dst2 = Blenders::BLEND_PREMULTIPLIED_ADD(*dst2, pix, p3);
      *dst2p1 = Blenders::BLEND_PREMULTIPLIED_ADD(*dst2p1, pix, p4);
      in++; dst1++; dst2++; dst1p1++; dst2p1++;
    }
  }
  #ifdef NO_PREMUL
  RECT r={0, 0, maxw+1, maxh+1};
  tmpcanvas->demultiplyRect(r);
  #endif
  tmpcanvas->blit(0, 0, dstcanvas, (int)dstx, (int)dsty, (int)maxw+1, (int)maxh+1, alpha, blender);
  releaseTempCanvas(tmpcanvas);
}

const float ONEOVER255 = 1/255.0f;
const float ONEOVER65025 = 1/65025.0f;

#define fastfMIN(a, b) (fastfabove((a), (b)) ? (b) : (a));

void __inline SUBPIXEL_STRETCHADD(unsigned long *bits, int w, int h, float x, float y, unsigned long color, float xpixelsize, float ypixelsize) {
  int a = color>>24;
  if (a == 0) return;

  int v;
  v = Blenders::alphatable[color&0xFF][a];
  v |= Blenders::alphatable[(color>>8)&0xFF][a]<<8;
  v |= Blenders::alphatable[(color>>16)&0xFF][a]<<16;
  color = v|(a<<24);
  
  signed int iy = (int)y;
  signed int ix = (int)x;
  
  float yshift = y-iy;
  float sxshift = x-ix;
  float oneminusyshift = 1.0f-yshift;
  float oneminusxshift = 1.0f-sxshift;
  float yweight = fastfMIN(ypixelsize, oneminusyshift);
  float thisyweight = yweight;

  unsigned long *lineptr = bits + (iy*w) + ix;
  unsigned long *intdest = lineptr;

  while (fastfabove(ypixelsize, 0.0f)) {
    float _xpixelsize = xpixelsize;

    float xshift = sxshift;
    float xweight = fastfMIN(xpixelsize, oneminusxshift);
    float thisxweight = xweight;
    lineptr = intdest;

    #undef BLEND_JOB
    #define BLEND_JOB(version) \
    while (fastfabove(_xpixelsize,0.0f)) { \
      *intdest = Blenders::BLEND_PREMULTIPLIED_ADD##version(*intdest, color, fastfrnd(thisxweight*thisyweight*255.0f)); \
      _xpixelsize -= thisxweight; \
      thisxweight = fastfMIN(_xpixelsize, 1.0f); \
      intdest++; \
    }
    MAKE_BLEND_JOB;
    ypixelsize -= thisyweight;
    thisyweight = fastfMIN(ypixelsize, 1.0f);
    intdest = lineptr;
    intdest += w;
  }
}

void __fastcall Canvas::stretchblit_hires(float srcx, float srcy, float srcw, float srch, CanvasBase *dstcanvas, float dstx, float dsty, float dstw, float dsth, int alpha, Blender *blender) {
  if (srcw < 1 || srch < 1) return;
  if (dstw <= 0 || dstw <= 0) return;

  // floating point srcw/h isn't supported at the moment. if you are adding support for this, be sure to remove 
  // later assumptions that these values are integers
  srcw = (int)srcw;
  srch = (int)srch;

  // this test assumes srcw/srch are integers, and thus checks if dtsw and dsth have no fractional parts at the same time as it checks if they are the same as srcw, srch
  if ((int)srcx == srcx && (int)srcy == srcy && (int)dstx == dstx && (int)dsty == dsty && srcw == dstw && srch == dsth) {
    blit((int)srcx, (int)srcy, dstcanvas, (int)dstx, (int)dsty, (int)dstw, (int)dsth, alpha, blender);
    return;
  }

  // this test assumes srcw/srch are integers, and thus checks if dtsw and dsth have no fractional parts at the same time as it checks if they are the same as srcw, srch
  if (dstw == srcw && dsth == srch && (int)srcx == srcx && (int)srcy == srcy) {
    shiftblit((int)srcx, (int)srcy, (int)srcw, (int)srch, dstcanvas, dstx, dsty, alpha, blender);
    return;
  }

  //if (alpha == -1) alpha = 255;
  if (alpha != -1) alpha = MAX(0, MIN(alpha, 255));

  int src_canvas_pitch, dst_canvas_pitch;
  int src_canvas_w, src_canvas_h, dst_canvas_w, dst_canvas_h;

  getDim(&src_canvas_w, &src_canvas_h, &src_canvas_pitch);
  if ((int)srcx > src_canvas_w || (int)srcy > src_canvas_h) return;

  BltCanvas *tmpcanvas = makeTempCanvas((int)dstw+2, (int)dsth+2);
  tmpcanvas->getDim(&dst_canvas_w, &dst_canvas_h, &dst_canvas_pitch);

  int maxw = dst_canvas_w - (int)dstx;
  int maxh = dst_canvas_h - (int)dsty;

  src_canvas_pitch >>= 2;
  dst_canvas_pitch >>= 2;

  ARGB32 *src_canvas_bits = (ARGB32 *)getBits();
  ARGB32 *dst_canvas_bits = (ARGB32 *)tmpcanvas->getBits();

  float dw, dh;

  float frac_srcx = srcx-(int)srcx;
  float frac_srcy = srcy-(int)srcy;

  dw = dstw / srcw;
  dh = dsth / srch;

  int _sx,sx,sy = 0;

  float _dstx = dstx-(int)dstx;
  float _dsty = dsty-(int)dsty;

  ARGB32 *in = src_canvas_bits + (int)srcy*src_canvas_pitch+(int)srcx;
  int nextline = src_canvas_pitch-srcw;

  // these two blitter loops assume srcw/h are integers!

  if (frac_srcx == 0 && frac_srcy == 0) {
    for (sy=0;sy<srch;sy++) {
      float ydest = _dsty+sy*dh;
      for (sx=0;sx<srcw;sx++) {
        ARGB32 pix = *in++;
        if (!(pix>>24)) continue;
        _sx = sx;
        sx++;
        float xpixelsize = dw;
        while (sx<srcw && *in == pix) { xpixelsize += dw; in++; sx++; }
        SUBPIXEL_STRETCHADD(dst_canvas_bits, dst_canvas_w, dst_canvas_h, _dstx + _sx*dw, ydest, pix, xpixelsize, dh);
        sx--;
      }
      in += nextline;
    }
  } else { 
    float mfrac_srcx = 1.0f-frac_srcx;
    float mfrac_srcy = 1.0f-frac_srcy;
    int p1 = fastfrnd((mfrac_srcx * mfrac_srcy) * 255.0f);
    int p2 = fastfrnd((frac_srcx * mfrac_srcy) * 255.0f);
    int p3 = fastfrnd((mfrac_srcx * frac_srcy) * 255.0f);
    int p4 = fastfrnd((frac_srcx * frac_srcy) * 255.0f);
    for (sy=0;sy<srch;sy++) {
      float ydest = _dsty+sy*dh;
      #undef BLEND_JOB
      #define BLEND_JOB(version) \
      for (sx=0;sx<srcw;sx++) { \
        ARGB32 pix; \
        if (sx==srcw-1) { \
          if (sy==srch-1) { \
            /* very last add blend with 1 pixel at (x,y) */ \
            pix = Blenders::BLEND_MUL_SCALAR##version(*in, p1); \
          } else { \
            /* last column add blend with 2 pixels at (x,y),(x,y+1) */ \
            pix = Blenders::BLEND_MUL_SCALAR##version(*in, p1); \
            pix = Blenders::BLEND_ADD##version(pix, *(in+src_canvas_pitch), p3); \
          } \
        } else if (sy==srch-1) { \
          /* last line add blend with 2 pixels at (x,y),(x+1,y) */ \
          pix = Blenders::BLEND_MUL_SCALAR##version(*in, p1); \
          pix = Blenders::BLEND_ADD##version(pix, *(in+1), p2); \
        } else { \
          /* normal add blend between 4 pixels at (x,y),(x+1,y),(x,y+1),(x+1,y+1) */ \
          pix = Blenders::BLEND_MUL_SCALAR##version(*in, p1); \
          pix = Blenders::BLEND_ADD##version(pix, *(in+1), p2); \
          pix = Blenders::BLEND_ADD##version(pix, *(in+src_canvas_pitch), p3); \
          pix = Blenders::BLEND_ADD##version(pix, *(in+src_canvas_pitch+1), p4); \
        } \
        in++; \
        if (!(pix>>24)) continue; \
        SUBPIXEL_STRETCHADD(dst_canvas_bits, dst_canvas_w, dst_canvas_h, _dstx + sx*dw, ydest, pix, dw, dh); \
      }
      MAKE_BLEND_JOB;
      in += nextline;
    }
  } 
  #ifdef NO_PREMUL
  RECT r={0, 0, dstw+2, dsth+2};
  tmpcanvas->demultiplyRect(r);
  #endif
  tmpcanvas->blit(0, 0, dstcanvas, (int)dstx, (int)dsty, (int)dstw+2, (int)dsth+2, alpha, blender); 
  releaseTempCanvas(tmpcanvas);
}

BltCanvas *Canvas::makeTempCanvas(int w, int h, BltCanvas **staticcanvas/* =&blit_tmpcanvas */, int extendedby/* =50 */) {
#ifdef PERSISTENT_STRETCH_TEMP_CANVAS
  int cw, ch, cp;
  if (staticcanvas == NULL) staticcanvas = &blit_tmpcanvas;
  if (*staticcanvas) {
    (*staticcanvas)->getDim(&cw, &ch, &cp);
    cp >>= 2;
    if (cw < w || ch < h) {
      delete *staticcanvas;
      *staticcanvas = new BltCanvas(w+extendedby, h+extendedby);
    } else {
      if (cw == w && ch == h) { (*staticcanvas)->fillBits(0); }
      else {
        RECT r={0, 0, w, h};
        (*staticcanvas)->fillBits(0, &r);
      }
    }
  } else {
    *staticcanvas = new BltCanvas(w+extendedby, h+extendedby);
  }
  return *staticcanvas;
#else
  return new BltCanvas(w, h);
#endif
}

void Canvas::releaseTempCanvas(BltCanvas *canvas) {
#ifdef PERSISTENT_STRETCH_TEMP_CANVAS
  // do nothing
#else
  delete canvas;
#endif
}

#define DEBUG_SCREEN_SHIFT 0
void Canvas::debug() {
  Canvas *c = new SysCanvas();
  int w, h;
  getDim(&w, &h, NULL);
  blit(0, 0, c, DEBUG_SCREEN_SHIFT, 0, w, h);
  delete c;
}


#define BF2 (~((3<<24)|(3<<16)|(3<<8)|3))

void Canvas::antiAliasTo(Canvas *dest, int w, int h, int aafactor) {
  ASSERT(aafactor != 0);
  if (aafactor == 1) {
    blit(0, 0, dest, 0, 0, w, h);
    return;
  }
  ASSERT(getBits() != NULL);
  ASSERT(dest->getBits() != NULL);
  if (getBits() == NULL || dest->getBits() == NULL) return;
  ASSERTPR(aafactor <= 2, "too lazy to generalize the code right now :)");
  //GdiFlush();
  // we should really store the bpp too
  int aaw = w * aafactor;
  unsigned long *s1 = (unsigned long *)getBits(), *s2 = s1 + 1;
  unsigned long *s3 = s1 + aaw, *s4 = s3 + 1;
  unsigned long *d = (unsigned long *)dest->getBits();
#if 1
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      unsigned long tmp = ((*s1 & BF2)>>2) + ((*s2 & BF2)>>2) + ((*s3 & BF2)>>2) + ((*s4 & BF2)>>2); 
      *d++ = tmp;

      s1 += 2; s2 += 2;
      s3 += 2; s4 += 2;
    }
    s1 += aaw; s2 += aaw;
    s3 += aaw; s4 += aaw;
  }
#else
for (int x = 0; x < w * h; x++) d[x] = s1[x];
#endif
}

void Canvas::colorToColor(COLORREF from, COLORREF to, RECT *r) {
  int w, h, ox, oy;
  
  // convert to bitmap order
  from = RGBTOBGR(from);
  to = RGBTOBGR(to);

  COLORREF *p;
  getDim(&w, &h, NULL);
  p = (COLORREF *)getBits();
  getOffsets(&ox, &oy);
  p += ox + r->left + (oy + r->top) * w;
  int rw = r->right-r->left;
  for (int j=r->top;j<r->bottom;j++) {
    for (int i=r->left;i<r->right;i++) {
      if (*p == from) 
        *p = to;
      p++;
    }
    p += w - rw;
  }
}

double Canvas::getSystemFontScale() {
  #ifndef _NOSTUDIO
  #ifdef WASABI_COMPILE_CONFIG
  int v = WASABI_API_CONFIG->getIntPublic("manualsysmetrics", -1);
  if (v != -1) return (v / 100.0);
  #endif
  #endif
#if defined(WIN32)
  SysCanvas c;
  int nLogDPIX = GetDeviceCaps(c.getHDC(), LOGPIXELSX);
  return ((double)nLogDPIX/96.0);
#elif defined(XWINDOW)
  double pxpermm = (double)DisplayWidth(XWindow::getDisplay(), XWindow::getScreenNum())
                   / (double)DisplayWidthMM(XWindow::getDisplay(), XWindow::getScreenNum());
  return pxpermm / (25.4/*mm/in*/);
#else
#error port me!
#endif // platform
}

void Canvas::premultiply(ARGB32 *m_pBits, int nwords, int newalpha) {
  if (newalpha==-1) {
    for (; nwords > 0; nwords--, m_pBits++) {
      *m_pBits = Blenders::PREMULTIPLY(*m_pBits);
    }
  } else {
    for (; nwords > 0; nwords--, m_pBits++) {
      unsigned char *pixel = (unsigned char *)m_pBits;
      pixel[0] = Blenders::alphatable[newalpha][pixel[0]];//(pixel[0] * newalpha) >> 8;        // blue
      pixel[1] = Blenders::alphatable[newalpha][pixel[1]];//(pixel[1] * newalpha) >> 8;        // green
      pixel[2] = Blenders::alphatable[newalpha][pixel[2]];//(pixel[2] * newalpha) >> 8;        // red
      pixel[3] = Blenders::alphatable[newalpha][pixel[3]];//(pixel[3] * newalpha) >> 8;        // alpha
    }
  }
}

void Canvas::demultiply(ARGB32 *bits, int nwords) {
  while (nwords--) {
    *bits = Blenders::DEMULTIPLY(*bits);
    bits++;
  }
}

void Canvas::demultiplyRect(RECT r) {
  ARGB32 *bits = (ARGB32 *)getBits();
  int w, h, p;
  getDim(&w, &h, &p); p>>=2;
  RECT cr = {0, 0, w, h};
  RECT ir;
  Std::rectIntersect(&cr, &r, &ir);
  int linesize = ir.right-ir.left;
  for (int y=ir.top; y < ir.bottom; y++) {
    demultiply(bits + y*p, linesize);
  }
}

char Canvas::retfontname[256];

WndCanvas::WndCanvas() {
#if defined(WIN32)
  hWnd = NULL;
#elif defined(XWINDOW)
  hWnd = None;
#else
#error port me!
#endif // platform
}

WndCanvas::~WndCanvas() {
#if defined(WIN32)  
  ASSERT((hWnd != NULL && hdc != NULL) || (hWnd == NULL && hdc == NULL));
  if (hWnd != NULL) ReleaseDC(hWnd, hdc);
#elif defined(XWINDOW)
  if (hWnd != None) {
    XFreeGC( XWindow::getDisplay(), hdc->gc );
    if ( hdc->clip )
      XDestroyRegion( hdc->clip );
    FREE( hdc );
  }
#else
#error port me!
#endif // platform
  hdc = NULL;
}

int WndCanvas::attachToClient(BaseWnd *basewnd) {
  ASSERT(basewnd != NULL);
  hWnd = basewnd->gethWnd();
#if defined(WIN32)
  ASSERT(hWnd != NULL);
  hdc = GetDC(hWnd);
  ASSERT(hdc != NULL);
  srcwnd = basewnd;
  return 1;
#elif defined(XWINDOW)
  ASSERT(hWnd != None);
  hdc = (HDC)MALLOC( sizeof( hdc_typ ) );
  hdc->d = hWnd;
  hdc->gc = XCreateGC( XWindow::getDisplay(), hdc->d, 0, NULL );

  RECT r;
  StdWnd::getWindowRect( hdc->d, &r );
  width = r.right - r.left;
  height = r.bottom - r.top;
  pitch = width * 4;

  ASSERT(hdc->gc != NULL);
  srcwnd = basewnd;
  return 1;
#else
#error port me!
#endif
}

#if 0//CUT
int WndCanvas::attachToWnd(HWND _hWnd) {
  hWnd = _hWnd;
  ASSERT(hWnd != NULL);
  hdc = GetWindowDC(hWnd);
  ASSERT(hdc != NULL);
  return 1;
}
#endif

PaintCanvas::PaintCanvas() {
#if defined(XWINDOW)
  hWnd = None;
#else
  hWnd = NULL;
#endif
}

PaintCanvas::~PaintCanvas() {
#if defined(WIN32)  
  if (hdc != NULL) EndPaint(hWnd, &ps);
#elif defined(XWINDOW)
  if (hdc != NULL) {
    XFreeGC( XWindow::getDisplay(), hdc->gc );
    if ( hdc->clip )
      XDestroyRegion( hdc->clip );
    FREE( hdc );
  }
#else
#error port me!
#endif // platform
  hdc = NULL;
}

void PaintCanvas::getRcPaint(RECT *r) {
#if defined(WIN32)  
  *r = ps.rcPaint;
#elif defined(XWINDOW)
  getClipBox(r);
/*
  Rect wr;
  wr.setRectLTRB(0,0,width,height);
  (*r) = (RECT)wr;
*/
#else
#error port me!
#endif 
}

int PaintCanvas::beginPaint(BaseWnd *basewnd) {
  int ret = beginPaint( basewnd->gethWnd() );
  if(ret) srcwnd = basewnd;
  return ret;
}

int PaintCanvas::beginPaint(OSWINDOWHANDLE wnd) {
  hWnd = wnd;        // NONPORTABLE
#if defined(WIN32)
  ASSERT(hWnd != NULL);
  hdc = BeginPaint(hWnd, &ps);
  ASSERT(hdc != NULL);
  return 1;
#elif defined(XWINDOW)
  ASSERT(hWnd != None);
  hdc = (HDC)MALLOC( sizeof( hdc_typ ) );
  hdc->d = hWnd;
  hdc->gc = XCreateGC( XWindow::getDisplay(), hdc->d, 0, NULL );
  ASSERT(hdc->gc != NULL);

  RegionI *clip = new RegionI();
  StdWnd::getUpdateRegion( hWnd, clip->getOSHandle() );
  StdWnd::validateRect( hWnd, NULL );
  selectClipRgn( clip );
  delete clip;

  return 1;
#else
#error port me!
#endif 
}

PaintBltCanvas::PaintBltCanvas() {
  hWnd = INVALIDOSWINDOWHANDLE;
  wnddc = NULL;
#if defined(XWINDOW)
  MEMSET( &hbmp, 0, sizeof( HBITMAP ) );
  //CUT MEMSET( &prevbmp, 0, sizeof( HBITMAP ) );
#else
  hbmp = NULL;
  prevbmp = NULL;
#endif
  bits = NULL;
  fcoord = TRUE;
  nonclient = FALSE;
}

PaintBltCanvas::~PaintBltCanvas() {
  RECT r;

  if (hdc == NULL) return;

  ASSERT(srcwnd != NULL);
        if (nonclient) //FG> nonclient painting fix
                srcwnd->getNonClientRect(&r);
        else
                srcwnd->getClientRect(&r); 

  // blt here
#if defined(WIN32)
  //GdiFlush();
  BitBlt(wnddc, r.left, r.top, r.right-r.left, r.bottom-r.top, hdc, 0, 0, SRCCOPY);

  //SelectClipRgn(hdc, NULL);
  // kill the bitmap and its DC
  SelectObject(hdc, prevbmp);
  DeleteDC(hdc);
  hdc = NULL;
  DeleteObject(hbmp);
  EndPaint(hWnd, &ps);        // end of wnddc
#elif defined(XWINDOW)
  if ( blitter ) {
    DCCanvas dcc( wnddc );
    Rect wr(r);
    blitter->blit( wr.left, wr.top, &dcc, 0, 0, 
		   wr.width(), wr.height() );
    
    delete blitter;
    blitter = NULL;
  }
#else
#error port me!
#endif
  
  bits=NULL;
  width=0;
  height=0;
  pitch=0;

  wnddc = NULL;
}

//FG> nonclient painting fix
int PaintBltCanvas::beginPaintNC(BaseWnd *basewnd) {
  nonclient = TRUE;
  return beginPaint(basewnd);
}

void PaintBltCanvas::getRcPaint(RECT *r) {
#if defined(WIN32)  
  *r = ps.rcPaint;
#elif defined(XWINDOW)
  Rect wr;
  wr.setRectLTRB(0,0,width,height);
  (*r) = (RECT)wr;
#else
#error port me!
#endif 
}

int PaintBltCanvas::beginPaint(BaseWnd *basewnd) {

  RECT r;
  
  if (nonclient)
    basewnd->getNonClientRect(&r); //FG> nonclient painting fix
  else
    basewnd->getClientRect(&r);

  if (r.right - r.left <= 0 || r.bottom - r.top <= 0) return 0;

  hWnd = basewnd->gethWnd();        // NONPORTABLE
  ASSERT(hWnd != INVALIDOSWINDOWHANDLE);

#if defined(WIN32)
  BITMAPINFO bmi;
  ZeroMemory(&bmi, sizeof bmi);
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = r.right - r.left;
  bmi.bmiHeader.biHeight = -(r.bottom - r.top);
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = 0;
  bmi.bmiHeader.biXPelsPerMeter = 0;
  bmi.bmiHeader.biYPelsPerMeter = 0;
  bmi.bmiHeader.biClrUsed = 0;
  bmi.bmiHeader.biClrImportant = 0;

  wnddc = BeginPaint(hWnd, &ps);

  ASSERT(wnddc != NULL);

  //GdiFlush();
  width=r.right-r.left;
  height=-ABS(r.bottom-r.top);
  pitch=width*4;
  hbmp = CreateDIBSection(wnddc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);  

  if (hbmp == NULL) {
    EndPaint(hWnd, &ps);        // end of wnddc
    wnddc = NULL;
    return 0;
  }

  // create tha DC
  hdc = CreateCompatibleDC(wnddc);
  if (hdc == NULL) {
    DeleteObject(hbmp);
    EndPaint(hWnd, &ps);        // end of wnddc
    wnddc = NULL;
    return 0;
  }
  prevbmp = (HBITMAP)SelectObject(hdc, hbmp);

  RegionI *clip = new RegionI(&ps.rcPaint);
  selectClipRgn(clip);
  delete clip;

#elif defined(XWINDOW)
  blitter = new BltCanvas( r.right - r.left, r.bottom - r.top );
  bits = blitter->getBits();
  hdc = blitter->getHDC();

  width=r.right-r.left;
  height=ABS(r.bottom-r.top);
  pitch=width*4;

  wnddc = (HDC)MALLOC( sizeof( hdc_typ ) );
  wnddc->d = basewnd->gethWnd();
  wnddc->gc = XCreateGC( XWindow::getDisplay(), hdc->d, 0, NULL );  

  RegionI *clip = new RegionI();
  StdWnd::getUpdateRegion( hWnd, clip->getOSHandle() );
  StdWnd::validateRect( hWnd, NULL );
  selectClipRgn( clip );
  delete clip;
#else
#error port me!
#endif // platform
  
  srcwnd = basewnd;

  return 1;
}

void *PaintBltCanvas::getBits() {
  return bits;
}

MemCanvas::MemCanvas(void *_bits, int w, int h, int p) {
  width = w;
  height = h;
  pitch = p;
  bits = _bits;
}

MemCanvas::MemCanvas(Bitmap *bmp) {
  width = bmp->getWidth();
  height = bmp->getHeight();
  pitch = bmp->getWidth()*4;
  bits = bmp->getBits();
}

MemCanvas::~MemCanvas() {
#if defined(WIN32)
  if (hdc) DeleteDC(hdc);
#elif defined(XWINDOW)
  XFreeGC( XWindow::getDisplay(), hdc->gc );
  if ( hdc->clip )
    XDestroyRegion( hdc->clip );
  FREE( hdc );
#else
#error port me!
#endif // platform
  hdc = NULL;
}

void MemCanvas::fillBits(COLORREF color, RECT *r) {
  if (r != NULL) {
    int w, h, pitch;
    getDim(&w, &h, &pitch);
    pitch >>= 2;
    int _w = MIN(r->right-r->left, w-r->left);
    int _h = MIN(r->bottom-r->top, h-r->top);
    if (_w < 0 || _h < 0) return;
    ARGB32 *bits = (ARGB32*)getBits()+r->top*pitch+r->left;
    while (_h--) {
      MEMFILL<DWORD>(bits, color, _w);
      bits+=pitch;
    }
  } else {
    DWORD *dwbits = (DWORD *)bits;
    MEMFILL<DWORD>(dwbits, color, width * height);
  }
}


int MemCanvas::createCompatible(Canvas *canvas) {
  ASSERT(canvas != NULL);
  //ASSERT(canvas->getHDC() != NULL);
  createCompatible(canvas->getHDC());
  srcwnd = canvas->getBaseWnd();
  return 1;
}

int MemCanvas::createCompatible(HDC dc) {
  ASSERT(dc!= NULL);
#if defined(WIN32)
  hdc = CreateCompatibleDC(dc);
#elif defined(XWINDOW)
  hdc = (HDC)MALLOC( sizeof( hdc_typ ) );
  hdc->d = dc->d;
  XCopyGC( XWindow::getDisplay(), dc->gc, 0, hdc->gc );
#else
#error port me!
#endif // platform
  ASSERT(hdc != NULL);
  srcwnd = NULL;
  return 1;
}

DCCanvas::DCCanvas(HDC clone, BaseWnd *srcWnd) {
  if (clone != NULL) cloneDC(clone, srcWnd);
}

DCCanvas::~DCCanvas() {
  hdc = NULL;
}

int DCCanvas::cloneDC(HDC clone, BaseWnd *srcWnd) {

  ASSERT(clone != NULL);
  hdc = clone;
  srcwnd = srcWnd;
  return 1;
}

SysCanvas::SysCanvas() {
#if defined(WIN32)
  hdc = GetDC(NULL);
#elif defined(XWINDOW)
  hdc = (HDC)MALLOC( sizeof( hdc_typ ) );
  hdc->d = XWindow::RootWin();
  hdc->gc = XCreateGC( XWindow::getDisplay(), hdc->d, 0, NULL );
#else
#error port me!
#endif // platform
}

SysCanvas::~SysCanvas() {
#if defined(WIN32)
  ReleaseDC(NULL, hdc);
#elif defined(XWINDOW)
  XFreeGC( XWindow::getDisplay(), hdc->gc );
  if ( hdc->clip )
    XDestroyRegion( hdc->clip );
  FREE( hdc );
#else
#error port me!
#endif // platform
  hdc = NULL;
}

DCBltCanvas::DCBltCanvas() {
  origdc = NULL;
#ifdef XWINDOW
  MEMSET( &hbmp, 0, sizeof( HBITMAP ) );
  //CUT MEMSET( &prevbmp, 0, sizeof( HBITMAP ) );
  blitter = NULL;
#else
  hbmp = prevbmp = NULL;
#endif
}

DCBltCanvas::~DCBltCanvas() {

  commitDC();

#if defined(WIN32)
  // kill the bitmap and its DC
  SelectObject(hdc, prevbmp);
  DeleteDC(hdc);
  hdc = NULL;
  DeleteObject(hbmp);
#elif defined(XWINDOW)
  if ( blitter ) {
    delete blitter;
    blitter = NULL;
  }
#else
#error port me!
#endif // platform

  // don't kill origdc, it's been cloned
}

int DCBltCanvas::setOrigDC(HDC neworigdc) { // FG> allows custom draw on lists to be much faster
  origdc = neworigdc;
  return 1;
}

int DCBltCanvas::commitDC(void) { //FG

  if (origdc) {
    RECT c;
#ifdef WIN32
    if (GetClipBox(origdc, &c) == NULLREGION)
#endif
      c = rect;

#if defined(WIN32)
    // shlap it down in its original spot
    //GdiFlush();
    BitBlt(origdc, c.left, c.top,
           c.right-c.left, c.bottom-c.top, hdc, c.left, c.top, SRCCOPY);
#elif defined(XWINDOW)
    DCCanvas dcc( origdc );

    blitter->blit( c.left, c.top, &dcc, c.left, c.top, 
                   c.right - c.left, c.bottom - c.top );
#else
#error port me!
#endif // platform
  }//origdc

  return 1;
}

int DCBltCanvas::cloneDC(HDC clone, RECT *r, BaseWnd *srcWnd) {
  origdc = clone;

  srcwnd = srcWnd;

  ASSERT(r != NULL);
  rect = *r;

#if defined(WIN32)
#if 1
  BITMAPINFO bmi;
  ZeroMemory(&bmi, sizeof bmi);
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = r->right - r->left;
  bmi.bmiHeader.biHeight = -ABS(r->bottom - r->top);
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = 0;
  bmi.bmiHeader.biXPelsPerMeter = 0;
  bmi.bmiHeader.biYPelsPerMeter = 0;
  bmi.bmiHeader.biClrUsed = 0;
  bmi.bmiHeader.biClrImportant = 0;
  hbmp = CreateDIBSection(origdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
  width=bmi.bmiHeader.biWidth;
  height=ABS(bmi.bmiHeader.biHeight);
  pitch=width*4;
#else
  hbmp = CreateCompatibleBitmap(clone, r->right - r->left, r->bottom - r->top);
#endif
  ASSERT(hbmp != NULL);

  // create tha DC
  hdc = CreateCompatibleDC(origdc);
  prevbmp = (HBITMAP)SelectObject(hdc, hbmp);
#elif defined(XWINDOW)
  blitter = new BltCanvas( r->right - r->left, r->bottom - r->top );
  bits = blitter->getBits();
  hdc = blitter->getHDC();

  width=hbmp.bmWidth;
  height=hbmp.bmHeight;
  pitch=width*4;
#else
#error port me!
#endif // platform
  // adjust their rect for them
  r->right -= r->left;
  r->left = 0;
  r->bottom -= r->top;
  r->top = 0;

  return 1;
}


BaseCloneCanvas::BaseCloneCanvas(CanvasBase *cloner) {
  if (cloner != NULL) clone(cloner);
}

int BaseCloneCanvas::clone(CanvasBase *cloner) {
  ASSERTPR(hdc == NULL, "can't clone twice");
  hdc = cloner->getHDC();
  bits = cloner->getBits();
  cloner->getDim(&width,&height,&pitch);
//  srcwnd = cloner->getBaseWnd();
  cloner->getOffsets(&xoffset, &yoffset);
  setTextFont(cloner->getTextFont());
  setTextSize(cloner->getTextSize());
  setTextBold(cloner->getTextBold());
  setTextOpaque(cloner->getTextOpaque());
  setTextUnderline(cloner->getTextUnderline());
  setTextItalic(cloner->getTextItalic());
  setTextAlign(cloner->getTextAlign());
  setTextColor(cloner->getTextColor());
  setTextBkColor(cloner->getTextBkColor());
  return (hdc != NULL);
}

BaseCloneCanvas::~BaseCloneCanvas() {
  hdc = NULL;
}

#ifdef WIN32 // DDSurfaceCanvas

DDSurfaceCanvas::DDSurfaceCanvas(LPDIRECTDRAWSURFACE surface, int w, int h) {
  surf = surface;
  _w = w;
  _h = h;  
  hdc = NULL;
  bits = NULL;
}

DDSurfaceCanvas::~DDSurfaceCanvas() {
  if (isready())
    exit();
}

int DDSurfaceCanvas::isready() {
  return bits != NULL;
}

void DDSurfaceCanvas::enter() {
  DDSURFACEDESC d={sizeof(d),};
  if ((surf->Lock(NULL,&d,DDLOCK_WAIT,NULL)) != DD_OK)
    return;

  surf->GetDC(&hdc);

  bits = d.lpSurface;
}

void DDSurfaceCanvas::exit() {
  surf->ReleaseDC(hdc);
  surf->Unlock(bits);
  bits = NULL;
  hdc = NULL;
}

#endif // WIN32 : DDSurfaceCanvas
