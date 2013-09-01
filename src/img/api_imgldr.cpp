#include <precomp.h>
#include "api_imgldr.h"

#ifdef CBCLASS
#undef CBCLASS
#endif
#define CBCLASS imgldr_apiI
START_DISPATCH;
  CB(IMGLDR_API_MAKEBMP,            imgldr_makeBmp);
  CB(IMGLDR_API_MAKEBMP2,           imgldr_makeBmp2);
  VCB(IMGLDR_API_RELEASEBMP,        imgldr_releaseBmp);
#ifdef WASABI_COMPILE_SKIN
  CB(IMGLDR_API_REQUESTBITMAP,  imgldr_requestBitmap);
  CB(IMGLDR_API_REQUESTSKINREGION,  imgldr_requestSkinRegion);
  VCB(IMGLDR_API_CACHESKINREGION,   imgldr_cacheSkinRegion);
  VCB(IMGLDR_API_RELEASEBITMAP, imgldr_releaseBitmap);
  VCB(IMGLDR_API_RETAINBITMAP, imgldr_retainBitmap);
#endif //WASABI_COMPILE_SKIN
END_DISPATCH;
