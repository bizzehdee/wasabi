#include "precomp.h"

#include <wnd\canvas.h>

#include "fontapi.h"
#include "fontdef.h"
//fixme #include "../../studio/font.h"
#ifdef WIN32
#include "win32\truetypefont_win32.h"
#else
#error port me
#endif

api_font *fontApi = NULL;

static svc_font *static_font;

FontApi::FontApi() {
//fixme  Font::init();
//  if (font == NULL) {
    static_font = new TrueTypeFont_Win32;
//  }
//  font->setFontFace("Courier");
}

FontApi::~FontApi() {
//fixme  Font::uninstallAll();
  delete static_font; static_font = NULL;
//  SvcEnum::release(font); font = NULL;
}

void FontApi::font_textOut(CanvasBase *c, int style, int x, int y, int w, int h, const char *txt) {
  if (txt == NULL || *txt == '\0') return;	// empty string = NOP
  if (static_font == NULL) {
    static_font = FontSvcEnum().getNext();
ASSERT(static_font != NULL);//hehe FUCKO
  }
//fixme  Font::dispatchTextOut(c, style, x, y, w, h, txt);
//DebugString("font: %s", txt);

  int tsize = c->getTextSize();
  int tbold = c->getTextBold();
  int topaque = c->getTextOpaque();
  int titalic = c->getTextItalic();
  int tunder = c->getTextUnderline();
  int talign = c->getTextAlign();
  int taa = c->getTextAntialias();
  COLORREF tcolor = BSWAP(c->getTextColor())>>8;
  COLORREF tbkcolor = BSWAP(c->getTextBkColor())>>8;
  int toffsetx, toffsety;
  c->getOffsets(&toffsetx, &toffsety);

  // set the font face to match too!
  static_font->setFontFace(c->getTextFont());
  
  RECT trect; 
  trect.left = x; 
  trect.top = y; 
  trect.right = x + w; 
  trect.bottom = y + h;

  switch (style) {
  case WA_FONT_TEXTOUT_NORMAL:
    static_font->textOut(c, x, y, txt, tsize, tbold, topaque, tunder, titalic, tcolor, tbkcolor, toffsetx, toffsety, taa);
  break;
  case WA_FONT_TEXTOUT_RECT:
    static_font->textOut(c, x, y, w, h, txt, tsize, tbold, topaque, tunder, titalic, talign, tcolor, tbkcolor, toffsetx, toffsety, taa);
  break;
  case WA_FONT_TEXTOUT_ELLIPSED:
    static_font->textOutEllipsed(c, x, y, w, h, txt, tsize, tbold, topaque, tunder, titalic, talign, tcolor, tbkcolor, toffsetx, toffsety, taa);
  break;
  case WA_FONT_TEXTOUT_WRAPPED:
    static_font->textOutWrapped(c, x, y, w, h, txt, tsize, tbold, topaque, tunder, titalic, talign, tcolor, tbkcolor, toffsetx, toffsety, taa);
  break;
  case WA_FONT_TEXTOUT_WRAPPEDPATHED:
    static_font->textOutWrappedPathed(c, x, y, w, txt, tsize, tbold, topaque, tunder, titalic, talign, tcolor, tbkcolor, toffsetx, toffsety, taa);
  break;
  case WA_FONT_TEXTOUT_CENTERED:
    static_font->textOutCentered(c, &trect, txt, tsize, tbold, topaque, tunder, titalic, talign, tcolor, tbkcolor, toffsetx, toffsety, taa);
  break;
  default:
    static_font->textOut(c, x, y, txt, tsize, tbold, topaque, tunder, titalic, tcolor, tbkcolor, toffsetx, toffsety, taa);
  }
}

int FontApi::font_getInfo(CanvasBase *c, const char *fontname, int infoid, const char *txt, int *w, int *h) {

//fixme  return Font::dispatchGetInfo(c, font, infoid, txt, w, h);

  static_font->setFontFace(c->getTextFont());

//FUCKO: this is dumb to pass in both c and c->getText*()... should just pass c
  if ((infoid & WA_FONT_GETINFO_WIDTHHEIGHT) == WA_FONT_GETINFO_WIDTHHEIGHT) {
    int rw, rh;
    static_font->getTextExtent(c, txt, &rw, &rh, c->getTextSize(), c->getTextBold(), c->getTextUnderline(), c->getTextItalic(), c->getTextAntialias());
    if (w) *w = rw;
    if (h) *h = rh;
  } else {
    if (infoid & WA_FONT_GETINFO_WIDTH) {
      if (w) *w = static_font->getTextWidth(c, txt, c->getTextSize(), c->getTextBold(), c->getTextUnderline(), c->getTextItalic(), c->getTextAntialias());
    }
    if (infoid & WA_FONT_GETINFO_HEIGHT) {
      if (h) *h = static_font->getTextHeight(c, txt, c->getTextSize(), c->getTextBold(), c->getTextUnderline(), c->getTextItalic(), c->getTextAntialias());
    }
  }
  return 1;
}
