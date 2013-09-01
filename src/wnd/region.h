#ifndef _REGION_H
#define _REGION_H

#include <bfc/platform/platform.h>

//#include "bitmap.h"
#include <bfc/dispatch.h>

class BaseWnd;
class Bitmap;
class Canvas;
class Region;
class RegionServer;

class Region : public Dispatchable {
protected:
  Region() {}
  virtual ~Region() {}

public:
  OSREGIONHANDLE getOSHandle(); // avoid as much as you can, should be used only when you need to call the OS api
    
  Region *clone();
  void disposeClone(Region *r);
  BOOL ptInRegion(const POINT *pt);
  void offset(int x, int y);
  void getBox(RECT *r);
  void subtractRegion(const Region *r);
  void subtractRgn(const Region *r) { subtractRegion(r); }//DEPRECATED
  void subtractRect(const RECT *r);
  void addRect(const RECT *r);
  void addRegion(const Region *r);
  void andRegion(const Region *r);
  void setRect(const RECT *r);
  void empty();
  int isEmpty();
  int equals(const Region *r);
  int enclosed(const Region *r, Region *outside=NULL);
  int intersectRgn(const Region *r, Region *intersection=NULL);
  int intersectRect(const RECT *r, Region *intersection=NULL);
  int isRect();
  void scale(double sx, double sy, BOOL round=0);
  void debug(int async=0);
  OSREGIONHANDLE makeWindowRegion(); // gives you a handle to a clone of the OSREGIONHANDLE object so you can insert it into a window's region with SetWindowRgn. ANY other use is prohibited

  // this is how you can enumerate the subrects that compose to make up the
  // entire region
  int getNumRects();
  int enumRect(int n, RECT *r);

  enum {
    REGION_GETOSHANDLE = 50,
    REGION_CLONE = 100,
    REGION_DISPOSECLONE = 110,
    REGION_PTINREGION = 120,
    REGION_OFFSET = 130,
    REGION_GETBOX = 140,
    REGION_SUBTRACTRGN = 150,
    REGION_SUBTRACTRECT = 160,
    REGION_ADDRECT = 170,
    REGION_ADD = 180,
    REGION_AND = 190,
    REGION_SETRECT = 200,
    REGION_EMPTY = 210,
    REGION_ISEMPTY = 220,
    REGION_EQUALS = 230,
    REGION_ENCLOSED = 240,
    REGION_INTERSECTRGN = 250,
    REGION_INTERSECTRECT = 260,
    REGION_ISRECT = 270,
    REGION_SCALE = 280,
    REGION_DEBUG = 290,
    REGION_MAKEWNDREGION = 300,
    REGION_GETNUMRECTS = 310,
    REGION_ENUMRECT = 320,
  };
};

inline OSREGIONHANDLE Region::getOSHandle() {
  return _call(REGION_GETOSHANDLE, (OSREGIONHANDLE)NULL);
}
    
inline Region *Region::clone() {
  return _call(REGION_CLONE, (Region *)NULL);
}

inline void Region::disposeClone(Region *r) {
  _voidcall(REGION_DISPOSECLONE, r);
}

inline int Region::ptInRegion(const POINT *pt) {
  return _call(REGION_PTINREGION, 0, pt);
}

inline void Region::offset(int x, int y) {
  _voidcall(REGION_OFFSET, x, y);
}

inline void Region::getBox(RECT *r) {
  _voidcall(REGION_GETBOX, r);
}

inline void Region::subtractRegion(const Region *reg) {
  _voidcall(REGION_SUBTRACTRGN, reg);
}

inline void Region::subtractRect(const RECT *r) {
  _voidcall(REGION_SUBTRACTRECT, r);
}

inline void Region::addRect(const RECT *r) {
  _voidcall(REGION_ADDRECT, r);
}

inline void Region::addRegion(const Region *r) {
  _voidcall(REGION_ADD, r);
}

inline void Region::andRegion(const Region *r) {
  _voidcall(REGION_AND, r);
}

inline void Region::setRect(const RECT *r) {
  _voidcall(REGION_SETRECT, r);
}

inline void Region::empty() {
  _voidcall(REGION_EMPTY);
}

inline int Region::isEmpty() {
  return _call(REGION_ISEMPTY, 0);
}

inline int Region::equals(const Region *r) {
  return _call(REGION_EQUALS, 0, r);
}

inline int Region::enclosed(const Region *r, Region *outside) {
  return _call(REGION_ENCLOSED, 0, r, outside);
}

inline int Region::intersectRgn(const Region *r, Region *intersection) {
  return _call(REGION_INTERSECTRGN, 0, r, intersection);
}

inline int Region::intersectRect(const RECT *r, Region *intersection) {
  return _call(REGION_INTERSECTRECT, 0, r, intersection);
}

inline int Region::isRect() {
  return _call(REGION_ISRECT, 0);
}

inline void Region::scale(double sx, double sy, BOOL round) {
  _voidcall(REGION_SCALE, sx, sy, round);
}

inline void Region::debug(int async) {
  _voidcall(REGION_DEBUG, async);
}

inline OSREGIONHANDLE Region::makeWindowRegion() {
  return _call(REGION_MAKEWNDREGION, (OSREGIONHANDLE)NULL);
}

inline int Region::getNumRects() {
  return _call(REGION_GETNUMRECTS, 0);
}

inline int Region::enumRect(int n, RECT *r) {
  return _call(REGION_ENUMRECT, 0, n, r);
}

class RegionI : public Region {
public:
  RegionI();
  RegionI(const RECT *r);
  RegionI(int l, int t, int r, int b);
  RegionI(OSREGIONHANDLE region);
  RegionI(Bitmap *bitmap, RECT *r=NULL, int xoffset=0, int yoffset=0, BOOL inverted=FALSE, int dothreshold=0, char threshold=0, int threversed=0, int minalpha=1); 
  RegionI(Canvas *c, RECT *defboundbox=NULL);
  virtual ~RegionI();

  Region *clone();
  void disposeClone(Region *r);
  BOOL ptInRegion(const POINT *pt);
  void offset(int x, int y);
  void getBox(RECT *r);
  void subtractRegion(const Region *reg);
  void subtractRect(const RECT *r);
  void addRect(const RECT *r);
  void addRegion(const Region *r);
  void andRegion(const Region *r);
  void setRect(const RECT *r);
  void empty();
  int isEmpty();
  int equals(const Region *r);
  int enclosed(const Region *r, Region *outside=NULL);
  int intersectRgn(const Region *r, Region *intersection=NULL);
  int intersectRect(const RECT *r, Region *intersection=NULL);
  int isRect();
  void scale(double sx, double sy, BOOL round=0);
  void debug(int async=0);

  // NONPORTABLE

  OSREGIONHANDLE makeWindowRegion(); // gives you a handle to a clone of the OSREGIONHANDLE object so you can insert it into a window's region with SetWindowRgn. ANY other use is prohibited
  OSREGIONHANDLE getOSHandle(); // avoid as much as you can, should be used only by WIN32-dependant classes

  // END NONPORTABLE

  int getNumRects();
  int enumRect(int n, RECT *r);

  OSREGIONHANDLE alphaToRegionRect(void *pbits32, int bmX, int bmY, int bmWidth, int bmHeight, int fullw, int fullh, int xoffset, int yoffset, BOOL portion, int _x, int _y, int _w, int _h, BOOL inverted, int dothreshold, unsigned char threshold, int thinverse, int minalpha);

private:
  void init();
  void optimize();
  void deoptimize();

  OSREGIONHANDLE hrgn;
  OSREGIONHANDLE alphaToRegionRect(Bitmap *bitmap, int xoffset, int yoffset, BOOL portion, int _x, int _y, int _w, int _h, BOOL inverted=FALSE, int dothreshold=0, unsigned char threshold=0, int thinverse=0, int minalpha=1/* 1..255*/);
  RECT overlay;
  int clonecount;
  RegionI *lastdebug;
  RegionServer *srv;
  RECT optrect;
  int optimized;

protected:
  RECVS_DISPATCH;
};

class RegionServer : public Dispatchable {
protected:
  RegionServer() {}
  virtual ~RegionServer() {}

public:
  void addRef(void *client);
  void delRef(void *client);
  Region *getRegion();

  enum {
    REGIONSERVER_ADDREF = 500,
    REGIONSERVER_DELREF = 550,
    REGIONSERVER_GETREGION = 600,
  };
};

inline void RegionServer::addRef(void *client) {
  _voidcall(REGIONSERVER_ADDREF, (Region *)NULL, client);
}

inline void RegionServer::delRef(void *client) {
  _voidcall(REGIONSERVER_DELREF, client);
}

inline Region * RegionServer::getRegion() {
  return _call(REGIONSERVER_GETREGION, (Region *)NULL);
}

class RegionServerI : public RegionServer {
public:
  RegionServerI() { numrefs = 0; }
  virtual ~RegionServerI() {}
    
  virtual void addRef(void *client) { numrefs++; }
  virtual void delRef(void *client) { numrefs--; }
  virtual Region *getRegion()=0;

  virtual int getNumRefs() { return numrefs; }

protected:
  RECVS_DISPATCH;

private:
  int numrefs;
};

#endif


