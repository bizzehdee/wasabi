#ifndef __SKINELEMMGR_H
#define __SKINELEMMGR_H

#include <bfc/ptrlist.h>
#ifdef WASABI_COMPILE_XMLPARSER
#include <api/xml/xmlreader.h>
#include <api/xml/xmlparamsi.h>
#endif
#ifndef WASABI_API_XML
#define XmlReaderParams void
#endif

class RegionServer;
class Region;
class DOMDocument;


class SkinElementsMgr {
public:
  static void setSkinsPath(const char *path);
  static const char * getSkinPath();
  static int *getSkinIterator();
  static int loadSkin(const char *name);
  static int cacheSkin(bool force=false);
  static int getLayerFromId(const char *id);
  static const char *getElementAlias(const char *id);
  static COLORREF skin_getColorElement(char *type, const char **color_grp);
  static const char *getBitmapFilename(const char *id, int *x, int *y, int *subw, int *subh,int *d, const char **rootpath, XmlReaderParams **params);
  static RegionServer *requestSkinRegion(const char *id);
  static void cacheSkinRegion(const char *id, Region *r);
};


#endif
