#ifndef _BLTCANVAS_H
#define _BLTCANVAS_H

#include "canvas.h"

class Bitmap;

#define BLTCANVAS_PARENT Canvas
class BltCanvas : public BLTCANVAS_PARENT {
public:
  BltCanvas(int w, int h, int nb_bpp=32, unsigned char *pal=NULL,int palsize=0);
  BltCanvas(HBITMAP bmp);
  virtual ~BltCanvas();
  void *getBits();
  HBITMAP getBitmap();

  Bitmap *makeBitmap(); // this one makes a new, with own bits
  void disposeBitmap(Bitmap *b); // call only after makeBitmap

  void fillBits(COLORREF color, RECT *r=NULL);

  void vflip(int vert_cells=1);
  void hflip(int hor_cells=1);
  void maskColor(COLORREF from, COLORREF to);
  void makeAlpha(int newalpha=-1); // -1 = premultiply using current alpha
  void premultiplyRect(RECT *r);

private:	// NONPORTABLE
  HBITMAP hbmp;
#ifdef WIN32
  HBITMAP prevbmp;
#endif
  PtrList<Bitmap> *skinbmps;
  BITMAP bm;
  BOOL ourbmp;
  int bpp;
};

#endif
