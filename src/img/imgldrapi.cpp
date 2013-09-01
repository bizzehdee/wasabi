#include "precomp.h"
#include "imgldrapi.h"
#include <img/imgldr.h>
#ifdef WASABI_COMPILE_SKIN
#include <api/skin/skinelem.h>
#endif

imgldr_api *imgLoaderApi = NULL;

ARGB32 *ImgLdrApi::imgldr_makeBmp(const char *filename, int *has_alpha, int *w, int *h)
{
  if (filename == NULL) {
    DebugString("illegal param : filename == NULL");
    return NULL;
  }
  return imageLoader::makeBmp(filename, NULL, has_alpha, w, h, NULL, TRUE, NULL);
}

ARGB32 *ImgLdrApi::imgldr_makeBmp2(OSMODULEHANDLE hInst, int id, int *has_alpha, int *w, int *h, const char *colorgroup) {
  return imageLoader::makeBmp(hInst, id, has_alpha,w,h, colorgroup);
}

void ImgLdrApi::imgldr_releaseBmp(ARGB32 *bmpbits) {
  if (bmpbits == NULL) {
    DebugString("illegal param : bmpbits == NULL");
    return;
  }
  imageLoader::release(bmpbits);
}

#ifdef WASABI_COMPILE_SKIN

ARGB32 *ImgLdrApi::imgldr_requestBitmap(const char *file, int *has_alpha, int *x, int *y, int *subw, int *subh, int *w, int *h, int cached) {
  if (file == NULL) {
    DebugString("illegal param : file == NULL");
    return NULL;
  }
  return imageLoader::requestBitmap(file, has_alpha, x, y, subw, subh, w, h, cached);
}

void ImgLdrApi::imgldr_releaseBitmap(ARGB32 *bmpbits) {
  if (bmpbits == NULL) {
    DebugString("illegal param : bmpbits == NULL");
    return;
  }
  imageLoader::releaseBitmap(bmpbits);
}

void ImgLdrApi::imgldr_retainBitmap(ARGB32 *bmpbits) {
  if (bmpbits == NULL) {
    DebugString("illegal param : bmpbits == NULL");
    return;
  }
  imageLoader::retainBitmap(bmpbits);
}

RegionServer *ImgLdrApi::imgldr_requestSkinRegion(const char *id) {
  if (id == NULL) {
    DebugString("illegal param : id == NULL");
    return NULL;
  }
  return SkinElementsMgr::requestSkinRegion(id);
}

void ImgLdrApi::imgldr_cacheSkinRegion(const char *id, Region *r) {
  if (id == NULL) {
    DebugString("illegal param : id == NULL");
  }
  if (r == NULL) {
    DebugString("illegal param : region == NULL");
  }
  SkinElementsMgr::cacheSkinRegion(id, r);
}

#endif

