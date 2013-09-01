#include "precomp.h"

#include <draw/bitmap.h>

#include "bltcanvas.h"

BltCanvas::~BltCanvas() {
  if (hdc == NULL) return;

  // kill the bitmap and its DC
#if defined(WIN32)
  SelectObject(hdc, prevbmp);
  DeleteDC(hdc);
  hdc = NULL;
  if (ourbmp) {
    //GdiFlush();
    DeleteObject(hbmp);
  }
#elif defined(XWINDOW)
  if (ourbmp) {
    XFreePixmap(XWindow::getDisplay(), hdc->d);
    WASABI_API_LINUX->linux_destroyXShm( hbmp.shmseginfo );
  }
  XFreeGC( XWindow::getDisplay(), hdc->gc );
  if ( hdc->clip )
    XDestroyRegion( hdc->clip );
  FREE( hdc );
  hdc = NULL;
#else
#error port me!
#endif
  if (skinbmps) {
    if (skinbmps->getNumItems() > 0) {
      DebugString("disposeBitmap not called for makeBitmap, deleting the clone\n");
      skinbmps->deleteAll();
    }
    delete skinbmps;
  }
}

BltCanvas::BltCanvas(HBITMAP bmp) {
#ifdef XWINDOW
  MEMSET( &hbmp, 0, sizeof( hbmp ) );
  //CUT MEMSET( &prevbmp, 0, sizeof( prevbmp ) );
#else  
  prevbmp = NULL;
#endif
  bits = NULL;
  fcoord = TRUE;
  ourbmp = FALSE;
  skinbmps=NULL;
  
  hbmp = bmp;
#ifndef WASABI_PLATFORM_XWINDOW // HBITMAP is a struct for us
  ASSERT(hbmp != NULL);
#endif

  // create tha DC
#if defined(WIN32)
  hdc = CreateCompatibleDC(NULL);
  prevbmp = (HBITMAP)SelectObject(hdc, hbmp);
#elif defined(XWINDOW)
  hdc = (HDC)MALLOC( sizeof( hdc_typ ) );
  hdc->d = hbmp.p;
  hdc->gc = XCreateGC( XWindow::getDisplay(), hdc->d, 0, NULL );
#else
#error port me!
#endif
}

BltCanvas::BltCanvas(int w, int h, int nb_bpp, unsigned char *pal, int palsize){
#ifdef XWINDOW
  MEMSET( &hbmp, 0, sizeof( hbmp ) );
  //CUT MEMSET( &prevbmp, 0, sizeof( prevbmp ) );
#else
  hbmp = NULL;
  prevbmp = NULL;
#endif
  bits = NULL;
  fcoord = TRUE;
  ourbmp = TRUE;
  bpp = nb_bpp;
  skinbmps=NULL;

  ASSERT(w != 0 && h != 0);
  if (w==0) w=1;
  if (h==0) h=1;

#if defined(WIN32)
  struct {
    BITMAPINFO bmi;
    RGBQUAD more_bm7iColors[256];
  } bitmap;
  
//  BITMAPINFO bmi;
  MEMZERO(&bitmap, sizeof(bitmap));
  if(pal) {
    for (int c = 0; c < palsize/(3); c ++) {
      bitmap.bmi.bmiColors[c].rgbRed = pal[c*3];
      bitmap.bmi.bmiColors[c].rgbGreen = pal[c*3+1];
      bitmap.bmi.bmiColors[c].rgbBlue = pal[c*3+2];
      bitmap.bmi.bmiColors[c].rgbReserved = 0;
    }
    bitmap.bmi.bmiHeader.biClrUsed = palsize/(3);
    bitmap.bmi.bmiHeader.biClrImportant = palsize/(3);
  } else {
    bitmap.bmi.bmiHeader.biClrUsed = 0;
    bitmap.bmi.bmiHeader.biClrImportant = 0;
  }
  bitmap.bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bitmap.bmi.bmiHeader.biWidth = ABS(w);
  bitmap.bmi.bmiHeader.biHeight = -ABS(h);
  bitmap.bmi.bmiHeader.biPlanes = 1;
  bitmap.bmi.bmiHeader.biBitCount = nb_bpp;
  bitmap.bmi.bmiHeader.biCompression = BI_RGB;
  bitmap.bmi.bmiHeader.biSizeImage = 0;
  bitmap.bmi.bmiHeader.biXPelsPerMeter = 0;
  bitmap.bmi.bmiHeader.biYPelsPerMeter = 0;
  //GdiFlush();
  hbmp = CreateDIBSection(NULL, &bitmap.bmi, DIB_RGB_COLORS, &bits, NULL, 0);
  if (hbmp == NULL) {
    return;
  }
  GetObject(hbmp, sizeof(BITMAP), &bm);
  width=bm.bmWidth;
  height=ABS(bm.bmHeight);
  pitch=bm.bmWidthBytes;

  // create tha DC
  hdc = CreateCompatibleDC(NULL);
  if (!hdc) __asm int 3;
  prevbmp = (HBITMAP)SelectObject(hdc, hbmp);
#elif defined(XWINDOW)
  ASSERT( nb_bpp == 32 );
  hbmp.bmWidth = ABS(w);
  hbmp.bmHeight = ABS(h);
  hbmp.shmseginfo = WASABI_API_LINUX->linux_createXShm( hbmp.bmWidth * hbmp.bmHeight * 4 );
  ASSERT(hbmp.shmseginfo != NULL);
  bits = hbmp.shmseginfo->shmaddr;
  hbmp.p = XShmCreatePixmap( XWindow::getDisplay(), XWindow::RootWin(),
                             (char *)bits, hbmp.shmseginfo,
                             hbmp.bmWidth, hbmp.bmHeight, 24 );

  hdc = (HDC)MALLOC( sizeof( hdc_typ ) );
  hdc->d = hbmp.p;
  hdc->gc = XCreateGC( XWindow::getDisplay(), hdc->d, 0, NULL );

  bm = hbmp;

  width=bm.bmWidth;
  height=ABS(bm.bmHeight);
  pitch=width*nb_bpp/8;
#else
#error port me!
#endif
}

void *BltCanvas::getBits() {
  return bits;
}

HBITMAP BltCanvas::getBitmap() {
  return hbmp;
}

Bitmap *BltCanvas::makeBitmap() { 
  // make a clone of the bitmap 
  Bitmap *clone = new Bitmap(getBitmap(), getHDC(), 1);

  if (!skinbmps) skinbmps = new PtrList<Bitmap>;
  skinbmps->addItem(clone);

  return clone;
}

void BltCanvas::disposeBitmap(Bitmap *b) {
  if (skinbmps != NULL && skinbmps->haveItem(b)) {
    skinbmps->removeItem(b);
    delete b;
  } else {
    DebugString("disposeBitmap called on unknown pointer, you should call it from the object used to makeBitmap()\n");
  }
}

void BltCanvas::fillBits(COLORREF color, RECT *r) {
  if (bpp == 32) {	// clear out the bits
    if (r != NULL) {
      RECT sr = *r;
      sr.left = MAX<int>(0, sr.left);
      sr.top = MAX<int>(0, sr.top);
      sr.right = MAX<int>(sr.left, sr.right);
      sr.bottom = MAX<int>(sr.top, sr.bottom);
      int w, h, pitch;
      getDim(&w, &h, &pitch);
      pitch >>= 2;
      int _w = MIN(sr.right-sr.left, w-sr.left);
      int _h = MIN(sr.bottom-sr.top, h-sr.top);
      if (_w < 0 || _h < 0) return;
      ARGB32 *bits = (ARGB32*)getBits()+sr.top*pitch+sr.left;
      while (_h--) {
        MEMFILL<DWORD>(bits, color, _w);
        bits+=pitch;
      }
    } else {
      DWORD *dwbits = (DWORD *)bits;
      MEMFILL<DWORD>(dwbits, color, width * height);
    }
  }
}

void BltCanvas::premultiplyRect(RECT *r) {
  int w, h, pitch;
  getDim(&w, &h, &pitch);
  pitch >>= 2;
  int _w = MIN(r->right-r->left, w-r->left);
  int _h = MIN(r->bottom-r->top, h-r->top);
  if (_w < 0 || _h < 0) return;
  ARGB32 *bits = (ARGB32*)getBits()+r->top*pitch+r->left;
  while (_h--) {
    premultiply(bits, _w);
    bits+=pitch;
  }
}

void BltCanvas::vflip(int vert_cells) {
  ASSERT(bits != NULL);
//  BITMAP bm;
//  int r = GetObject(hbmp, sizeof(BITMAP), &bm);
//  if (r == 0) return;
  int w = bm.bmWidth, h = bm.bmHeight;
  int bytes = 4 * w;
  char *tmpbuf = (char *)MALLOC_NOINIT(bytes);
  int cell_h = h/vert_cells;
  for (int j = 0; j < vert_cells; j++) 
   for (int i = 0; i < cell_h/2; i++) {
    char *p1, *p2;
    p1 = (char *)bits + bytes * i + (j * cell_h*bytes);
    p2 = (char *)bits + bytes * ((cell_h - 1) - i) + (j * cell_h*bytes);
    if (p1 == p2) continue;
    MEMCPY(tmpbuf, p1, bytes);
    MEMCPY(p1, p2, bytes);
    MEMCPY(p2, tmpbuf, bytes);
  }
  FREE(tmpbuf);
}

void BltCanvas::hflip(int hor_cells) {
  ASSERT(bits != NULL);
  // todo: optimize
  int w = bm.bmWidth, h = bm.bmHeight;
  for (int i=0;i<hor_cells;i++)
    for (int x=0;x<w/2/hor_cells;x++)
      for (int y=0;y<h;y++)  {
        int *p = ((int *)bits)+x+y*w+(i*w/hor_cells);
        int *d = ((int *)bits)+((w/hor_cells)-x)+y*w+(i*w/hor_cells)-1;
        int t = *p;
        *p = *d;
        *d = t;
      }
}

void BltCanvas::maskColor(COLORREF from, COLORREF to) {
  int n = bm.bmWidth * bm.bmHeight;
  //GdiFlush();
  DWORD *b = (DWORD *)getBits();
  from &= 0xffffff;
  while (n--) {
    if ((*b & 0xffffff) == from) {
      *b = to;
    } else *b |= 0xff000000;	// force all other pixels non masked
    b++;
  }
}

void BltCanvas::makeAlpha(int newalpha) {
  int w, h;
  getDim(&w, &h, NULL);
  premultiply((ARGB32 *)getBits(), w*h, newalpha);
}

