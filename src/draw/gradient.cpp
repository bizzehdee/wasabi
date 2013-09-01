#include <precomp.h>

#include "gradient.h"

#include <math.h>//floor
#include <bfc/ptrlist.h>
#include <parse/pathparse.h>
#include <draw/filteredcolor.h>
#include <draw/argb32.h>

#define DEFAULT_GRAD_MODE "linear"

/*CUT, see std.h
template<class T> inline void SWAP(T &a, T &b) {
  T c = a;
  a = b;
  b = c;
}
*/

class GradientPoint {
public:
  GradientPoint(float p, ARGB32 c, const char *group="") : pos(p), color(c, group) { }
  float pos;
  double dist;
  FilteredColor color;
  float x, y;
  static int compareItem(GradientPoint *p1, GradientPoint* p2) {
    int r = CMP3(p1->pos, p2->pos);
    if (r == 0) return CMP3(p1, p2);
    else return r;
  }
};

class Gradient::GradientList : public PtrListQuickSorted<GradientPoint, GradientPoint> { };

inline unsigned int LERPu(unsigned int a, unsigned int b, float p) {
//  ASSERT(p >= 0);
//  ASSERT(p <= 1.f);
  unsigned int ret = (unsigned int)((float)b * p + (float)a * (1.f - p));
  return ret;
}

inline float LERPf(float a, float b, float p) {
//  ASSERT(p >= 0);
//  ASSERT(p <= 1.f);
  return (b * p + a * (1.f - p));
}

Gradient::Gradient() :
  gammagroup("")
{
  gradient_x1 = 0.0f;
  gradient_y1 = 0.0f;
  gradient_x2 = 1.0f;
  gradient_y2 = 1.0f;
  reverse_colors = 0;
  antialias = 0;
  mode = DEFAULT_GRAD_MODE;
  list = new GradientList;
  list->addItem(new GradientPoint(0.0f, 0xff00ff00));
  list->addItem(new GradientPoint(.5, 0x000000ff));
  list->addItem(new GradientPoint(1.0f, 0xffff0000));
}

Gradient::~Gradient() {
  list->deleteAll();
  delete list;
}

void Gradient::setX1(float x1) {
  gradient_x1 = x1;
  onParamChange();
}

void Gradient::setY1(float y1) {
  gradient_y1 = y1;
  onParamChange();
}

void Gradient::setX2(float x2) {
  gradient_x2 = x2;
  onParamChange();
}

void Gradient::setY2(float y2) {
  gradient_y2 = y2;
  onParamChange();
}

void Gradient::clearPoints() {
  list->deleteAll();
  onParamChange();
}

void Gradient::addPoint(float pos, ARGB32 color) {
  list->addItem(new GradientPoint(pos, color, gammagroup));
  onParamChange();
}

void Gradient::setPoints(const char *pointlist) {
  clearPoints();
  if (pointlist == NULL || *pointlist == '\0') return;
// 0.5=233,445,245,123;
  PathParser pp(pointlist, ";");
  if (pp.getNumStrings() <= 0) return;
  for (int i = 0; i < pp.getNumStrings(); i++) {
    PathParser rp(pp.enumString(i), "=");
    if (rp.getNumStrings() != 2) continue;
    float pos = (float)ATOF(rp.enumString(0));
    /*ARGB32 color = (ARGB32)WASABI_API_SKIN->parse(rp.enumString(1), "coloralpha");*/
    PathParser cp(rp.enumString(1), ",");
    if (cp.getNumStrings() < 3 || cp.getNumStrings() > 4) continue;
    ARGB32 color = MKARGB32( (cp.enumString(3) ? ATOI(cp.enumString(3)) : 0xff),
                              ATOI(cp.enumString(0)),
                              ATOI(cp.enumString(1)),
                              ATOI(cp.enumString(3)));
    addPoint(pos, color);
  }
}

void Gradient::setReverseColors(int c) {
  reverse_colors = c;
}

void Gradient::setAntialias(int c) {
  antialias = c;
}

void Gradient::setMode(const char *_mode) {
  mode = _mode;
  if (mode.isempty()) mode = DEFAULT_GRAD_MODE;
}

void Gradient::setGammaGroup(const char *group) {
  gammagroup = group;
  // reset our points
  foreach(list)
    list->getfor()->color.setColorGroup(group);
  endfor
}

static inline ARGB32 colorLerp(ARGB32 color1, ARGB32 color2, float pos) {
  unsigned int a1 = (color1>>24) & 0xff;
  unsigned int a2 = (color2>>24) & 0xff;
  unsigned int r1 = (color1>>16) & 0xff;
  unsigned int r2 = (color2>>16) & 0xff;
  unsigned int g1 = (color1>>8) & 0xff;
  unsigned int g2 = (color2>>8) & 0xff;
  unsigned int b1 = (color1) & 0xff;
  unsigned int b2 = (color2) & 0xff;
  return (LERPu(a1, a2, pos)<<24) | (LERPu(r1, r2, pos) << 16) | (LERPu(g1,g2,pos)<<8) | LERPu(b1, b2, pos);
}

void Gradient::renderGrad(ARGB32 *ptr, int len, int *positions) {

  int npos = list->getNumItems();
  ASSERT(npos >= 2);

  ARGB32 color1, color2;
  int x1, x2;
  for (int i = 0; i < npos-1; i++) {
    color1 = list->q(i)->color.getColor();
    color2 = list->q(i+1)->color.getColor();

    if (reverse_colors) {
      color1 = BGRATOARGB(color1);
      color2 = BGRATOARGB(color2);
    }

    x1 = positions[i];
    x2 = positions[i+1];
    if (x1 == x2) continue;
    // hflip if need be
    if (x1 > x2) {
      SWAP(x1, x2);
      SWAP(color1, color2);
    }
    float c = 0;
    float segment_len = (float)((x2 - x1)+1);

    if (x1 < 0) {	// clip left
      c += -x1;
      x1 = 0;
    }
    for (int x = x1; x < x2; x++, c += 1.0f) {
      if (x >= len) break;	// clip right
      ptr[x] = colorLerp(color1, color2, c / segment_len);
    }
  }
#if 0//later
  // fill in left if needed
  if (positions[0] > 0) MEMFILL<ARGB32>(ptr, list->q(0)->color, positions[0]);

  // and right if needed
  int rpos = positions[npos-1];
  if (rpos < len) MEMFILL<ARGB32>(ptr+rpos, list->getLast()->color, len-rpos);
#endif
}

void Gradient::renderGradient(ARGB32 *bits, int w, int h) {
  list->sort();

  ARGB32 default_color = 0xffff00ff;
  if (list->getNumItems() == 1) default_color = list->q(0)->color.getColor();
  // blank it out to start
  MEMFILL<ARGB32>(bits, default_color, w * h);

  if (list->getNumItems() > 1) {
    if (mode.iscaseequal("linear")) {
//FUCKO: not if endcaps are filled

      // force non-vertical lines
      if (ABS(gradient_x1 - gradient_x2) < 0.0005f) gradient_x2 = gradient_x1+0.0005f;

      float px1 = gradient_x1 * w, py1 = gradient_y1 * h;
      float px2 = gradient_x2 * w, py2 = gradient_y2 * h;

      // convert to y = mx + b
      float m = (py2 - py1)/(px2 - px1);
      m = -1.f/m;	// invert the slope

      int nitems = list->getNumItems();

      // get the in-pixels x and y for points on the gradient
      for (int i = 0; i < nitems; i++) {
        GradientPoint *gp = list->q(i);
        // need x and y given pos
        gp->x = LERPf(px1, px2, gp->pos);
        gp->y = LERPf(py1, py2, gp->pos);
      }

      MemBlock<int> positions(nitems);
      for (int _y = 0; _y < h; _y++) {
        // project all the color points onto this scanline
        for (int i = 0; i < nitems; i++) {
          GradientPoint *gp = list->q(i);
// y = mx + b
// b = y - mx;
          float newb = gp->y - m * gp->x;
// y = mx + newb
// y - newb = mx
// (y - newb)/m = x
          float xxx = (_y - newb)/m;
          positions[i] = (int)floor(xxx+0.5f);
        }
        renderGrad(bits+_y*w, w, positions);
      }
    } else if (mode.iscaseequal("circular")) {
      double tot = SQRT(SQR(gradient_x1 - gradient_x2) + SQR(gradient_y1 - gradient_y2));
      foreach(list)
        GradientPoint *gp = list->getfor();
        gp->dist = gp->pos * tot;
      endfor

      ARGB32 *dst = bits;
      for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
          ARGB32 c;
          if (antialias) {
            double fx = (((double)x)-0.5f) / (double)w;
            double fy = (((double)y)-0.5f) / (double)h;
            ARGB32 ul = getPixelCirc(fx, fy);
            fx = (((double)x)+0.5f) / (double)w;
            fy = (((double)y)-0.5f) / (double)h;
            ARGB32 ur = getPixelCirc(fx, fy);
            fx = (((double)x)+0.5f) / (double)w;
            fy = (((double)y)+0.5f) / (double)h;
            ARGB32 lr = getPixelCirc(fx, fy);
            fx = (((double)x)-0.5f) / (double)w;
            fy = (((double)y)+0.5f) / (double)h;
            ARGB32 ll = getPixelCirc(fx, fy);
            c = colorLerp(colorLerp(ll, lr, 0.5f), colorLerp(ul, ur, 0.5f), 0.5);
          } else {
            double fy = (double)y / (double)h;
            double fx = (double)x / (double)w;
            c = getPixelCirc(fx, fy);
          }
          *dst++ = c;
        }
      }
    }
  }//list.getNumItems()>1

  StdMath::premultiplyARGB32(bits, w * h);
}

ARGB32 Gradient::getPixelCirc(double fx, double fy) {
  int nitems = list->getNumItems();
  //double dist = SQR(fx - gradient_x1) + SQR(fy - gradient_y1);
  double dist = SQRT(SQR(fx - gradient_x1) + SQR(fy - gradient_y1));
  ARGB32 c = 0xff00ff00;
  if (dist <= list->q(0)->dist)
    c = list->q(0)->color.getColor();
  else if (dist >= list->getLast()->dist)
    c = list->getLast()->color.getColor();
  else for (int i = 0; i < nitems-1; i++) {
    if (list->q(i)->dist <= dist && list->q(i+1)->dist >= dist) {
      double pdist = list->q(i+1)->dist - list->q(i)->dist;
      double pp = dist - list->q(i)->dist;
      pp /= pdist;
      if (list->q(i)->color.getColor() == list->q(i+1)->color.getColor())
        c = list->q(i)->color.getColor();
      else
        c = colorLerp(list->q(i)->color.getColor(), list->q(i+1)->color.getColor(), pp);
      break;
    }
  }
  if (reverse_colors) c = BGRATOARGB(c);
  return c;
}
