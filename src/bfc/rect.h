#ifndef _WASABI_RECT_H_
#define _WASABI_RECT_H_

#include <bfc/platform/platform.h>
#include <bfc/std.h>
#include <bfc/ptrlist.h>

// Wrapper for platform-specific RECTs.
// Mostly auto-ported with Std::.
// Only thing to port for each platform is the setRECT(RECT*) method (rect.cpp).
namespace wasabi  {
class NOVTABLE Rect {
public:
  Rect() : left(0), top(0), right(0), bottom(0) {}
  Rect(const RECT &r) { setFromRECT(&r); }
  Rect(const RECT *r) { setFromRECT(r);  }

  int left, top, right, bottom;
  
  inline unsigned int width() const { return right - left; }
  inline unsigned int height() const { return bottom - top; }
  
  inline POINT topLeft() const { POINT p= {left,top}; return p; }
  inline POINT bottomRight() const { POINT p= {right,bottom}; return p; }
  
  inline operator RECT() { return Std::makeRect(left, top, right, bottom); }
  inline Rect operator=(const RECT r) { setFromRECT(&r); return *this; }
  
  void setFromRECT(const RECT *r);
  inline int setRectLTRB(int _left=0, int _top=0, int _right=0, int _bottom=0)
  { left = _left; top = _top; right = _right; bottom = _bottom; }
  inline int setRectXYWH(int x=0, int y=0, unsigned int width=0, unsigned int height=0)
  { left = x; top = y; right = x + width; bottom = y + height; }
};
}
#endif // _WASABI_RECT_H_
