#ifndef STATICTEXTWND_H
#define STATICTEXTWND_H

#include <wnd/virtualwnd.h>

#define StaticTextWnd_PARENT VirtualWnd

#define FONTSTYLE_NORMAL    0
#define FONTSTYLE_BOLD      1
#define FONTSTYLE_ITALIC    2
#define FONTSTYLE_UNDERLINE 4

class StaticTextWnd: public StaticTextWnd_PARENT {
public:
  StaticTextWnd();
  virtual ~StaticTextWnd();

	virtual int onPaint(Canvas *c);
  
  void setTextColor(COLORREF color);
  void setTextShadowColor(COLORREF color);
  void setDisabledTextColor(COLORREF color);
  void setDisabledTextShadowColor(COLORREF color);
  void setTextSize(int size);
  void setTextFont(const char *facename);
  void setTextStyle(int styleflags);

  virtual int getPreferences(int what);

private:
  void prepareCanvas(Canvas *c);
  void restoreCanvas(Canvas *c);

  COLORREF m_textcolor, m_textshadowcolor;
  COLORREF m_disabledtextcolor, m_disabledtextshadowcolor;
  int m_textsize;
  String m_textfont;
  int m_textstyle;
};

#endif

