#ifndef WASABI_FLOODFILL_H
#define WASABI_FLOODFILL_H

#include <bfc/std.h>
#include <bfc/stack.h>

class FloodFill32 {
public:
  FloodFill32(ARGB32 *bits, int w, int h, int eightway=TRUE);

  void fill(int x, int y, ARGB32 to_color=0);

protected:
  // you can override these if you want to ... but make sure your
  // adjustPixel changes the pixel enough so that testPixel returns FALSE!
  virtual int testPixel(ARGB32 color);
  virtual ARGB32 adjustPixel(ARGB32 pixel);

  ARGB32 from_color, to_color;

protected:
  ARGB32 *bits;
  int w, h;

private:
  void pushPixel(int x, int y);

  Stack<POINT> stack;
  int eightway;
};

class MagicWandFill : public FloodFill32 {
public:
  MagicWandFill(ARGB32 *bits, int w, int h, int eightway=TRUE);

  virtual int testPixel(ARGB32 color);
  virtual ARGB32 adjustPixel(ARGB32 pixel);

  void setTolerance(float t);
  void addMagicPixel(int x, int y);
  void addMagicPixelRegion(int x, int y, int w, int h);
  void clearMagicPixels();

protected:
  float pixelCmp(ARGB32 a, ARGB32 b);

private:
  float tolerance;
  TList<ARGB32> samples;
};

#endif
