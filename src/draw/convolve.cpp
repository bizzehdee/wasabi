#include "precomp.h"

#include "convolve.h"

#include <bfc/std.h>
#include <draw/argb32.h>
#include <draw/drawpoly.h>
#include <bfc/util/profiler.h>

Convolve3x3::Convolve3x3(ARGB32 *_bits, int _w, int _h) : bits(_bits), w(_w), h(_h) {
  setStdType(IDENTITY);
  outputmask = MKARGB32(255,255,255,255);
  oobcolor = MKARGB32(0,0,0,0);
}

void Convolve3x3::setPos(int x, int y, float v) {
  ASSERT(x >= -1 && x <= 1 && y >= -1 && y <= 1);
  vals[y+1][x+1] = v;
}

void Convolve3x3::setMultiplier(float m) {
  multiplier = m;
}

void Convolve3x3::setDivider(float divider) {
  if (ABS(divider) <= 0.0000001) divider = (float)(SIGN(divider)*0.0000001);
  divisor = divider;
}

void Convolve3x3::setOutputMask(ARGB32 mask) {
  outputmask = mask;
}

void Convolve3x3::setOutofBoundsColor(ARGB32 oob) {
  oobcolor = oob;
}

template<int colors> void _convolve(ARGB32 *bits, int w, int h,
  float oo_divisor,
  MemMatrix<ARGB32> &temp, float mul_vals[3][3], ARGB32 outputmask) {
  ARGB32 *dest = bits;
  ARGB32 notoutputmask = ~outputmask;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      float ra=0, rr=0, rg=0, rb=0;
      for (int a = -1; a <= 1; a++) {
        for (int b = -1; b <= 1; b++) {
          int px = x + b, py = y + a;
          ARGB32 c = temp(px+1, py+1);
          float val = mul_vals[b+1][a+1];
          ra += ((float)ALP(c)) * val;
          if (colors) {
            rr += ((float)RED(c)) * val;
            rg += ((float)GRN(c)) * val;
            rb += ((float)BLU(c)) * val;
          }
        }
      }

      ra *= oo_divisor;
      if (colors) {
        rr *= oo_divisor;
        rg *= oo_divisor;
        rb *= oo_divisor;
      }

      unsigned int al, r, g, b;
      al = MINMAX<unsigned int>((unsigned int)floor(ra+0.5f), 0, 255);
      if (colors) {
        r = MINMAX<unsigned int>((unsigned int)floor(rr+0.5f), 0, 255);
        g = MINMAX<unsigned int>((unsigned int)floor(rg+0.5f), 0, 255);
        b = MINMAX<unsigned int>((unsigned int)floor(rb+0.5f), 0, 255);
      }
      ARGB32 v = *dest;
      v &= notoutputmask;
      if (colors) {
        v |= (MKARGB32(al, r, g, b) & outputmask);
      } else {
        v |= MKARGB32_ALPHA(al) & outputmask;
      }
      *dest ++ = v;
    }
    Std::usleep(0);	// piggy
  }
}

void Convolve3x3::convolve() {
  if (bits == NULL || w <= 0 || h <= 0 || outputmask == 0) return; // nothin'

//FUCKO man this is such a hack! better to just make the main loop run from 1,1 to w-1,h-1
//and then special case the corners and edges
  MemMatrix<ARGB32> temp(w+2, h+2/*, bits*/);	// make a copy
  MEMFILL<ARGB32>(temp.m(), oobcolor, (w+2)*(h+2));
  for (int y = 0; y < h; y++) {
    MEMCPY32(temp.m2(1,y+1), bits+y*w, w);
  }

  // premultiply filter vals with constant multiplier
  float mulvals[3][3];
  for (int a = 0; a < 3; a++)
    for (int b = 0; b < 3; b++)
      mulvals[a][b] = vals[a][b] * multiplier;

//DebugString("output mask %x comp %x",outputmask, MKARGB32(0,255,255,255));
  if (outputmask & MKARGB32(0,255,255,255)) {// colors being output
ASSERT(outputmask & 0x00ffffff);
//DebugString("one");
    _convolve<1>(bits, w, h, 1.f/divisor, temp, mulvals, outputmask);
  } else { // alpha only
//DebugString("zero");
ASSERT(!(outputmask & 0x00ffffff));
    _convolve<0>(bits, w, h, 1.f/divisor, temp, mulvals, outputmask);
  }
}

void Convolve3x3::setStdType(int type) {
  switch (type) {
    case IDENTITY:
      ZERO(vals);
      vals[1][1] = 1;
      setMultiplier(1.0);
      setDivider(1.0);
    break;
    case BLUR:
      MEMFILL<float>((float*)vals, 1.0, 3*3);
      setMultiplier(1.0);
      setDivider(9.0);
    break;
  }
}
