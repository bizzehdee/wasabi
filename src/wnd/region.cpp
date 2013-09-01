#include "precomp.h"

//CUT #ifdef WIN32

//CUT #include <wasabicfg.h>
//CUT? #include <img/api_imgldr.h>
#include <bfc/std.h>
#include <bfc/util/profiler.h>
#include <draw/bitmap.h>

#include "basewnd.h"
#include "canvas.h"

#include "region.h"

//CUT #ifndef _NOSTUDIO
//CUT #include "../../../studio/api.h"
//CUT #endif

#define GETOSHANDLE(x) (const_cast<Region *>(x)->getOSHandle())

#define CBCLASS RegionI
START_DISPATCH;
  CB(REGION_GETOSHANDLE, getOSHandle);
  CB(REGION_CLONE, clone);
  VCB(REGION_DISPOSECLONE, disposeClone);
  CB(REGION_PTINREGION, ptInRegion);
  VCB(REGION_OFFSET, offset);
  VCB(REGION_GETBOX, getBox);
  VCB(REGION_SUBTRACTRGN, subtractRegion);
  VCB(REGION_SUBTRACTRECT, subtractRect);
  VCB(REGION_ADDRECT, addRect);
  VCB(REGION_ADD, addRegion);
  VCB(REGION_AND, andRegion);
  VCB(REGION_SETRECT, setRect);
  VCB(REGION_EMPTY, empty);
  CB(REGION_ISEMPTY, isEmpty);
  CB(REGION_EQUALS, equals);
  CB(REGION_ENCLOSED, enclosed);
  CB(REGION_INTERSECTRECT, intersectRect);
  CB(REGION_INTERSECTRGN, intersectRgn);
  CB(REGION_ISRECT, isRect);
  VCB(REGION_SCALE, scale);
  VCB(REGION_DEBUG, debug);
  CB(REGION_MAKEWNDREGION, makeWindowRegion);
  CB(REGION_GETNUMRECTS, getNumRects);
  CB(REGION_ENUMRECT, enumRect);
END_DISPATCH;
#undef CBCLASS

RegionI::RegionI() {
#if defined(WIN32)
  hrgn = CreateRectRgn(0,0,0,0);
#elif defined(XWINDOW)
  hrgn = XCreateRegion();
#else
#error port me!
#endif // platform
  init();
}

RegionI::RegionI(const RECT *r) {
#if defined(WIN32)
  hrgn = CreateRectRgn(r->left,r->top,r->right,r->bottom);
  if (!hrgn) hrgn = CreateRectRgn(0,0,0,0);
#elif defined(XWINDOW)
  hrgn = XCreateRegion();
# if defined(WASABI_WIN32RECT)
  XRectangle xr = { r->left, r->top, r->right - r->left, r->bottom - r->top };
  XUnionRectWithRegion( &xr, hrgn, hrgn );
# else
  XUnionRectWithRegion( &r, hrgn, hrgn );
#endif // RECT
#else
#error port me!
#endif // platform
  init();
  optimize();
}

RegionI::RegionI(int l, int t, int r, int b) {
#if defined(WIN32)
  hrgn = CreateRectRgn(l,t,r,b);
  if (!hrgn) hrgn = CreateRectRgn(0,0,0,0);
#elif defined(XWINDOW)
  hrgn = XCreateRegion();
# if defined(WASABI_WIN32RECT)
  XRectangle xr = { l, t, r - l, b - t };
  XUnionRectWithRegion( &xr, hrgn, hrgn );
# else
  XUnionRectWithRegion( &r, hrgn, hrgn );
#endif // RECT
#else
#error port me!
#endif // platform
  init();
  optimize();
}

RegionI::RegionI(OSREGIONHANDLE r) {
#if defined(WIN32)
  OSREGIONHANDLE R = CreateRectRgn(0,0,0,0);
  CombineRgn(R, r, r, RGN_COPY);
  hrgn = R;
#elif defined(XWINDOW)
  hrgn = XCreateRegion();
  XUnionRegion( hrgn, r, hrgn );
#else
#error port me!
#endif // platform
  init();
  optimize();
}

RegionI::RegionI(Canvas *c, RECT *defbounds) {
#if defined(WIN32)
  hrgn = CreateRectRgn(0,0,0,0);
  if (!GetClipRgn(c->getHDC(), hrgn)) {
    if (defbounds != NULL)
      SetRectRgn(hrgn, defbounds->left, defbounds->top, defbounds->right, defbounds->bottom);
  }
#elif defined(XWINDOW)
  HDC dc = c->getHDC();
  hrgn = XCreateRegion();

  if ( dc->clip ) {
    XUnionRegion( hrgn, dc->clip, hrgn );
  } else {
    int w, h;
    c->getDim( &w, &h, NULL );
    XRectangle xr = { 0, 0, w, h };
    XUnionRectWithRegion( &xr, hrgn, hrgn );
  }
#else
#error port me!
#endif // platform
  init();
  optimize();
}

RegionI::~RegionI() {
  delete lastdebug;
  if (srv != NULL) srv->delRef(this);
  ASSERT(clonecount==0);
  if (srv == NULL && hrgn != NULL)
#if defined(WIN32)
    DeleteObject(hrgn);
#elif defined(XWINDOW)
    XDestroyRegion(hrgn);
#else
#error port me!
#endif // platform
} 

void RegionI::init() {
  srv = NULL;
  clonecount = 0;
  lastdebug = NULL;
  optimized = 0;
}

Region *RegionI::clone() {
  Region *newregion = new RegionI(getOSHandle());
  clonecount++;
  return newregion;
}

void RegionI::disposeClone(Region *r) {
  RegionI *ri = static_cast<RegionI *>(r);
  delete ri; // todo: validate pointer before deleting
  clonecount--;
}

// returns a handle that SetWindowRgn understands (non portable). We should NOT delete this handle, windows will delete 
// it by itself upon setting a new region of destroying the window
OSREGIONHANDLE RegionI::makeWindowRegion() {
  deoptimize();
#if defined(WIN32)
  OSREGIONHANDLE R = CreateRectRgn(0, 0, 0, 0);
  CombineRgn(R, hrgn, hrgn, RGN_COPY);
#elif defined(XWINDOW)
  OSREGIONHANDLE R = XCreateRegion();
  XUnionRegion( R, hrgn, R  );
#else
#error port me!
#endif // platform
  optimize();
  return R;
}

RegionI::RegionI(Bitmap *bitmap, RECT *r, int xoffset, int yoffset, BOOL inverted, int dothreshold, char threshold, int thinverse, int minalpha) {
  init();

//  PR_ENTER("alphatoregionrect");

#ifndef _NOSTUDIO
#ifdef WASABI_COMPILE_IMGLDR
#ifdef WASABI_COMPILE_SKIN

  const char *id = bitmap->getBitmapName();

  if (xoffset == 0 && yoffset == 0 && r == NULL && !inverted && !dothreshold && minalpha == 1 && id != NULL && *id != 0) {
    srv = WASABI_API_IMGLDR->imgldr_requestSkinRegion(id);
    if (srv != NULL) {
      srv->addRef(this);
      hrgn = srv->getRegion()->getOSHandle();
    }
  }

  if (srv == NULL) {

#endif
#endif
#endif

    if (r)
      hrgn = alphaToRegionRect(bitmap, xoffset, yoffset, TRUE, r->left, r->top, r->right-r->left, r->bottom-r->top, inverted, dothreshold, threshold, thinverse, minalpha);
    else
      hrgn = alphaToRegionRect(bitmap, xoffset, yoffset, FALSE, 0, 0, 0, 0, inverted, dothreshold, threshold, thinverse, minalpha);

#ifndef _NOSTUDIO
#ifdef WASABI_COMPILE_SKIN
#ifdef WASABI_COMPILE_IMGLDR

    if (id != NULL && *id != 0) {
      if (xoffset == 0 && yoffset == 0 && r == NULL && !inverted && !dothreshold && minalpha == 1) {
        WASABI_API_IMGLDR->imgldr_cacheSkinRegion(id, this);
        srv = WASABI_API_IMGLDR->imgldr_requestSkinRegion(id);
        if (srv != NULL) {
          srv->addRef(this);
          DeleteObject(hrgn);
          hrgn = srv->getRegion()->getOSHandle();
        }
      }
    } 
  }
#endif
#endif
#endif

  optimize();

//  PR_LEAVE();
}

OSREGIONHANDLE RegionI::alphaToRegionRect(Bitmap *bitmap, int xoffset, int yoffset, BOOL portion, int _x, int _y, int _w, int _h, BOOL inverted, int dothreshold, unsigned char threshold, int thinverse, int minalpha) {
  return alphaToRegionRect(bitmap->getBits(), bitmap->getX(), bitmap->getY(),
    bitmap->getWidth(), bitmap->getHeight(), bitmap->getFullWidth(),
    bitmap->getFullHeight(), xoffset, yoffset, portion, _x, _y, _w, _h,
    inverted, dothreshold, threshold, thinverse, minalpha);
}

OSREGIONHANDLE RegionI::alphaToRegionRect(void *pbits32, int bmX, int bmY, int bmWidth, int bmHeight, int fullw, int fullh, int xoffset, int yoffset, BOOL portion, int _x, int _y, int _w, int _h, BOOL inverted, int dothreshold, unsigned char threshold, int thinverse, int minalpha) {
  if (!pbits32) return NULL;
#ifdef WIN32
  OSREGIONHANDLE ret = NULL;
  RGNDATA *pData;
#endif
#ifdef XWINDOW
  OSREGIONHANDLE ret = XCreateRegion();
  XRectangle xr;
#endif
  int y, x;

#ifdef WIN32
  // For better performances, we will use the ExtCreateRegion() function to create the
  // region. This function take a RGNDATA structure on entry. We will add rectangles by
  // amount of ALLOC_UNIT number in this structure.
  // JF> rects are 8 bytes, so this allocates just under 16kb of memory, no need to REALLOC
#define MAXRECTS 2000
  pData = (RGNDATA *)MALLOC(sizeof(RGNDATAHEADER) + (sizeof(RECT) * MAXRECTS));
  if (!pData) return NULL;

  pData->rdh.dwSize = sizeof(RGNDATAHEADER);
  pData->rdh.iType = RDH_RECTANGLES;
  pData->rdh.nCount = pData->rdh.nRgnSize = 0;

  SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
#endif

  int x_end=(portion ? _w+_x : bmWidth);
  int y_end=(portion ? _h+_y : bmHeight);
  int x_start=(portion ? _x : 0);
  int y_start=(portion ? _y : 0);

  x_start += bmX;
  x_end += bmX;
  y_start += bmY;
  y_end += bmY;

  unsigned int iv=minalpha<<24;//inverted?0xff000000:0;

  int shiftx = xoffset - bmX;
  int shifty = yoffset - bmY;

  for (y = y_start; y < y_end; y++) {
    // Scan each bitmap pixel from left to right
    unsigned int *lineptr=((unsigned int *)pbits32) + fullw * y;
    for (x = x_start; x < x_end; x++) {
      // Search for a continuous range of "non transparent pixels"
      int x0 = x;
      unsigned int *p = lineptr;
      if (dothreshold) {
        if (inverted) {
          if (thinverse) {
            while (x < x_end) {
              unsigned int a=p[x];
              if ((a&0xff000000) >= iv || 
                  (((((a & 0xFF) > threshold || ((a & 0xFF00) >> 8) > threshold || ((a & 0xFF0000) >> 16) > threshold)))))
                break;
              x++;
            }
          } else {
            while (x < x_end) {
              unsigned int a=p[x];
              if ((a&0xff000000) >= iv || 
                  (((((a & 0xFF) < threshold || ((a & 0xFF00) >> 8) < threshold || ((a & 0xFF0000) >> 16) < threshold)))))
                break;
              x++;
	    }
          }
        } else {
          if (thinverse) {
            while (x < x_end) {
              unsigned int a=p[x];
              if ((a&0xff000000) < iv || 
                  (((((a & 0xFF) > threshold || ((a & 0xFF00) >> 8) > threshold || ((a & 0xFF0000) >> 16) > threshold)))))
                break;
              x++;
	    }
          } else {
	    while (x < x_end) {
              unsigned int a=p[x];
              if ((a&0xff000000) < iv || 
                  (((((a & 0xFF) < threshold || ((a & 0xFF00) >> 8) < threshold || ((a & 0xFF0000) >> 16) < threshold)))))
                break;
              x++;
            }
          }
        }
      } else {
        if (inverted) {
          while (x < x_end) {
            if ((p[x] & 0xFF000000) >= iv) break;
            x++;
          }
        } else {
          while (x < x_end) {
            if ((p[x] & 0xFF000000) < iv) break;
            x++;
          }
        }
      }

      if (x > x0) {
#ifdef WIN32
        SetRect(((RECT *)&pData->Buffer) + pData->rdh.nCount, x0 + shiftx, y + shifty, x + shiftx, y + 1 + shifty);

        pData->rdh.nCount++;

        if (x0 + shiftx < pData->rdh.rcBound.left) pData->rdh.rcBound.left = x0 + shiftx;
        if (y + shifty < pData->rdh.rcBound.top) pData->rdh.rcBound.top = y + shifty;
        if (x + shiftx > pData->rdh.rcBound.right) pData->rdh.rcBound.right = x + shiftx;
        if (y + 1 + shifty > pData->rdh.rcBound.bottom) pData->rdh.rcBound.bottom = y + 1 + shifty;

        // On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
        // large (ie: > 4000). Therefore, we have to create the region by multiple steps.
        if (pData->rdh.nCount == MAXRECTS) {
          OSREGIONHANDLE h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * pData->rdh.nCount), pData);
          if (ret) {
            CombineRgn(ret, ret, h, RGN_OR);
            DeleteObject(h);
          } else ret = h;
          pData->rdh.nCount = 0;
          SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
        }
#elif defined(XWINDOW)
        xr.x = x0 + shiftx; xr.y = y + shifty;
        xr.width = x - x0; xr.height = 1;
        
        XUnionRectWithRegion( &xr, ret, ret );
#endif
      }
    }
  }

#ifdef WIN32
  // Create or extend the region with the remaining rectangles
  OSREGIONHANDLE h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * pData->rdh.nCount), pData);
  if (ret) {
    CombineRgn(ret, ret, h, RGN_OR);
    DeleteObject(h);
  } else
    ret = h;

  // Clean up
  FREE(pData);
#endif

  return ret;
}

BOOL RegionI::ptInRegion(const POINT *pt) {
  if (optimized) return Std::pointInRect(&optrect, (POINT&)*pt); // cast needed for dropping 'const' 
  ASSERT(hrgn != NULL);
#if defined(WIN32)
  return PtInRegion(hrgn, pt->x, pt->y);
#elif defined(XWINDOW)
  return XPointInRegion( hrgn, pt->x, pt->y );
#else
#error port me!
#endif // platform
}

void RegionI::offset(int x, int y) {
  if (optimized) {
    optrect.left += x;
    optrect.top += y;
    optrect.right += x;
    optrect.bottom += y;
    return;
  }
  ASSERT(hrgn != NULL);
  if (srv) {
#if defined(WIN32)
    hrgn = CreateRectRgn(0,0,0,0);
#elif defined(XWINDOW)
    hrgn = XCreateRegion();
#else
#error port me!
#endif // platform
    RegionServer *s = srv;
    srv = NULL;
    addRegion(s->getRegion());
    s->delRef(this);
  }
  if (x == 0 && y == 0) return;
  deoptimize(); // because addregion may have optimized it
#if defined(WIN32)
  OffsetRgn(hrgn, x, y);
#elif defined(XWINDOW)
  XOffsetRegion(hrgn, x, y);
#else
#error port me!
#endif // platform
  optimize();
}

void RegionI::getBox(RECT *r) {
  if (optimized) {
    *r = optrect;
    return;
  }
  ASSERT(r != NULL);
#if defined(WIN32)
  GetRgnBox(hrgn, r);
#elif defined(XWINDOW)
#if defined(WASABI_WIN32RECT)
  XRectangle xr;
  XClipBox(hrgn, &xr);
  r->top = xr.y;
  r->left = xr.x;
  r->right = xr.x + xr.width;
  r->bottom = xr.y + xr.height;
#else
  XClipBox(hrgn, r);
#endif // RECT
#else
#error port me!
#endif // platform
}

OSREGIONHANDLE RegionI::getOSHandle() {
  deoptimize();
  ASSERT(hrgn != NULL);
  return hrgn;
}

void RegionI::subtractRect(const RECT *r) {
  subtractRegion(&RegionI(r));
}

void RegionI::subtractRegion(const Region *reg) {
  if (srv) {
#if defined(WIN32)
    hrgn = CreateRectRgn(0,0,0,0);
#elif defined(XWINDOW)
    hrgn = XCreateRegion();
#else
#error port me!
#endif // platform
    RegionServer *s = srv;
    srv = NULL;
    addRegion(s->getRegion());
    s->delRef(this);
  }
  deoptimize();
#if defined(WIN32)
  CombineRgn(hrgn, hrgn, GETOSHANDLE(reg), RGN_DIFF);
#elif defined(XWINDOW)
  XSubtractRegion( hrgn, GETOSHANDLE(reg), hrgn );
#else
#error port me!
#endif // platform
  optimize();
}

void RegionI::andRegion(const Region *reg) {
  if (srv) {
#if defined(WIN32)
    hrgn = CreateRectRgn(0,0,0,0);
#elif defined(XWINDOW)
    hrgn = XCreateRegion();
#else
#error port me!
#endif // platform
    RegionServer *s = srv;
    srv = NULL;
    addRegion(s->getRegion());
    s->delRef(this);
  }
  deoptimize();
#if defined(WIN32)
  CombineRgn(hrgn, hrgn, GETOSHANDLE(reg), RGN_AND);
#elif defined(XWINDOW)
  XIntersectRegion( hrgn, GETOSHANDLE(reg), hrgn );
#else
#error port me!
#endif // platform
  optimize();
}

void RegionI::addRect(const RECT *r) {
  addRegion(&RegionI(r));
}

void RegionI::addRegion(const Region *reg) {
  if (srv) {
#if defined(WIN32)
    hrgn = CreateRectRgn(0,0,0,0);
#elif defined(XWINDOW)
    hrgn = XCreateRegion();
#else
#error port me!
#endif // platform
    RegionServer *s = srv;
    srv = NULL;
    addRegion(s->getRegion());
    s->delRef(this);
  }
  deoptimize();
  ASSERT(hrgn != NULL);
  ASSERT(reg != NULL);
#if defined(WIN32)
  CombineRgn(hrgn, hrgn, GETOSHANDLE(reg), RGN_OR);
#elif defined(XWINDOW)
  XUnionRegion( hrgn, GETOSHANDLE(reg), hrgn );
#else
#error port me!
#endif // platform
  optimize();
}

int RegionI::isEmpty() {
#if defined(WIN32)  
  RECT r;
  getBox(&r);
  if (r.left == r.right || r.bottom == r.top) return 1;
  return 0;
#elif defined(XWINDOW)
  XEmptyRegion( hrgn );
#else
#error port me!
#endif // platform
}

int RegionI::enclosed(const Region *r, Region *outside) {
  deoptimize();
  int ret;
#if defined(WIN32)
  OSREGIONHANDLE del=NULL;
  if (!outside) 
    del = CreateRectRgn(0,0,0,0);
  int rs = CombineRgn(outside ? outside->getOSHandle() : del, hrgn, GETOSHANDLE(r), RGN_DIFF);
  if (del != NULL) DeleteObject(del);
  optimize();
  ret = rs == NULLREGION;
#elif defined(XWINDOW)
  OSREGIONHANDLE del;
  if (!outside)
    del = XCreateRegion();
  else
    del = outside->getOSHandle();
  XSubtractRegion(hrgn, GETOSHANDLE(r), del);
  ret = XEmptyRegion(del);
  if(!outside) XDestroyRegion(del);
#else
#error port me!
#endif // platform
  return ret;
}

#define IntersectRgn(hrgnResult, hrgnA, hrgnB) CombineRgn(hrgnResult, hrgnA, hrgnB, RGN_AND)

int RegionI::intersectRgn(const Region *r, Region *intersection) {
  deoptimize();
  int ret;
#if defined(WIN32)
  OSREGIONHANDLE del = NULL;
  if (intersection == NULL)
    del = CreateRectRgn(0,0,0,0);
  int rs = IntersectRgn(intersection ? intersection->getOSHandle() : del, hrgn, GETOSHANDLE(r));
  if (del != NULL) DeleteObject(del);
  optimize();
  ret = (rs != NULLREGION && rs != ERROR);
#elif defined(XWINDOW)
  OSREGIONHANDLE del;
  if (intersection == NULL)
    del = XCreateRegion();
  else
    del=intersection->getOSHandle();
  XIntersectRegion( hrgn, GETOSHANDLE( r ), del );
  ret = !XEmptyRegion( del );
  if (intersection == NULL) XDestroyRegion( del );
#else
#error port me!
#endif // platform
  return ret;
}

int RegionI::intersectRect(const RECT *r, Region *intersection) {
  return intersectRgn(&RegionI(r), intersection);
}

void RegionI::empty() {
  if (srv) {
#if defined(WIN32)
    hrgn = CreateRectRgn(0,0,0,0);
#elif defined(XWINDOW)
    hrgn = XCreateRegion();
#else
#error port me!
#endif // platform
    ASSERT(hrgn != NULL);
    srv->delRef(this);
    srv = NULL;
    optimize();
    return;
  }
  deoptimize();
  ASSERT(hrgn != NULL);
#if defined(WIN32)
  DeleteObject(hrgn);
  hrgn = CreateRectRgn(0,0,0,0);
#elif defined(XWINDOW)
  XDestroyRegion(hrgn);
  hrgn = XCreateRegion();
#else
#error port me!
#endif // platform
  ASSERT(hrgn != NULL);
  optimize();
}

void RegionI::setRect(const RECT *r) {
  if (srv) {
#if defined(WIN32)
    hrgn = CreateRectRgn(0,0,0,0);
#elif defined(XWINDOW)
    hrgn = XCreateRegion();
#else
#error port me!
#endif // platform
    srv->delRef(this);
    srv = NULL;
    optimize();
    return;
  }
  deoptimize();
  ASSERT(hrgn != NULL);
#if defined(WIN32)
  SetRectRgn(hrgn, r->left, r->top, r->right, r->bottom);
#elif defined(XWINDOW)
  XDestroyRegion( hrgn );
  hrgn = XCreateRegion();
#ifdef WASABI_WIN32RECT
  XRectangle xr = { r->left, r->top, r->right - r->left, r->bottom - r->top };
  XUnionRectWithRegion( &xr, hrgn, hrgn );
#else
  XUnionRectWithRegion( r, hrgn, hrgn );
#endif // RECT
#else
#error port me!
#endif // platform
  optimize();
}

int RegionI::equals(const Region *r) {
  ASSERT(r);
#if defined(WIN32)
  Region *cl = const_cast<Region*>(r)->clone();
  cl->subtractRegion(this);
  int ret = cl->isEmpty();
  const_cast<Region*>(r)->disposeClone(cl);
  cl = clone();
  cl->subtractRegion(r);
  ret &= cl->isEmpty();
  disposeClone(cl);
  return ret;
#elif defined(XWINDOW)
  return XEqualRegion( hrgn, GETOSHANDLE( r ) );
#else
#error port me!
#endif // platform
}

int RegionI::isRect() {
  if (optimized) return 1;
  RECT r;
  getBox(&r);
#if defined(WIN32)
  if (equals(&RegionI(&r))) {
    return 1;
  }
  return 0;
#elif defined(XWINDOW)
  Rect wr(r);
  return RectangleIn == XRectInRegion( hrgn, wr.left, wr.top, wr.width(), wr.height() );
#else
#error port me!
#endif // platform
}

#ifdef XWINDOW
void do_scale( OSREGIONHANDLE inrgn, OSREGIONHANDLE outrgn, XRectangle inrect, XRectangle outrect ) {
  switch( XRectInRegion( inrgn, inrect.x, inrect.y, inrect.width, inrect.height ) ) {
  case RectangleIn:
    XUnionRectWithRegion( &outrect, outrgn, outrgn );
    break;

  case RectangleOut:
    // Nothing to do..
    break;

  case RectanglePart:
    {
      XRectangle ti, to;
      ti = inrect; to = outrect;
      ti.width /= 2; ti.height /= 2;
      to.width /= 2; to.height /= 2;
      do_scale( inrgn, outrgn, ti, to );

      ti.x += ti.width;
      ti.width = inrect.width - ti.width;
      to.x += to.width;
      to.width = outrect.width - to.width;
      do_scale( inrgn, outrgn, ti, to );

      ti.x = inrect.x;
      ti.width = inrect.width / 2;
      ti.y += ti.height;
      ti.height = inrect.height - ti.height;
      to.x = outrect.x;
      to.width = outrect.width / 2;
      to.y += to.height;
      to.height = outrect.height - to.height;
      do_scale( inrgn, outrgn, ti, to );

      ti.x += ti.width;
      ti.width = inrect.width - ti.width;
      to.x += to.width;
      to.width = outrect.width - to.width;
      do_scale( inrgn, outrgn, ti, to );
    }
    break;
  }
}
#endif

void RegionI::scale(double sx, double sy, BOOL round) {
  if (srv) {
#if defined(WIN32)
    hrgn = CreateRectRgn(0,0,0,0);
#elif defined(XWINDOW)
    hrgn = XCreateRegion();
#else
#error port me!
#endif // platform
    RegionServer *s = srv;
    srv = NULL;
    addRegion(s->getRegion());
    s->delRef(this);
  }
  deoptimize();
  ASSERT(hrgn != NULL);
#if defined(WIN32)
  DWORD size=0;
  RECT box;
  getBox(&box);
  size = GetRegionData(hrgn, size, NULL); 
  if (!size) return;
  DWORD res;
  RGNDATA *data = (RGNDATA *)MALLOC(size);
  RECT *r = (RECT *)data->Buffer;

  res = GetRegionData(hrgn, size, (RGNDATA *)data);
  double adj = round?0.99999:0.0;
  int iadj = round?1:0;

  if (data->rdh.nCount == 1) {
    RECT nr = box;
    nr.left = (int)((double)nr.left * sx -iadj);
    nr.top = (int)((double)nr.top * sy -iadj);
    nr.right = (int)((double)nr.right * sx + adj);
    nr.bottom = (int)((double)nr.bottom * sy + adj);
    setRect(&nr);
    FREE(data);
    return;
  }

  for (int i=0;i<(int)data->rdh.nCount;i++) {
    r[i].left = (int)((double)r[i].left * sx -iadj);
    r[i].top = (int)((double)r[i].top * sy -iadj);
    r[i].right = (int)((double)r[i].right * sx + adj);
    r[i].bottom = (int)((double)r[i].bottom * sy + adj);
  }

  OSREGIONHANDLE nhrgn = ExtCreateRegion(NULL, size, data);
  if (!nhrgn) {
	 nhrgn = CreateRectRgn(0,0,0,0);
  }
  FREE(data);
  DeleteObject(hrgn);
  hrgn = nhrgn;
#elif defined(XWINDOW)
  OSREGIONHANDLE outrgn = XCreateRegion();
  
  XRectangle inrect, outrect;
  XClipBox( hrgn, &inrect );
  outrect.x = (short int)(inrect.x * sx);
  outrect.y = (short int)(inrect.y * sy);
  outrect.width = (int)(inrect.width * sx);
  outrect.height = (int)(inrect.height * sy);

  do_scale( hrgn, outrgn, inrect, outrect );

  XDestroyRegion( hrgn );
  hrgn = outrgn;
#else
#error port me!
#endif // platform

  optimize();
}

void RegionI::debug(int async) {
#if defined(WIN32)
  if (!async) {
    SysCanvas c;
    RECT r;
    getBox(&r);
//    c.fillRect(&r, 0);
    InvertRgn(c.getHDC(), getOSHandle());
    Sleep(200);
    InvertRgn(c.getHDC(), getOSHandle());
  } else {
    SysCanvas c;
    RECT r;
    getBox(&r);
//    c.fillRect(&r, 0);
    if (lastdebug)
      InvertRgn(c.getHDC(), lastdebug->getOSHandle());
    delete lastdebug;
    lastdebug = new RegionI();
    lastdebug->addRegion(this);
    InvertRgn(c.getHDC(), getOSHandle());
  }
#else
#warning RegionI::debug() does nothing.
  DebugString( "portme -- RegionI::debug\n" );
#endif
}

#ifdef XWINDOW
// Sssh - we're not supposed to know about this...
// WaI> ... says Keith Peters, but Keith Packard (of XOrg) says:
//   "There are several extension libraries which refer to the internals of the
//   _XRegion structure for precisely this purpose [getting rectangles/BOXs].  
//   I assert that this makes the _XRegion structure part of the public Xlib API 
//   and hence subject to the same rules as the rest of the API regarding changes 
//   (i.e. it will never change)."
// so, it's okay to know about it. (internal_region == _XRegion)
struct BOX {
  short x1, x2, y1, y2;
};

struct internal_region {
  long size;
  long numRects;
  BOX *rects;
  BOX extents;
};
#endif

// later we can cache this data or something if needed
int RegionI::getNumRects() {
  if (optimized) return 1;
#if defined(WIN32)
  int bytes_needed = GetRegionData(hrgn, 0, NULL) + sizeof(RGNDATA);
  MemBlock<unsigned char> data(bytes_needed);
  GetRegionData(hrgn, bytes_needed, (LPRGNDATA)data.getMemory());
  RGNDATA *rgndata = reinterpret_cast<RGNDATA *>(data.getMemory());
  return rgndata->rdh.nCount;
#elif defined(XWINDOW)
  return ((internal_region *)hrgn)->numRects;
#else
#error port me!
#endif // platform
}

int RegionI::enumRect(int n, RECT *r) {
  if (optimized) {
    if (n == 0) {
      if (r != NULL) *r = optrect;
      return 1;
    }
    return 0;
  }
#if defined(WIN32)
  if (n < 0) return 0;
  int bytes_needed = GetRegionData(hrgn, 0, NULL) + sizeof(RGNDATA);
  MemBlock<unsigned char> data(bytes_needed);
  GetRegionData(hrgn, bytes_needed, (LPRGNDATA)data.getMemory());
  RGNDATA *rgndata = reinterpret_cast<RGNDATA *>(data.getMemory());
  int nrects = rgndata->rdh.nCount;
  if (n >= nrects) return 0;
  RECT *rectlist = reinterpret_cast<RECT*>(rgndata->Buffer);
  *r = rectlist[n];
  return 1;
#elif defined(XWINDOW)
  if ( n >= getNumRects() || n < 0 ) return 0;

  BOX b = ((internal_region *)hrgn)->rects[n];

  (*r) = Std::makeRect(b.x1, b.y1, b.x2, b.y2);
  return 1;
#else
#error port me!
#endif // platform
}

void RegionI::optimize() {
  if (optimized) return;
  ASSERT(hrgn != NULL);
  if (srv != NULL) return; // region is cached and shared, do not optimize
#if defined(WIN32)
  getBox(&optrect);
  RECT br;
  OSREGIONHANDLE gr = CreateRectRgnIndirect(&optrect);
  OSREGIONHANDLE res = CreateRectRgn(0,0,0,0);
  // if they don't intersect, we may be offset
  IntersectRgn(res, gr, hrgn);
  GetRgnBox(res, &br);
  if (br.left == br.right || br.bottom == br.top) {
    DeleteObject(gr);
    DeleteObject(res);
    return;
  }
  // if they intersect, but when subtracting the region from the rect, we get nothing, they're the same, let's optimize
  CombineRgn(res, gr, hrgn, RGN_DIFF);
  DeleteObject(gr);
  GetRgnBox(res, &br);
  DeleteObject(res);
  if (br.left == br.right || br.bottom == br.top) {
    optimized = 1;
    DeleteObject(hrgn);
    hrgn = NULL;
  }
#else
// other platforms probably don't need to be optimized like this.
#endif
}

void RegionI::deoptimize() {
  if (!optimized) return;
  ASSERT(hrgn == NULL);
#if defined(WIN32)
  hrgn = CreateRectRgnIndirect(&optrect);
  if (hrgn == NULL) {
    DebugString(Std::getLastErrorString().getValue());
    ASSERT(optrect.right > optrect.left && optrect.top < optrect.bottom);
  }
  ASSERT(hrgn != NULL);
  optimized = 0;
#else
// other platforms probably don't need to be optimized like this.
#endif
}


#define CBCLASS RegionServerI
START_DISPATCH;
  VCB(REGIONSERVER_ADDREF, addRef);
  VCB(REGIONSERVER_DELREF, delRef);
  CB(REGIONSERVER_GETREGION, getRegion);
END_DISPATCH;

//CUT #endif//WIN32
