#ifndef COLORPICKERWND_H
#define COLORPICKERWND_H

#include <wnd/virtualwnd.h>

class Bitmap;
class ColorPickerWndCB;

#define COLORPICKERWND_PARENT VirtualWnd

class ColorPickerWnd : public COLORPICKERWND_PARENT {
public:
  ColorPickerWnd();
  virtual ~ColorPickerWnd();

  void setHueWheelWidth(double width);
  void setCursorBmp(Bitmap *bmp, int autodelete=1);
  void registerCallback(ColorPickerWndCB *callback);
  void unregisterCallback(ColorPickerWndCB *callback);

  void setColor(double hue, double sat, double lum, ColorPickerWndCB *setby=NULL);
  void setColor(ARGB32 rgb, ColorPickerWndCB *setby=NULL);
  ARGB32 getColor();
  void getColor(double *hue, double *sat, double *lum);
  //void setSLMode(int mode); // 0 for square, 1 for triangle

  virtual int onPaint(Canvas *c);
  virtual int onMouseMove(int x, int y);
  virtual int onResize();
  virtual int onLeftButtonDown(int x, int y);
  virtual int onLeftButtonUp(int x, int y);

  virtual void onColorChange(ColorPickerWndCB *setby=NULL);

  // feel free to make that use draw/ahsv.h if you want :P
  void hslToRgb(double h, double s, double l, double *r, double *g, double *b);
  void rgbToHsl(double r, double g, double b, double *hue, double *saturation, double *luminosity);

private:
  void updateRectangle(int x, int y);
  void updateCircle(int x, int y);

  double m_control_hue;
  double m_control_lum;
  double m_control_sat;
  double m_huewheel_width;
  Bitmap *m_cursorbmp;
  int m_delete_cursor;

  double m_last_hue_radius;
  int m_last_rectanglew;
  int m_last_rectanglex;
  int m_last_rectangley;
  int m_click_circle;
  int m_click_rectangle;
  //int m_slmode;
  PtrList<ColorPickerWndCB> m_callbacks;
};

class ColorPickerWndCB {
public:
  virtual void onColorPickerColorChanged(ColorPickerWnd *picker) {};
};

#endif
