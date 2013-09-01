#include "precomp.h"
#include "api_skin.h"


#ifdef CBCLASS
#undef CBCLASS
#endif

#define CBCLASS api_skinI
START_DISPATCH;
    CB(API_SKIN_SKIN_GETELEMENTCOLOR2, skin_getColorElement);
    CB(API_SKIN_SKIN_GETELEMENTREF2, skin_getColorElementRef);
    CB(API_SKIN_SKIN_GETBITMAPCOLOR, skin_getBitmapColor);
    CB(API_SKIN_SKIN_GETITERATOR, skin_getIterator);
    VCB(API_SKIN_SKIN_SETSKINPATHOVERRIDE, setSkinPathOverride);
    VCB(API_SKIN_SKIN_RENDERBASETEXTURE, skin_renderBaseTexture);
    VCB(API_SKIN_SKIN_REGISTERBASETEXTUREWINDOW, skin_registerBaseTextureWindow);
    VCB(API_SKIN_SKIN_UNREGISTERBASETEXTUREWINDOW, skin_unregisterBaseTextureWindow);
    VCB(API_SKIN_SKIN_SWITCHSKIN, skin_switchSkin);
    CB(API_SKIN_SKIN_GETVERSION, skin_getVersion);
    VCB(API_SKIN_SETLOCKUI, skin_setLockUI);
    CB(API_SKIN_GETLOCKUI, skin_getLockUI);
    CB(API_SKIN_GETSKINNAME, getSkinName);
    CB(API_SKIN_GETSKINPATH, getSkinPath);
    CB(API_SKIN_GETSKINSPATH, getSkinsPath);
    CB(API_SKIN_GETDEFAULTSKINPATH, getDefaultSkinPath);
    //CB(API_SKIN_PATH_GETAPPPATH,
    CB(API_SKIN_IMGLDR_MAKEBMP, imgldr_makeBmp);
    //CB(API_SKIN_OLDIMGLDR_MAKEBMP2,
    CB(API_SKIN_IMGLDR_MAKEBMP2, imgldr_makeBmp2);
    CB(API_SKIN_IMGLDR_REQUESTSKINBITMAP, imgldr_requestSkinBitmap);
    VCB(API_SKIN_IMGLDR_RELEASESKINBITMAP, imgldr_releaseSkinBitmap);
    VCB(API_SKIN_IMGLDR_RETAINSKINBITMAP, imgldr_retainSkinBitmap);
    CB(API_SKIN_IMGLDR_REQUESTSKINREGION , imgldr_requestSkinRegion);
    VCB(API_SKIN_IMGLDR_CACHESKINREGION, imgldr_cacheSkinRegion);
    //CB(API_SKIN_REGISTERSKINFILTER,
    //CB(API_SKIN_UNREGISTERSKINFILTER, 
    VCB(API_SKIN_REAPPLYSKINFILTERS, reapplySkinFilters);
    CB(API_SKIN_FILTERSKINCOLOR, filterSkinColor);
    CB(API_SKIN_GETNUMCOLORSETS, colortheme_getNumColorSets);
    CB(API_SKIN_ENUMCOLORSET, colortheme_enumColorSet);
    CB(API_SKIN_GETNUMCOLORGROUPS, colortheme_getNumColorGroups);
    CB(API_SKIN_ENUMCOLORGROUPNAME, colortheme_enumColorGroupName);
    CB(API_SKIN_ENUMCOLORGROUP, colortheme_enumColorGroup);
    CB(API_SKIN_GETCOLORGROUP,colortheme_getColorGroup);
    VCB(API_SKIN_SETCOLORSET, colortheme_setColorSet);
    CB(API_SKIN_GETCOLORSET, colortheme_getColorSet);
    VCB(API_SKIN_NEWCOLORSET, colortheme_newColorSet);
    VCB(API_SKIN_UPDATECOLORSET, colortheme_updateColorSet);
    VCB(API_SKIN_PAINTSET_RENDER, paintset_render);
    VCB(API_SKIN_PAINTSET_RENDERTITLE, paintset_renderTitle);
    CB(API_SKIN_PAINTSET_PRESENT, paintset_present);
    VCB(API_SKIN_DRAWANIMATEDRECTS, drawAnimatedRects);
    VCB(API_SKIN_SENDDDECOMMAND, sendDdeCommand);
END_DISPATCH;
#undef CBCLASS

ARGB32 *api_skinI::imgldr_makeBmp2(OSMODULEHANDLE hInst, int id, int *has_alpha, int *w, int *h, const char *colorgroup) {
  return static_cast<api_skin *>(this)->imgldr_makeBmp(hInst, id, has_alpha, w, h, colorgroup);
}