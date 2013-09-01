#include <precomp.h>
#include <wnd/canvas.h>
#include "statictextwnd.h"

StaticTextWnd::StaticTextWnd() : m_textsize(13),
                                 m_textfont("Tahoma"),
                                 m_textstyle(0),
                                 m_textcolor(0),
                                 m_textshadowcolor(0),
                                 m_disabledtextcolor(0),
                                 m_disabledtextshadowcolor(0) {
  m_textcolor = GetSysColor(COLOR_WINDOWTEXT);
}

StaticTextWnd::~StaticTextWnd() {
}

int StaticTextWnd::onPaint(Canvas *c) {
  StaticTextWnd_PARENT::onPaint(c);
  RECT r;
  getClientRect(&r);

  prepareCanvas(c);

  COLORREF color = m_textcolor;
  if (m_disabledtextcolor != 0 && !isEnabled()) color = m_disabledtextcolor;
  COLORREF shadow = m_textshadowcolor;
  if (m_disabledtextshadowcolor != 0 && !isEnabled()) shadow = m_disabledtextshadowcolor;

  if (shadow != 0) {
    c->pushTextColor(shadow);
    c->textOut(r.left+1+1, r.top+1, getName());
  }
  c->popTextColor();
  c->pushTextColor(color);

  // right/bottom margins are only used by autowh
  c->textOut(r.left+1, r.top, getName());

  c->popTextColor();
  
  restoreCanvas(c);

  return 1;
}

void StaticTextWnd::prepareCanvas(Canvas *c) {
  if (m_textsize != -1) c->pushTextSize(m_textsize);
  if (!m_textfont.isempty()) c->pushTextFont(m_textfont);
  if (m_textstyle != 0) {
    c->pushTextBold((m_textstyle & FONTSTYLE_BOLD) ? 1 : 0);
    c->pushTextItalic((m_textstyle & FONTSTYLE_ITALIC) ? 1 : 0);
    c->pushTextUnderline((m_textstyle & FONTSTYLE_UNDERLINE) ? 1 : 0);
  }
}

void StaticTextWnd::restoreCanvas(Canvas *c) {
  if (m_textstyle != 0) {
    c->popTextUnderline();
    c->popTextItalic();
    c->popTextBold();
  }
  if (!m_textfont.isempty()) c->popTextFont();
  if (m_textsize != -1) c->popTextSize();
}

void StaticTextWnd::setTextColor(COLORREF color) {
  m_textcolor = color;
  invalidate();
}

void StaticTextWnd::setTextShadowColor(COLORREF color) {
  m_textshadowcolor = color;
  invalidate();
}

void StaticTextWnd::setDisabledTextColor(COLORREF color) {
  m_disabledtextcolor = color;
  invalidate();
}

void StaticTextWnd::setDisabledTextShadowColor(COLORREF color) {
  m_disabledtextshadowcolor = color;
  invalidate();
}

void StaticTextWnd::setTextSize(int size) {
  m_textsize = size;
  invalidate();
}

void StaticTextWnd::setTextFont(const char *facename) {
  m_textfont = facename;
  invalidate();
}

void StaticTextWnd::setTextStyle(int style) {
  m_textstyle = style;
}

int StaticTextWnd::getPreferences(int what) {
  if (what == SUGGESTED_W ||
      what == SUGGESTED_H) {
    SysCanvas sc;
    prepareCanvas(&sc);

    int margins[4];
    getMargins(&margins[0], &margins[1], &margins[2], &margins[3]);

    int r;
    if (what == SUGGESTED_W) r = sc.getTextWidth(getName()) + margins[0] + margins[2] + 2;
    else r = sc.getTextHeight(getName()) + margins[1] + margins[3] + 1;

    restoreCanvas(&sc);
    
    return r;
  }
  return StaticTextWnd_PARENT::getPreferences(what);
}

