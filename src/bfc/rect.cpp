#include <precomp.h>
#include "rect.h"

void Rect::setFromRECT(const RECT *r) {
#if defined(WIN32) || defined (WASABI_WIN32RECT)
  left = r->left;
  top = r->top;
  right = r->right;
  bottom = r->bottom;
#elif defined(XWINDOW)
  left = r->x;
  top = r->y;
  right = r->x + r->width;
  bottom = r->y + r->height;
#else
#error port me!
#endif // platform
}


