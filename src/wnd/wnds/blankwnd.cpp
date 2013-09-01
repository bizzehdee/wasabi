#include "precomp.h"

#include "blankwnd.h"

#include <wnd/canvas.h>

BlankWnd::BlankWnd(RGB32 _color) : color(_color) { }

int BlankWnd::onPaint(Canvas *canvas) {
  PaintCanvas pc;
  if (canvas == NULL) {
    if (!pc.beginPaint(this)) return 0;
    canvas = &pc;
  }

  canvas->fillRect(&clientRect(), color);

  return 1;
}

