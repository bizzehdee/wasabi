#ifndef __SKINAPI_H
#define __SKINAPI_H

#include "api_skin.h"


class skinApiI: public api_skinI {
public:
  skinApiI();
  ~skinApiI();
  static const char *getServiceName() { return "Skin API"; }
  static GUID getServiceType() { return WaSvc::UNIQUE; }
  virtual COLORREF skin_getColorElement(char *type, const char **color_grp=NULL);
  virtual COLORREF *skin_getColorElementRef(char *type, const char **color_grp=NULL);
  virtual int *skin_getIterator();  
  virtual void setSkinPathOverride(const char *path);
  virtual void skin_setLockUI(int l);
  virtual int skin_getLockUI();
  virtual double skin_getVersion();
  virtual COLORREF skin_getBitmapColor(const char *bitmapid);
  virtual void skin_renderBaseTexture(RootWnd *base, CanvasBase *c, RECT *r, RootWnd *destWnd, int alpha=255);
  virtual void skin_registerBaseTextureWindow(RootWnd *window, const char *bmp=NULL);
  virtual void skin_unregisterBaseTextureWindow(RootWnd *window);
  virtual void skin_switchSkin(char *skin_name);
  virtual const char *getSkinName();
  virtual const char *getSkinPath();
  virtual const char *getSkinsPath();
  virtual const char *getDefaultSkinPath();
  virtual ARGB32 *imgldr_makeBmp(const char *filename, int *has_alpha, int *w, int *h);
  virtual ARGB32 *imgldr_makeBmp(OSMODULEHANDLE hInst, int id, int *has_alpha, int *w, int *h, const char *colorgroup=NULL);
  virtual ARGB32 *imgldr_requestSkinBitmap(const char *file, int *has_alpha, int *x, int *y, int *subw, int *subh, int *w, int *h, int cached);
  virtual RegionServer *imgldr_requestSkinRegion(const char *id);
  virtual void imgldr_cacheSkinRegion(const char *id, Region *r);
  virtual void imgldr_releaseSkinBitmap(ARGB32 *bmpbits);
  virtual void imgldr_retainSkinBitmap(ARGB32 *bmpbits);
  virtual COLORREF filterSkinColor(COLORREF, const char *elementid, const char *groupname);
  virtual void reapplySkinFilters();
  virtual int colortheme_getNumColorSets();
  virtual const char *colortheme_enumColorSet(int n);
  virtual int colortheme_getNumColorGroups(const char *colorset);
  virtual const char *colortheme_enumColorGroupName(const char *colorset, int n);
  virtual ColorThemeGroup *colortheme_enumColorGroup(int colorset, int colorgroup);
  virtual ColorThemeGroup *colortheme_getColorGroup(const char *colorset, const char *colorgroup);
  virtual void colortheme_setColorSet(const char *colorset);
  virtual const char *colortheme_getColorSet();
  virtual void colortheme_newColorSet(const char *set);
  virtual void colortheme_updateColorSet(const char *set);
  virtual int paintset_present(int set);
  virtual void paintset_render(int set, CanvasBase *c, const RECT *r, int alpha=255);
  virtual void paintset_renderTitle(char *t, CanvasBase *c, const RECT *r, int alpha=255);
  virtual void drawAnimatedRects(const RECT *r1, const RECT *r2);
  virtual void sendDdeCommand(const char *application, const char *command, DWORD minimumDelay);
  virtual int getNumComponents();
  virtual GUID getComponentGUID(int c);
  virtual const char *getComponentName(GUID componentGuid);
  virtual CfgItem *compon_getCfgInterface(GUID componentGuid);
  virtual int comp_notifyScripts(const char *s, int i1, int i2);
  virtual int autopopup_registerGuid(GUID g, const char *desc, const char *prefered_container=NULL, int container_flag=0);
  virtual int autopopup_registerGroupId(const char *groupid, const char *desc, const char *prefered_container=NULL, int container_flag=0);
  virtual void autopopup_unregister(int id);
  virtual int skinwnd_toggleByGuid(GUID g, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0);
  virtual int skinwnd_toggleByGroupId(const char *groupid, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0);
  virtual RootWnd *skinwnd_createByGuid(GUID g, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0, int starthidden=0, int *isnew = NULL);
  virtual RootWnd *skinwnd_createByGroupId(const char *groupid, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0, int starthidden=0, int *isnew = NULL);
  virtual void skinwnd_destroy(RootWnd *w, RECT *destanimrect=NULL);
  virtual int skinwnd_getNumByGuid(GUID g);
  virtual RootWnd *skinwnd_enumByGuid(GUID g, int n);
  virtual int skinwnd_getNumByGroupId(const char *groupid);
  virtual RootWnd *skinwnd_enumByGroupId(const char *groupid, int n);
  virtual void skinwnd_attachToSkin(RootWnd *w, int side, int size);
  virtual ScriptObject *skin_getContainer(const char *container_name);
  virtual ScriptObject *skin_getLayout(ScriptObject *container, const char *layout_name);
  virtual int loadSkinFile(const char *xmlfile);
  virtual void unloadSkinPart(int skinpart);


};


#endif