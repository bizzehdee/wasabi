#include "precomp.h"
#include "skinapi.h"
#include "skinelem.h"
#include "draw/bitmap.h"
#include "img/imgldr.h"

api_skin *skinApi = NULL;
Bitmap *basetexture = NULL;

skinApiI::skinApiI() {
  // You could specifiy your skin path here or just after Apiinit in your main
  //SkinElementsMgr::setSkinsPath("X:/");
  //SkinElementsMgr::loadSkin("wasabi_player_skin");
  
}

skinApiI::~skinApiI() {
  if ( basetexture ) delete basetexture;


}

COLORREF skinApiI::skin_getColorElement(char *type, const char **color_grp)  {

  return SkinElementsMgr::skin_getColorElement(type,color_grp);
}


COLORREF *skinApiI::skin_getColorElementRef(char *type, const char **color_grp)  {

  return NULL;
}



int *skinApiI::skin_getIterator()  {
  return SkinElementsMgr::getSkinIterator();
}



void skinApiI::setSkinPathOverride(const char *path)  {

}



void skinApiI::skin_setLockUI(int l)  {

}



int skinApiI::skin_getLockUI()  {

  return 0;
}



double skinApiI::skin_getVersion()  {

  return 1;
}



COLORREF skinApiI::skin_getBitmapColor(const char *bitmapid)  {

  return RGB(0,0,0);
}



void skinApiI::skin_renderBaseTexture(RootWnd *base, CanvasBase *c, RECT *r, RootWnd *destWnd, int alpha)  {
  if( c && r ) {
    if ( !basetexture ) basetexture = new Bitmap("wasabi.basetexture");
    if ( basetexture ) basetexture->stretchToRect(c, r);
  }
}



void skinApiI::skin_registerBaseTextureWindow(RootWnd *window, const char *bmp)  {

}



void skinApiI::skin_unregisterBaseTextureWindow(RootWnd *window)  {

}



void skinApiI::skin_switchSkin(char *skin_name)  {

}



const char *skinApiI::getSkinName()  {

  return NULL;
}



const char *skinApiI::getSkinPath()  {
  return SkinElementsMgr::getSkinPath();
}



const char *skinApiI::getSkinsPath()  {

  return NULL;
}



const char *skinApiI::getDefaultSkinPath()  {

  return NULL;
}



ARGB32 *skinApiI::imgldr_makeBmp(const char *filename, int *has_alpha, int *w, int *h)  {

  return NULL;
}



  //virtual ARGB32 *imgldr_makeBmp(OSMODULEHANDLE hInst, int id, int *has_alpha, int *w, int *h, const char *colorgroup=NULL);
ARGB32 *skinApiI::imgldr_makeBmp(OSMODULEHANDLE hInst, int id, int *has_alpha, int *w, int *h, const char *colorgroup)  {

  return NULL;
}


ARGB32 *skinApiI::imgldr_requestSkinBitmap(const char *file, int *has_alpha, int *x, int *y, int *subw, int *subh, int *w, int *h, int cached)  {

  return NULL;
}



RegionServer *skinApiI::imgldr_requestSkinRegion(const char *id)  {

  return NULL;
}



void skinApiI::imgldr_cacheSkinRegion(const char *id, Region *r)  {

}



void skinApiI::imgldr_releaseSkinBitmap(ARGB32 *bmpbits)  {
  imageLoader::releaseBitmap(bmpbits);
}

void skinApiI::imgldr_retainSkinBitmap(ARGB32 *bmpbits)  {
  imageLoader::retainBitmap(bmpbits);
}


COLORREF skinApiI::filterSkinColor(COLORREF color, const char *elementid, const char *groupname)  {

  return color;
}



void skinApiI::reapplySkinFilters()  {

}



int skinApiI::colortheme_getNumColorSets()  {

  return 0;
}



const char *skinApiI::colortheme_enumColorSet(int n)  {

  return NULL;
}



int skinApiI::colortheme_getNumColorGroups(const char *colorset)  {

  return 0;
}



const char *skinApiI::colortheme_enumColorGroupName(const char *colorset, int n)  {

  return NULL;
}



ColorThemeGroup *skinApiI::colortheme_enumColorGroup(int colorset, int colorgroup)  {

  return NULL;
}



ColorThemeGroup *skinApiI::colortheme_getColorGroup(const char *colorset, const char *colorgroup)  {

  return NULL;
}



void skinApiI::colortheme_setColorSet(const char *colorset)  {

}



const char *skinApiI::colortheme_getColorSet()  {

  return NULL;
}



void skinApiI::colortheme_newColorSet(const char *set)  {

}



void skinApiI::colortheme_updateColorSet(const char *set)  {

}



int skinApiI::paintset_present(int set)  {

  return 0;
}



void skinApiI::paintset_render(int set, CanvasBase *c, const RECT *r, int alpha)  {

}



void skinApiI::paintset_renderTitle(char *t, CanvasBase *c, const RECT *r, int alpha)  {

}



void skinApiI::drawAnimatedRects(const RECT *r1, const RECT *r2)  {

}



void skinApiI::sendDdeCommand(const char *application, const char *command, DWORD minimumDelay)  {

}



int skinApiI::getNumComponents()  {

  return 0;
}



GUID skinApiI::getComponentGUID(int c)  {

  return INVALID_GUID;
}



const char *skinApiI::getComponentName(GUID componentGuid)  {

  return NULL;
}



CfgItem *skinApiI::compon_getCfgInterface(GUID componentGuid)  {

  return NULL;
}



int skinApiI::comp_notifyScripts(const char *s, int i1, int i2)  {

  return 0;
}



int skinApiI::autopopup_registerGuid(GUID g, const char *desc, const char *prefered_container, int container_flag)  {

  return 0;
}



int skinApiI::autopopup_registerGroupId(const char *groupid, const char *desc, const char *prefered_container, int container_flag)  {

  return 0;
}



void skinApiI::autopopup_unregister(int id)  {

}



int skinApiI::skinwnd_toggleByGuid(GUID g, const char *prefered_container, int container_flag, RECT *sourceanimrect, int transcient)  {

  return 0;
}



int skinApiI::skinwnd_toggleByGroupId(const char *groupid, const char *prefered_container, int container_flag, RECT *sourceanimrect, int transcient)  {

  return 0;
}



RootWnd *skinApiI::skinwnd_createByGuid(GUID g, const char *prefered_container, int container_flag, RECT *sourceanimrect, int transcient, int starthidden, int *isnew)  {

  return NULL;
}



RootWnd *skinApiI::skinwnd_createByGroupId(const char *groupid, const char *prefered_container, int container_flag, RECT *sourceanimrect, int transcient, int starthidden, int *isnew)  {

  return NULL;
}



void skinApiI::skinwnd_destroy(RootWnd *w, RECT *destanimrect)  {

}



int skinApiI::skinwnd_getNumByGuid(GUID g)  {

  return 0;
}



RootWnd *skinApiI::skinwnd_enumByGuid(GUID g, int n)  {

  return NULL;
}



int skinApiI::skinwnd_getNumByGroupId(const char *groupid)  {

  return 0;
}



RootWnd *skinApiI::skinwnd_enumByGroupId(const char *groupid, int n)  {

  return NULL;
}



void skinApiI::skinwnd_attachToSkin(RootWnd *w, int side, int size)  {

}



ScriptObject *skinApiI::skin_getContainer(const char *container_name)  {

  return NULL;
}



ScriptObject *skinApiI::skin_getLayout(ScriptObject *container, const char *layout_name)  {

  return NULL;
}



int skinApiI::loadSkinFile(const char *xmlfile)  {

  return 0;
}



void skinApiI::unloadSkinPart(int skinpart)  {

}


