#include "precomp.h"

#include <wnd/bltcanvas.h>
#include <wnd/region.h>

#include "backbufferwnd.h"


// -----------------------------------------------------------------------
BackBufferWnd::BackBufferWnd() {
  backbuffer = 0;
  canvas_w = -1;
  canvas_h = -1;
  back_buffer = NULL;
}               

// -----------------------------------------------------------------------
BackBufferWnd::~BackBufferWnd() {
  delete back_buffer;
}

//------------------------------------------------------------------------
BltCanvas *BackBufferWnd::getBackBuffer() {
  return back_buffer;
}

// -----------------------------------------------------------------------
int BackBufferWnd::onPaint(Canvas *canvas) {

  BBWND_PARENT::onPaint(canvas);

  if (!canvas) return 1;

  RECT r;
  getClientRect(&r);

  if (back_buffer && r.right-r.left > 0 && r.bottom -r.top > 0) {

    int w = r.right-r.left;
    int h = r.bottom-r.top;

    if (canvas_w != w || canvas_h != h) {
      delete back_buffer;
      back_buffer = new BltCanvas(w, h);
      canvas_w = w;
      canvas_h = h;
    }

    RegionI *reg = new RegionI;
    canvas->getClipRgn(reg);
    back_buffer->selectClipRgn(reg);
    canvas->blit(r.left, r.top, back_buffer, 0, 0, w, h);
    back_buffer->selectClipRgn(NULL);
    delete reg;
  }

  return 1;
}

int BackBufferWnd::onSiblingInvalidateRgn(Region *r, RootWnd *who, int who_idx, int my_idx) {
  if (who_idx >= my_idx || !wantBackBuffer()) return 0;

  RECT rr;
  getClientRect(&rr);

  Region *_r = getRegion();
  RegionI *__r=NULL;

  if (!_r) {
    __r = new RegionI();
    _r = __r;
    _r->setRect(&rr);
  } else {
    _r->offset(rr.left, rr.top);
  }

  int intersect = _r->intersectRgn(r);
  if (intersect)
    r->addRegion(_r);

  delete __r;

  return intersect;
}

