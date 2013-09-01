#ifndef WASABI_CONVOLVE_H
#define WASABI_CONVOLVE_H

// 10-may-2004 BU fixed bad bad bug that made it only appear to work
//                added standard types

class Convolve3x3 {
public:
  Convolve3x3(ARGB32 *bits, int w, int h);

  void setPos(int x, int y, float v);
  void setMultiplier(float m);
  void setDivider(float d);
  void setOutputMask(ARGB32 mask);
  void setOutofBoundsColor(ARGB32 oob);

  void convolve();

  void setStdType(int type);

  enum {
    IDENTITY, BLUR
  };

private:
  ARGB32 *bits;
  int w, h;
  float vals[3][3];
  float multiplier, divisor;
  ARGB32 outputmask, oobcolor;
};

#endif
