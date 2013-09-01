#ifndef _WASABI_FILTER_H
#define _WASABI_FILTER_H

#include "argb32.h"

// this is a point filter, as in, each pixel depends only on its original
// value, and not the values of any neighboring pixels
template <class T>
class PointFilter {
public:
  PointFilter(ARGB32 *_bits, int _cw, int _ch, float _p1=0, float _p2=0,
              int _stride=-1) :
    bits(_bits), cw(_cw), ch(_ch), p1(_p1), p2(_p2),
    stride(_stride > 0 ? _stride : cw)
  { }

  void exec() {
    ARGB32 *src = bits;
    for (int y = 0; y < ch; y++) {
      for (int x = 0; x < cw; x++) {
        src[x] = T::execPixel(src[x], p1, p2);
      }
      src += stride;
    }
  }

protected:
  ARGB32 *bits;
  int cw, ch, stride;	// canvas width/height/stride
  float p1, p2;
};

#endif
