//NONPORTABLE
#ifndef _BITMAP_H
#define _BITMAP_H

#include <wasabicfg.h>
#include <bfc/string/string.h>
#include <img/api_imgldr.h>
#include <wnd/bltcanvas.h>
#include <draw/blending.h>
#include <bfc/std.h>
#include <wnd/canvas.h>

class CanvasBase;	// see canvas.h

// #define NO_MMX // NO! do not define this here!

class Region;

// a skinnable bitmap
class Bitmap {
public:
  Bitmap();
  Bitmap(const Bitmap *b);
  Bitmap(const Bitmap &b);
#ifndef _NOSTUDIO
#ifdef WASABI_COMPILE_IMGLDR
  Bitmap(HINSTANCE hInst, int _id, const char *colorgroup=NULL);	//NONPORTABLE
#endif
  Bitmap(const char *elementname, int cached=1);
#endif
  Bitmap(int w, int h, DWORD bgcolor=0xffff00ff, int noinit=0);// creates solid fill image
  //Bitmap(ARGB32 *copybits, int w, int h);	 // copies ARGB32 bits
  Bitmap(void *data, int data_len);		// decodes data through imgldr

  Bitmap(HBITMAP bitmap);
  Bitmap(HBITMAP bitmap, HDC dc, int has_alpha=0, void *bits=NULL);
  Bitmap(ARGB32 *copybits, int bits_w, int bits_h, int subimage_offset_x=0, int subimage_offset_y=0, int subimage_w=-1, int subimage_h=-1);
  virtual ~Bitmap();

  Bitmap &operator=(const Bitmap &b);

  int getWidth() const { return subimage_w == -1 ? fullimage_w : subimage_w; };
  int getHeight() const { return subimage_h == -1 ? fullimage_h : subimage_h; };
  int getFullWidth() const { return fullimage_w; };
  int getFullHeight() const { return fullimage_h; };
  int getX() const { return x_offset == -1 ? 0 : x_offset; };
  int getY() const { return y_offset == -1 ? 0 : y_offset; };
  int getBpp() const { return 32; };
  int getAlpha() const { return has_alpha; };
  void setHasAlpha(int ha);
  virtual ARGB32 *getBits() const;
  int isInvalid();
  inline int valid() { return !isInvalid(); }
  
  const char *getBitmapName();

  int setPixel(int x, int y, ARGB32 color);	//clipped
  void setAllPixels(ARGB32 color);

  void blit(CanvasBase *canvas, int x, int y);
  void blitAlpha(CanvasBase *canvas, int x, int y, int alpha=255, Blender *blender=NULL);
  // blits a chunk of source into dest rect
  void blitToRect(CanvasBase *canvas, RECT *src, RECT *dst, int alpha=255, Blender *blender=NULL);
  void blitTile(CanvasBase *canvas, RECT *dest, int xoffs=0, int yoffs=0, int alpha=255);
  void blitRectToTile(CanvasBase *canvas, RECT *dest, RECT *src, int xoffs=0, int yoffs=0, int alpha=255);
  void stretch(CanvasBase *canvas, int x, int y, int w, int h, Blender *blender=NULL);
  void stretchToRect(CanvasBase *canvas, RECT *r, Blender *blender=NULL);
  void stretchRectToRect(CanvasBase *canvas, RECT *src, RECT *dst, Blender *blender=NULL);
  void stretchToRectAlpha(CanvasBase *canvas, RECT *r, int alpha=255, Blender *blender=NULL);
  void stretchToRectAlpha(CanvasBase *canvas, RECT *src, RECT *dst, int alpha=255, Blender *blender=NULL);
  // rotate and blit, safe zone is a centered circle whose radius is MIN(bmpwidth, bmpheight)
  void blitRotated(CanvasBase *canvas, float x, float y, int w, int h, float angle_in_radians, float scale=-1, int alpha=255, Blender *blender=NULL, int subsample=1, float centerxoffsetsrc=0, float centeryoffsetsrc=0, float centerxoffsetdst=0, float centeryoffsetdst=0, int wrapx=0, int wrapy=0);
  // This version is a helper to the one above, it extends the bitmap so that all of the original pixels 
  // lie in the safe zone then blits to the destination canvas at coordinates such that x,y lie at the 
  // center of the bitmap. It also fills an optional rect with the resulting blit rect (trunc(left/top), ceil(right/bottom))
  void blitRotatedNonCroppedCentered(CanvasBase *canvas, float x, float y, float angle_in_radians, float scale=-1, RECT *blitrect=NULL, int alpha=255, Blender *blender=NULL, int subsample=1);

  COLORREF getPixel(int x, int y);
  void getBoundingBox(RECT *r);
  int checkAlphaConsistancy();
  void bgra2argb();

protected:
  int has_alpha;

  int x_offset,y_offset,subimage_w,subimage_h,fullimage_w,fullimage_h;

  ARGB32 *bits;
  int last_failed;
  String bitmapname;
  int invalid;
  int allocmethod;

  enum {
    ALLOC_NONE              = 0, // bits is NULL, or should NOT be deallocated because it points to someone else' bits
    ALLOC_LOCAL             = 1, // alloc'ed with MALLOC, should be freed with FREE
    ALLOC_MEMMGRAPI         = 2, // alloc'ed with image loader service, should be freed with mem manager api's sysFree
    ALLOC_IMGLDRAPI         = 3, // alloc'ed with image loader api without element management or caching, should be freed with img loader api releaseBmp
    ALLOC_IMGLDRAPI_MANAGED = 4, // alloc'ed with image loader api with element management and caching, should be freed with img loader api releaseBitmap
  };

protected:

  void init();// call from each constructor
  void copyFromBitmap(const Bitmap *b);
  void bmpToBits(HBITMAP hbmp, HDC defaultDC=NULL);//BU> doesn't seem to work?
  void makeInvalid();
  void freeBitmap();
};

// This class makes a bitmap whose bits are NOT copied, the bitmap simply uses the provided piece of memory
// as its content, it's up to you to do the appropriate reference counting if needed
class WrapperBitmap : public Bitmap {
public:
  WrapperBitmap(ARGB32 *_bits, int bits_w, int bits_h, int subimage_offset_x=0, int subimage_offset_y=0, int subimage_w=-1, int subimage_h=-1);
  WrapperBitmap(BltCanvas *canvas);
};


#endif
