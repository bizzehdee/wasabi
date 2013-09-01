#ifndef WASABI_DRAWPOLY_H
#define WASABI_DRAWPOLY_H

#include <bfc/std.h>

class Draw {
public:
  static void beginPolygon(ARGB32 *bits, int w, int h, ARGB32 color);
  static void addPoint(int x, int y);
  static void endPolygon();
  static void drawPointList(ARGB32 *bits, int w, int h, const char *pointlist);

  // handles clipping
  // basu sez: the alpha value of the color is used for painting, but NOT for blending
  // lone sez: alpha selects more or less of the original vs new color, it is the param used in ADJ1, so using 0 will replace the old color by the new color entirely (including its alpha), ie, NOT blending
  //in this case, if color has an alpha of 0, the new color will also have an alpha of 0"
  // :)
  static void rect(ARGB32 *bits, int cw, int ch, int x, int y, int w, int h, ARGB32 color, int filled=FALSE, int alpha=255, int apply_color_alpha=TRUE);
  static void hline(ARGB32 *bits, int cw, int ch, int x1, int y1, int x2, ARGB32 color, int alpha=255, int apply_color_alpha=TRUE);
};

// x,y;x,y;x,y;x,y=R,G,B|x,y;x,y;x,y;=R,G,B

#endif
