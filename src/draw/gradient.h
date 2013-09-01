#ifndef WASABI_GRADIENT_H
#define WASABI_GRADIENT_H

#include <bfc/std.h>

class COMEXP Gradient {
public:
  Gradient();
  virtual ~Gradient();

  void setX1(float x1);
  void setY1(float y1);
  void setX2(float x2);
  void setY2(float y2);
  
  void clearPoints();
  void addPoint(float pos, ARGB32 color);

  // "pos=color;pos=color" "0.25=34,45,111"
  void setPoints(const char *str);

  void setReverseColors(int c);

  void setAntialias(int c);

  void setMode(const char *mode);

  void setGammaGroup(const char *group);

  // note: this will automatically premultiply against alpha
  void renderGradient(ARGB32 *bits, int width, int height);

protected:
  virtual void onParamChange() { }

  ARGB32 getPixelCirc(double x, double y);

private:
  float gradient_x1, gradient_y1, gradient_x2, gradient_y2;
  class GradientList;
  GradientList *list;
  void renderGrad(ARGB32 *bits, int len, int *positions);
  int reverse_colors;
  int antialias;
  String mode, gammagroup;
};

#endif
