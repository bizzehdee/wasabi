#include <precomp.h>
#include "colorpickerwnd.h"
#include <draw/bitmap.h>
#include <bfc/std_wnd.h>

ColorPickerWnd::ColorPickerWnd() {
  m_control_hue = -1;
  m_control_sat = 0;
  m_control_lum = 0;
  m_huewheel_width = 24.0;
  m_click_circle = m_click_rectangle = 0;
  m_cursorbmp = NULL;
//  m_slmode = 1;
}

ColorPickerWnd::~ColorPickerWnd() {
  if (m_delete_cursor) delete m_cursorbmp;
};

void ColorPickerWnd::setHueWheelWidth(double width) {
  m_huewheel_width = width;
}

void ColorPickerWnd::setCursorBmp(Bitmap *bmp, int autodelete/* =1 */) {
  m_cursorbmp = bmp;
  m_delete_cursor = autodelete;
}

void ColorPickerWnd::hslToRgb(double h, double s, double l, double *r, double *g, double *b) {
  double x,y,z;
  double v = (l <= 0.5) ? (l*(1.0+s)) : (l+s-l*s);
  if ((s == 0.0) || (h == -1.0)) { *r=l; *g=l; *b=l; return; }
  y = 2.0*l-v;
  x = y+(v-y)*(6.0*h-floor(6.0*h));
  z = v-(v-y)*(6.0*h-floor(6.0*h));
  switch ((int) (6.0*h)) {
    case 0: *r=v; *g=x; *b=y; break;
    case 1: *r=z; *g=v; *b=y; break;
    case 2: *r=y; *g=v; *b=x; break;
    case 3: *r=y; *g=z; *b=v; break;
    case 4: *r=x; *g=y; *b=v; break;
    case 5: *r=v; *g=y; *b=z; break;
    default: *r=v; *g=x; *b=y; break;
  }
}

void ColorPickerWnd::rgbToHsl(double r, double g, double b, double *hue, double *saturation, double *luminosity) {
  #define Epsilon 1.0e-12
  double delta, max, min;
  max=MAX(r,MAX(g,b));
  min=MIN(r,MIN(g,b));
  *hue=(-1.0);
  *saturation=0.0;
  *luminosity=(0.5+Epsilon)*(min+max);
  delta=max-min;
  if (delta == 0.0) return;
  *saturation=delta/((*luminosity <= 0.5) ? (min+max) : (2.0-max-min));
  if (r == max) {
    *hue=(g == min ? 5.0+(max-b)/delta : 1.0-(max-g)/delta);
  } else {
    if (g == max) *hue=(b == min ? 1.0+(max-r)/delta : 3.0-(max-b)/delta);
    else *hue=(r == min ? 3.0+(max-g)/delta : 5.0-(max-r)/delta);
  }
  *hue/=6.0;
}

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795029
#endif

int ColorPickerWnd::onPaint(Canvas *c) {
  COLORPICKERWND_PARENT::onPaint(c);
  RECT tr, r;
  getClientRect(&r); 
  c->fillRect(&r, 0xFFFFFF, 255);

  tr = r;

  r.right -= 32;
  r.left += 32;
  r.top += 32;
  r.bottom -= 32;

  tr.right -= 4;
  tr.left += 4;
  tr.top += 4;
  tr.bottom -= 4;

  int lx = r.right-r.left;
  int ly = r.bottom-r.top;
  int tlx = tr.right-tr.left;
  int tly = tr.bottom-tr.top;

  COLORREF *bits = (COLORREF *)c->getBits();
  int canvasw, canvash, canvasp;
  c->getDim(&canvasw, &canvash, &canvasp);
  canvasp /= 4;

  int hx = tlx/2;
  int hy = tly/2;
  int radius = MIN(hx, hy);

  for (int y=-radius;y<=radius;y++) {
    for (int x=-radius;x<=radius;x++) {
      // distance of point from center of wheel
      double l = SQRT(x*x + y*y);
      // if above outer radius, or below inner radius, skip
      if (l < radius - m_huewheel_width || l > radius) continue;
      // center the width of the color wheel around 0, and normalize it
      double a = ABS(l - (radius-(m_huewheel_width/2))) / (m_huewheel_width/2);
      // calculate an alpha value for the pixel based on the distance of the point from 0
      a = 1.0f-(1.0f/(1.0f+exp(-(a-0.80f)*50.0f)));
      // calculate the angle on the wheel and convert it to a hue wheel value
      double angle = atan2((y/(double)radius),(x/(double)radius)) / (M_PI*2);
      if (angle < 0) angle = 1+angle;
      // convert hue angle to rgb
      double _r, _g, _b;
      hslToRgb(angle, 1.0, 0.5, &_r, &_g, &_b);
      COLORREF c = RGB(_r*255, _g*255, _b*255) | (0xFF<<24);
      // blit the pixel
      COLORREF *b = bits+canvasp*(tr.top+hy+y)+tr.left+hx+x;
      *b = Blenders::BLEND_ADJ3(*b, c, a*255);
    }
  }

  int side = ((radius-(m_huewheel_width+2)) * sin(M_PI/4))*2;

  double d = 255/(double)side;
  int sx = (lx - side)/2;
  int sy = (ly - side)/2;

  RECT sr={r.left+sx-1, r.top+sy-1, r.left+sx+side+2, r.top+sy+side+2};
  c->drawRect(&sr, 1, 0x00000000);

  double yv = 0;

  int is = r.top+sy;
  int ie = is + side;
  int js = r.left+sx;
  int je = js + side;
  int ic = (side / 2) + is;
  int jc = (side / 2) + js;
  //float fside = (float)side;

  /*switch (m_slmode) {
    case 0: { */
      for (int i=is;i<=ie;i++) {
        double xv = 0;
        for (int j=js;j<=je;j++) {
          COLORREF *bit = bits+canvasp*i+j;
          double r, g, b;
          hslToRgb(m_control_hue, xv/255, 1-(yv/255), &r, &g, &b);
          COLORREF c = RGB(r*255, g*255, b*255) | (0xFF<<24);
          *bit = c;
          xv += d;
        }
        yv += d;
      }
  /*    break;
    }
    case 1: {
      for (signed int i=is;i<=ie;i++) {
        int idc = ABS(ic-i);
        for (int j=js;j<=je;j++) {
          if ((j-js) + idc*2 > side) continue;
          COLORREF *bit = bits+canvasp*i+j;
          double r, g, b;
          int idm = (side-(j-js))/2;
          float sbase = (j-js)/fside;
          float sv = sbase + (1-sbase)*((float)idc/(float)idm);
          // dammit this isn't right, sv to 1.0 isn't supposed to be linear, argg
          // i don't know what function i'm supposed to use, fuck it :(
          hslToRgb(m_control_hue, sv, 1-(yv/255), &r, &g, &b);
          COLORREF c = RGB(r*255, g*255, b*255) | (0xFF<<24);
          *bit = c;
        }
        yv += d;
      }
      break;
    }
  };*/

  if (m_cursorbmp) {
    int crossw = m_cursorbmp->getWidth();
    int crossh = m_cursorbmp->getHeight();

    if (m_control_hue != -1) {
      double angle;
      if (m_control_hue > 0.5)
        angle = -(1-m_control_hue);
      else
        angle = m_control_hue;
      angle *= 2*M_PI;
  
      int huecrossx = cos(angle) * (radius-(m_huewheel_width/2)) + tr.left+hx;
      int huecrossy = sin(angle) * (radius-(m_huewheel_width/2)) + tr.top+hy;
      huecrossx -= crossw/2;
      huecrossy -= crossh/2;
      m_cursorbmp->blitAlpha(c, huecrossx, huecrossy);
    }
    int colorx = js + m_control_sat*side;
    int colory = ie - m_control_lum*side;
    colorx -= crossw/2;
    colory -= crossh/2;
    m_cursorbmp->blitAlpha(c, colorx, colory);
  }

  m_last_hue_radius = radius;
  m_last_rectanglew = side;
  m_last_rectanglex = js;
  m_last_rectangley = is;

  return 1;
}

int ColorPickerWnd::onLeftButtonUp(int x, int y) {
  COLORPICKERWND_PARENT::onLeftButtonUp(x, y);
  m_click_circle = 0;
  m_click_rectangle = 0;
  endCapture();
  return 1;
}

int ColorPickerWnd::onLeftButtonDown(int x, int y) {
  COLORPICKERWND_PARENT::onLeftButtonDown(x, y);

  RECT r;
  getClientRect(&r);

  int w = r.right-r.left;
  int h = r.bottom-r.top;

  int cx = w/2 + r.left;
  int cy = h/2 + r.top;

  int dx = x - cx;
  int dy = y - cy;

  int l = SQRT(dx*dx + dy*dy);

  if (l > m_last_hue_radius - m_huewheel_width+1) {
    if (l < m_last_hue_radius+2) {
      m_click_circle = 1;
      m_click_rectangle = 0;
      updateCircle(x, y);
      onColorChange();
    }
  } else {
    if (x >= m_last_rectanglex && x < m_last_rectanglex+m_last_rectanglew
     && y >= m_last_rectangley && y < m_last_rectangley+m_last_rectanglew) {
      m_click_rectangle = 1;
      m_click_circle = 0;
      updateRectangle(x, y);
      onColorChange();
    }
  }

  beginCapture();

  return 1;
}


int ColorPickerWnd::onMouseMove(int x, int y) {
  COLORPICKERWND_PARENT::onMouseMove(x, y);
  if (m_click_circle) {
    updateCircle(x, y);
    onColorChange();
  } else if (m_click_rectangle) {
    updateRectangle(x, y);
    onColorChange();
  }
  return 1;
}

int ColorPickerWnd::onResize() {
  COLORPICKERWND_PARENT::onResize();
  invalidate();
  return 1;
}

void ColorPickerWnd::updateCircle(int x, int y) {
  RECT r;
  getClientRect(&r);

  int w = r.right-r.left;
  int h = r.bottom-r.top;

  int cx = w/2 + r.left;
  int cy = h/2 + r.top;

  int dx = x - cx;
  int dy = y - cy;

  double angle = atan2((dy/(double)m_last_hue_radius),(dx/(double)m_last_hue_radius)) / (M_PI*2);
  if (angle < 0) angle = 1+angle;

  m_control_hue = angle;

  invalidate();
  StdWnd::update(gethWnd());
}

void ColorPickerWnd::updateRectangle(int x, int y) {
  int px, py;
  px = x - m_last_rectanglex;
  py = y - m_last_rectangley;
  px = MIN(px, m_last_rectanglew);
  py = MIN(py, m_last_rectanglew);
  int cy = m_last_rectangley + m_last_rectanglew/2;

  double sat = px / (double)m_last_rectanglew;
  double lum = py / (double)m_last_rectanglew;

  sat = MAX(0.0, sat);
  lum = MAX(0.0, lum);
  sat = MIN(1.0, sat);
  lum = MIN(1.0, lum);

  lum = 1-lum;

  m_control_lum = lum;
  m_control_sat = sat;

  invalidate();
  StdWnd::update(gethWnd());
}

void ColorPickerWnd::registerCallback(ColorPickerWndCB *callback) {
  if (m_callbacks.haveItem(callback)) return;
  m_callbacks.addItem(callback);
}

void ColorPickerWnd::unregisterCallback(ColorPickerWndCB *callback) {
  m_callbacks.removeItem(callback);
}

void ColorPickerWnd::onColorChange(ColorPickerWndCB *setby) {
  foreach(m_callbacks)
    ColorPickerWndCB *cb = m_callbacks.getfor();
    if (cb != setby) cb->onColorPickerColorChanged(this);
  endfor;
};

void ColorPickerWnd::setColor(ARGB32 rgb, ColorPickerWndCB *setby) {
  double r, g, b;
  b = (rgb >> 16) & 0xFF;
  g = (rgb >> 8) & 0xFF;
  r =  rgb & 0xFF;
  r /= 255;
  g /= 255;
  b /= 255;
  rgbToHsl(r, g, b, &m_control_hue, &m_control_sat, &m_control_lum);
  invalidate();
  onColorChange(setby);
}

void ColorPickerWnd::setColor(double hue, double sat, double lum, ColorPickerWndCB *setby/* =NULL */) {
  m_control_hue = hue;
  m_control_sat = sat;
  m_control_lum = lum;
  invalidate();
  onColorChange(setby);
}

void ColorPickerWnd::getColor(double *hue, double *sat, double *lum) {
  if (hue) *hue = m_control_hue;
  if (sat) *sat = m_control_sat;
  if (lum) *lum = m_control_lum;
}

ARGB32 ColorPickerWnd::getColor() {
  double r, g, b;
  hslToRgb(m_control_hue, m_control_sat, m_control_lum, &r, &g, &b);
  return RGB(r*255, g*255, b*255);
}
