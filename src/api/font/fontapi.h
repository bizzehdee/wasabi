#ifndef __FONTAPI_H
#define __FONTAPI_H

#include "api_font.h"

class FontApi : public api_fontI {
public:
  FontApi();
  virtual ~FontApi();
  void font_textOut(CanvasBase *c, int style, int x, int y, int w, int h, const char *txt);
  int font_getInfo(CanvasBase *c, const char *font, int infoid, const char *txt, int *w, int *h);
};

extern api_font *fontApi;

#endif
