#ifndef __IMGLDRAPI_H
#define __IMGLDRAPI_H

#include <img/api_imgldr.h>

class ImgLdrApi : public imgldr_apiI {
  public:
     ARGB32 *imgldr_makeBmp(const char *filename, int *has_alpha, int *w, int *h);
     ARGB32 *imgldr_makeBmp2(OSMODULEHANDLE hInst, int id, int *has_alpha, int *w, int *h, const char *colorgroup = NULL);
     void imgldr_releaseBmp(ARGB32 *bmpbits);
#ifdef WASABI_COMPILE_SKIN
     ARGB32 *imgldr_requestBitmap(const char *file, int *has_alpha, int *x, int *y, int *subw, int *subh, int *w, int *h, int cached);
     RegionServer *imgldr_requestSkinRegion(const char *id);
     void imgldr_cacheSkinRegion(const char *id, Region *r);
     void imgldr_releaseBitmap(ARGB32 *bmpbits);
     void imgldr_retainBitmap(ARGB32 *bmpbits);
#endif //WASABI_COMPILE_SKIN
};


#endif
