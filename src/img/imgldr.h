#ifndef __IMGLDR_H
#define __IMGLDR_H

//CUT
#define FT_UNKNOWN 0
#define FT_BMP		 1
#define FT_JPEG		 5
#define FT_PNG		 6

#include <bfc/ptrlist.h>
#ifdef WASABI_COMPILE_XMLPARSER
#include <api/xml/xmlreader.h>
#include <api/xml/xmlparamsi.h>
#else
class XmlReaderParamsI;
#endif
#ifdef WASABI_COMPILE_SKIN
#include <api/skin/api_skin.h>
#include <api/skin/skinfilter.h>
#endif

typedef struct {
  ARGB32 *bitmapbits;
  String filename;
  int usageCount;
  BOOL has_alpha;
  int width;
  int height;
  String includepath;
//  String rootpath;
  XmlReaderParamsI *params;
  String original_element_id;
  String fullpathfilename;
} skinCacheEntry;

typedef unsigned long ARGB32;

#ifdef WASABI_COMPILE_SKIN

class skinCacheComp {
public:
  static int compareItem(void *p1, void *p2) {
    return STRCMP(((skinCacheEntry *)p1)->fullpathfilename, ((skinCacheEntry *)p2)->fullpathfilename);
  }
  static int compareAttrib(const char *attrib, void *item) {
    return STRCMP(attrib, ((skinCacheEntry *)item)->fullpathfilename);
  }
};

#endif //WASABI_COMPILE_SKIN

#ifndef WASABI_API_XML
#define XmlReaderParams void
#endif

class imageLoader {
public:
  static ARGB32 *makeBmp(const char *filename, const char *path, int *has_alpha, int *w, int *h, XmlReaderParams *params, BOOL addmem, int *force_nocache);
  static ARGB32 *makeBmp(HINSTANCE hInst, int id, int *has_alpha, int *w, int *h, const char *forcegroup=NULL);
  static int getFileType(unsigned char *pData);
  static String getWallpaper();
  static void release(ARGB32 *bitmapbits);
#ifdef WASABI_COMPILE_SKIN
  static ARGB32 *requestBitmap(const char *id, int *has_alpha, int *x, int *y, int *subw, int *subh, int *w, int *h, int cached=1);
  static void releaseBitmap(ARGB32 *bmpbits);
  static void retainBitmap(ARGB32 *bmpbits);
  static void applySkinFilters();
  static void applySkinFilters(skinCacheEntry *entry);
  static COLORREF filterSkinColor(COLORREF color, const char *elementid, const char *groupname);
  static int layerEqual(const char *id1, const char *id2);
#endif //WASABI_COMPILE_SKIN
  static int getMemUsage();
  static int getNumCached();
private:
#ifdef WASABI_COMPILE_SKIN
  static int paramsMatch(XmlReaderParams *a, XmlReaderParams *b);
  static PtrListInsertMultiSorted<skinCacheEntry,skinCacheComp> skinCacheList;
#endif //WASABI_COMPILE_SKIN
  static void optimizeHasAlpha(ARGB32 *bits, int len, int *has_alpha);
  static void addMemUsage(const char *filename, int size);
  static void subMemUsage(int size);
  static int totalMemUsage;
};
#endif
