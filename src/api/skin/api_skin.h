#ifndef __API_SKIN_H
#define __API_SKIN_H

#include <bfc/dispatch.h>
#include <bfc/common.h>

class RootWnd;
class CfgItem;
class ScriptObject;
class ColorThemeGroup;
class CanvasBase;

class api_skin: public Dispatchable {
  protected:
    api_skin() {}
    ~api_skin() {}
  public:
// skinning
  COLORREF skin_getColorElement(char *type, const char **color_grp=NULL);
  COLORREF *skin_getColorElementRef(char *type, const char **color_grp=NULL);
  int *skin_getIterator();  // an int that cycles on skin reloading
  void setSkinPathOverride(const char *path); // UNSUPPORTED IN MONOLITHIC API
  void skin_setLockUI(int l);
  int skin_getLockUI();
  double skin_getVersion();
  COLORREF skin_getBitmapColor(const char *bitmapid);

#define RenderBaseTexture renderBaseTexture //CUT

  void skin_renderBaseTexture(RootWnd *base, CanvasBase *c, RECT *r, RootWnd *destWnd, int alpha=255);
  void skin_registerBaseTextureWindow(RootWnd *window, const char *bmp=NULL);
  void skin_unregisterBaseTextureWindow(RootWnd *window);

  /**
    Switch to a specific skin using the skin
    name.

    @param skin_name Name of the skin to load.
  */
  void skin_switchSkin(char *skin_name);

  /**
    Get the name of the currently loaded
    skin.

    @ret Name of the currently loaded skin.
  */
  const char *getSkinName();

  /**
    Get the path of the currently loaded
    skin.

    @ret Path of the currently loaded skin.
  */
  const char *getSkinPath();

  /**
    Get the path containing all skins subdirectories

    @ret Skins root path
  */
  const char *getSkinsPath();


  /**
    Get the path to the default application
    skin.

    @ret Path to the default application skin.
  */
  const char *getDefaultSkinPath();

// generic malloc/free. Use these if you are returning data that other
// components will need to free (possibly after you have been unloaded)
  /**
    Allocate a block of memory specified in bytes from the system heap.

    NOTE: Please DO NOT just use this instead of malloc(). Only use it
    to allocate RAM that another component will free with sysFree().

    @see sysFree()
    @see sysRealloc()
    @see SysMemCallbackI
    @ret Pointer to the allocated memory block or NULL on failure.
    @param size Requested size of the memory block (in bytes).
  */
  void *sysMalloc(int size);

  /**
    Free a block of memory previously allocated from the system heap.

    NOTE: Please DO NOT just use this instead of free(). Only call it with
    memory allocated via sysMalloc().

    @see sysMalloc()
    @see sysRealloc()
    @see SysMemCallbackI
    @param ptr Pointer to the memory block to free.
  */
  void sysFree(void *ptr);

  /**
    Allocate supplementary memory to a previously allocated
    memory block. This will take care of copying the data to
    the newly allocated memory block.

    @see SysMemCallbackI
    @ret Pointer to the reallocated memory block or NULL on failure.
    @param ptr      Pointer to the memory block to resize.
    @param newsize  New size of the memory block (in bytes).
  */
  void *sysRealloc(void *ptr, int newsize);

  /**
    Notify the system that a sysMem block has changed. This will cause
    a SysMemCallback to be sent.
    @see SysMemCallbackI
    @see sysRealloc()
    @param ptr The pointer to the memory block whose contents has changed.
  */
  void sysMemChanged(void *ptr);

// image loading
  ARGB32 *imgldr_makeBmp(const char *filename, int *has_alpha, int *w, int *h);
  ARGB32 *imgldr_makeBmp(OSMODULEHANDLE hInst, int id, int *has_alpha, int *w, int *h, const char *colorgroup=NULL);
  ARGB32 *imgldr_requestSkinBitmap(const char *file, int *has_alpha, int *x, int *y, int *subw, int *subh, int *w, int *h, int cached);
  RegionServer *imgldr_requestSkinRegion(const char *id);
  void imgldr_cacheSkinRegion(const char *id, Region *r);
  void imgldr_releaseSkinBitmap(ARGB32 *bmpbits);
  void imgldr_retainSkinBitmap(ARGB32 *bmpbits);

  COLORREF filterSkinColor(COLORREF, const char *elementid, const char *groupname);
  void reapplySkinFilters();

  /**
    Get the number of color themes available for
    the currently loaded skin.

    @ret Number of color themes.
  */
  int colortheme_getNumColorSets();

  /**
    Enumerate the names of the color themes
    available for the currently loaded skin.
    This is done using the color themes index
    number, the index start at 0.

    @ret Name of the color theme.
    @param n Index number of the color theme.
  */
  const char *colortheme_enumColorSet(int n);

  /**
    Get the number of color groups present in
    the color theme.

    @ret Number of color groups.
    @param colorset Name of the color theme.
  */
  int colortheme_getNumColorGroups(const char *colorset);

  /**
    Enumerate the names of the color groups
    present in the color theme. This is done using
    the color groups index number, the index starts
    at 0.

    @ret Name of the color group.
    @param colorset Name of the color theme.
    @param n Index number of the color group.
  */
  const char *colortheme_enumColorGroupName(const char *colorset, int n);

  /**
    Enumerate the color theme groups.

    @ret A ColorThemeGroup object.
    @param colorset the index of the color set.
    @param colorgroup the index of the clor group.
  */
  ColorThemeGroup *colortheme_enumColorGroup(int colorset, int colorgroup);
  ColorThemeGroup *colortheme_getColorGroup(const char *colorset, const char *colorgroup);

  /**
    Apply a color theme to the currently loaded
    skin.

    @param colorset Name of the color theme to use.
  */
  void colortheme_setColorSet(const char *colorset);

  /**
    Get the name of the currently applied
    color set.

    @ret Name of the currently applied color set.
  */
  const char *colortheme_getColorSet();
  void colortheme_newColorSet(const char *set);
  void colortheme_updateColorSet(const char *set);

  int paintset_present(int set);
  void paintset_render(int set, CanvasBase *c, const RECT *r, int alpha=255);
  void paintset_renderTitle(char *t, CanvasBase *c, const RECT *r, int alpha=255);

// animated rects
  void drawAnimatedRects(const RECT *r1, const RECT *r2);

// Dde com
  /**
    Send a DDE (Dynamic Data Exchange) message to another
    application. This is Win32 specific and NOT PORTABLE.

    @param application  Name of the application.
    @param command      Command to send.
    @param minimumDelay Minimum delay between commands.
  */
  void sendDdeCommand(const char *application, const char *command, DWORD minimumDelay);

// Components
  /**
    Get the number of loaded components.

    @ret Number of loaded components.
  */
  int getNumComponents();

  /**
    Get the GUID of a component based on it's
    loading index number.

    The loading index number is determined simply
    by the order in which the components were loaded.
    Index starts at 0.

    @see GUID
    @ret GUID of the requested component or 0 if the index is out of bounds.
    @param c Index number of the component.
  */
  GUID getComponentGUID(int c);

  /**
    Get the name of the component using it's
    GUID.

    @see GUID
    @ret Name of the component.
    @param componentGuid GUID of the component.
  */
  const char *getComponentName(GUID componentGuid);

  /**
    Get the configuration item registered
    by the component using the component's
    GUID.

    @see GUID
    @ret Configuration item registered by the component.
    @param componentGuid GUID of the component.
  */
  CfgItem *compon_getCfgInterface(GUID componentGuid);

  /**
  */
  int comp_notifyScripts(const char *s, int i1, int i2);

  /**
  */
  int autopopup_registerGuid(GUID g, const char *desc, const char *prefered_container=NULL, int container_flag=0);

  /**
  */
  int autopopup_registerGroupId(const char *groupid, const char *desc, const char *prefered_container=NULL, int container_flag=0);

  /**
  */
  void autopopup_unregister(int id);

  int skinwnd_toggleByGuid(GUID g, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0);
  int skinwnd_toggleByGroupId(const char *groupid, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0);
  RootWnd *skinwnd_createByGuid(GUID g, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0, int starthidden=0, int *isnew = NULL);
  RootWnd *skinwnd_createByGroupId(const char *groupid, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0, int starthidden=0, int *isnew = NULL);
  void skinwnd_destroy(RootWnd *w, RECT *destanimrect=NULL);
  int skinwnd_getNumByGuid(GUID g);
  RootWnd *skinwnd_enumByGuid(GUID g, int n);
  int skinwnd_getNumByGroupId(const char *groupid);
  RootWnd *skinwnd_enumByGroupId(const char *groupid, int n);
  void skinwnd_attachToSkin(RootWnd *w, int side, int size);

  ScriptObject *skin_getContainer(const char *container_name);
  ScriptObject *skin_getLayout(ScriptObject *container, const char *layout_name);

// loads a skin file, but does not switch main skin.
// use this to load a custom xml skin/script/definition file or package (using includes)
// either before (if you want to allow the content to be overriden) or after
// skin loading
// return value is what you need to give to unloadSkinPart() to clean up your mess
  int loadSkinFile(const char *xmlfile);
  void unloadSkinPart(int skinpart);
  
  protected:
    enum {
    API_SKIN_SKIN_GETELEMENTCOLOR2 = 300,
    API_SKIN_SKIN_GETELEMENTREF2 = 310,
    API_SKIN_SKIN_GETBITMAPCOLOR = 315,
    API_SKIN_SKIN_GETITERATOR = 320,
    API_SKIN_SKIN_SETSKINPATHOVERRIDE = 325,
    API_SKIN_SKIN_RENDERBASETEXTURE = 330,
    API_SKIN_SKIN_REGISTERBASETEXTUREWINDOW = 340,
    API_SKIN_SKIN_UNREGISTERBASETEXTUREWINDOW = 350,
    API_SKIN_SKIN_SWITCHSKIN = 360,
    API_SKIN_SKIN_GETVERSION = 365,
    API_SKIN_SETLOCKUI = 370,
    API_SKIN_GETLOCKUI = 380,
    API_SKIN_GETSKINNAME = 400,
    API_SKIN_GETSKINPATH = 410,
    API_SKIN_GETSKINSPATH = 415,
    API_SKIN_GETDEFAULTSKINPATH = 420,
    API_SKIN_PATH_GETAPPPATH = 430,
    API_SKIN_IMGLDR_MAKEBMP = 500,
    API_SKIN_OLDIMGLDR_MAKEBMP2 = 510, // DEPRECATED, CUT
    API_SKIN_IMGLDR_MAKEBMP2 = 511,
    API_SKIN_IMGLDR_REQUESTSKINBITMAP = 520,
    API_SKIN_IMGLDR_RELEASESKINBITMAP = 530,
    API_SKIN_IMGLDR_RETAINSKINBITMAP = 535,
    API_SKIN_IMGLDR_REQUESTSKINREGION = 540,
    API_SKIN_IMGLDR_CACHESKINREGION = 550,
    API_SKIN_REGISTERSKINFILTER = 600,
    API_SKIN_UNREGISTERSKINFILTER = 610,
    API_SKIN_REAPPLYSKINFILTERS = 620,
    API_SKIN_FILTERSKINCOLOR = 630,
    API_SKIN_GETNUMCOLORSETS = 650,
    API_SKIN_ENUMCOLORSET = 660,
    API_SKIN_GETNUMCOLORGROUPS = 670,
    API_SKIN_ENUMCOLORGROUP = 680,
    API_SKIN_ENUMCOLORGROUPNAME = 681,
    API_SKIN_GETCOLORGROUP = 682,
    API_SKIN_SETCOLORSET = 690,
    API_SKIN_GETCOLORSET = 695,
    API_SKIN_NEWCOLORSET = 696,
    API_SKIN_UPDATECOLORSET = 697,
    API_SKIN_PAINTSET_RENDER = 700,
    API_SKIN_PAINTSET_RENDERTITLE = 710,
    API_SKIN_PAINTSET_PRESENT = 720,
    API_SKIN_DRAWANIMATEDRECTS = 800,
    API_SKIN_SENDDDECOMMAND = 900,
    };

};

inline COLORREF api_skin::skin_getColorElement(char *type, const char **color_grp) {
  return _call( API_SKIN_SKIN_GETELEMENTCOLOR2, (COLORREF)0, type, color_grp );
}

inline COLORREF *api_skin::skin_getColorElementRef(char *type, const char **color_grp) {
  return _call( API_SKIN_SKIN_GETELEMENTREF2, (COLORREF *)0, type, color_grp );
}

inline int *api_skin::skin_getIterator() {
  return _call( API_SKIN_SKIN_GETITERATOR, (int *)0);
}

inline void api_skin::skin_renderBaseTexture(RootWnd *base, CanvasBase *c, RECT *r, RootWnd *destWnd, int alpha) {
  _voidcall( API_SKIN_SKIN_RENDERBASETEXTURE, base, c, r, destWnd, alpha );
}

inline void api_skin::skin_registerBaseTextureWindow(RootWnd *window, const char *bmp) {
  _voidcall( API_SKIN_SKIN_REGISTERBASETEXTUREWINDOW, window, bmp);
}

inline void api_skin::skin_unregisterBaseTextureWindow(RootWnd *window) {
  _voidcall( API_SKIN_SKIN_UNREGISTERBASETEXTUREWINDOW, window );
}

inline void api_skin::skin_switchSkin(char *skin_name) {
  _voidcall( API_SKIN_SKIN_SWITCHSKIN, skin_name );
}

inline double api_skin::skin_getVersion() {
  return _call( API_SKIN_SKIN_GETVERSION, (double)0);
}


inline const char *api_skin::getSkinName() {
  return _call( API_SKIN_GETSKINNAME, ""  );
}

inline int api_skin::skin_getLockUI() {
  return _call( API_SKIN_GETLOCKUI, (int)0  );
}

inline const char *api_skin::getSkinPath() {
  return _call( API_SKIN_GETSKINPATH, ""  );
}

inline const char *api_skin::getDefaultSkinPath() {
  return _call( API_SKIN_GETDEFAULTSKINPATH, ""  );
}

/*
inline const char *api_skin::getAppPath() {
  return _call( API_SKIN_PATH_GETAPPPATH, ""  );
}
*/

inline ARGB32 *api_skin::imgldr_makeBmp(const char *filename, int *has_alpha, int *w, int *h) {
  return _call( API_SKIN_IMGLDR_MAKEBMP, (ARGB32 *)0, filename, has_alpha, w, h );
}

inline ARGB32 *api_skin::imgldr_makeBmp(HINSTANCE hInst, int id, int *has_alpha, int *w, int *h, const char *colorgroup) {
  return _call( API_SKIN_IMGLDR_MAKEBMP2, (ARGB32 *)0, hInst, id, has_alpha, w, h, colorgroup);
}

inline ARGB32 *api_skin::imgldr_requestSkinBitmap(const char *file, int *has_alpha, int *x, int *y, int *subw, int *subh, int *w, int *h, int cached) {
  return _call( API_SKIN_IMGLDR_REQUESTSKINBITMAP, (ARGB32 *)0, file, has_alpha, x, y, subw, subh, w, h, cached );
}

inline void api_skin::imgldr_releaseSkinBitmap(ARGB32 *bmpbits) {
  _voidcall( API_SKIN_IMGLDR_RELEASESKINBITMAP, bmpbits );
}

inline void api_skin::imgldr_retainSkinBitmap(ARGB32 *bmpbits) {
  _voidcall( API_SKIN_IMGLDR_RETAINSKINBITMAP, bmpbits );
}

inline RegionServer *api_skin::imgldr_requestSkinRegion(const char *id){
  return _call(API_SKIN_IMGLDR_REQUESTSKINREGION, (RegionServer *)NULL, id);
}

inline COLORREF api_skin::filterSkinColor(COLORREF color, const char *elementid, const char *groupname) {
  return _call( API_SKIN_FILTERSKINCOLOR, (COLORREF)0, color, elementid, groupname );
}

inline void api_skin::reapplySkinFilters() {
  _voidcall( API_SKIN_REAPPLYSKINFILTERS  );
}

inline int api_skin::colortheme_getNumColorSets() {
  return _call(API_SKIN_GETNUMCOLORSETS, 0);
}

inline const char *api_skin::colortheme_enumColorSet(int n) {
  return _call(API_SKIN_ENUMCOLORSET, (const char *)NULL, n);
}

inline int api_skin::colortheme_getNumColorGroups(const char *colorset) {
  return _call(API_SKIN_GETNUMCOLORGROUPS, 0, colorset);
}

inline ColorThemeGroup *api_skin::colortheme_enumColorGroup(int colorset, int colorgroup) {
  return _call(API_SKIN_ENUMCOLORGROUP, (ColorThemeGroup *)NULL, colorset, colorgroup);
}

inline const char *api_skin::colortheme_enumColorGroupName(const char *colorset, int n) {
  return _call(API_SKIN_ENUMCOLORGROUPNAME, (const char *)NULL, colorset, n);
}

inline ColorThemeGroup *api_skin::colortheme_getColorGroup(const char *colorset, const char *colorgroup) {
  return _call(API_SKIN_GETCOLORGROUP, (ColorThemeGroup *)NULL, colorset, colorgroup);
}

inline void api_skin::colortheme_setColorSet(const char *colorset) {
  _voidcall(API_SKIN_SETCOLORSET, colorset);
}

inline const char *api_skin::colortheme_getColorSet() {
  return _call(API_SKIN_GETCOLORSET, (const char *)NULL);
}

inline void api_skin::paintset_render(int set, CanvasBase *c, const RECT *r, int alpha) {
  _voidcall( API_SKIN_PAINTSET_RENDER, set, c,  r, alpha);
}

inline void api_skin::paintset_renderTitle(char *t, CanvasBase *c, const RECT *r, int alpha) {
  _voidcall( API_SKIN_PAINTSET_RENDERTITLE, t, c,  r, alpha);
}

inline void api_skin::drawAnimatedRects(const RECT *r1, const RECT *r2) {
  _voidcall( API_SKIN_DRAWANIMATEDRECTS,  r1,  r2 );
}

inline void api_skin::sendDdeCommand(const char *application, const char *command, DWORD minimumDelay) {
  _voidcall( API_SKIN_SENDDDECOMMAND, application, command, minimumDelay );
}

class api_skinI: public api_skin {
  public:
  virtual COLORREF skin_getColorElement(char *type, const char **color_grp=NULL)=0;
  virtual COLORREF *skin_getColorElementRef(char *type, const char **color_grp=NULL)=0;
  virtual int *skin_getIterator()=0;  
  virtual void setSkinPathOverride(const char *path)=0;
  virtual void skin_setLockUI(int l)=0;
  virtual int skin_getLockUI()=0;
  virtual double skin_getVersion()=0;
  virtual COLORREF skin_getBitmapColor(const char *bitmapid)=0;
  virtual void skin_renderBaseTexture(RootWnd *base, CanvasBase *c, RECT *r, RootWnd *destWnd, int alpha=255)=0;
  virtual void skin_registerBaseTextureWindow(RootWnd *window, const char *bmp=NULL)=0;
  virtual void skin_unregisterBaseTextureWindow(RootWnd *window)=0;
  virtual void skin_switchSkin(char *skin_name)=0;
  virtual const char *getSkinName()=0;
  virtual const char *getSkinPath()=0;
  virtual const char *getSkinsPath()=0;
  virtual const char *getDefaultSkinPath()=0;
  virtual ARGB32 *imgldr_makeBmp(const char *filename, int *has_alpha, int *w, int *h)=0;
  //virtual ARGB32 *imgldr_makeBmp(OSMODULEHANDLE hInst, int id, int *has_alpha, int *w, int *h, const char *colorgroup=NULL)=0;
  virtual ARGB32 *imgldr_makeBmp2(OSMODULEHANDLE hInst, int id, int *has_alpha, int *w, int *h, const char *colorgroup=NULL);
  virtual ARGB32 *imgldr_requestSkinBitmap(const char *file, int *has_alpha, int *x, int *y, int *subw, int *subh, int *w, int *h, int cached)=0;
  virtual RegionServer *imgldr_requestSkinRegion(const char *id)=0;
  virtual void imgldr_cacheSkinRegion(const char *id, Region *r)=0;
  virtual void imgldr_releaseSkinBitmap(ARGB32 *bmpbits)=0;
  virtual void imgldr_retainSkinBitmap(ARGB32 *bmpbits)=0;
  virtual COLORREF filterSkinColor(COLORREF, const char *elementid, const char *groupname)=0;
  virtual void reapplySkinFilters()=0;
  virtual int colortheme_getNumColorSets()=0;
  virtual const char *colortheme_enumColorSet(int n)=0;
  virtual int colortheme_getNumColorGroups(const char *colorset)=0;
  virtual const char *colortheme_enumColorGroupName(const char *colorset, int n)=0;
  virtual ColorThemeGroup *colortheme_enumColorGroup(int colorset, int colorgroup)=0;
  virtual ColorThemeGroup *colortheme_getColorGroup(const char *colorset, const char *colorgroup)=0;
  virtual void colortheme_setColorSet(const char *colorset)=0;
  virtual const char *colortheme_getColorSet()=0;
  virtual void colortheme_newColorSet(const char *set)=0;
  virtual void colortheme_updateColorSet(const char *set)=0;
  virtual int paintset_present(int set)=0;
  virtual void paintset_render(int set, CanvasBase *c, const RECT *r, int alpha=255)=0;
  virtual void paintset_renderTitle(char *t, CanvasBase *c, const RECT *r, int alpha=255)=0;
  virtual void drawAnimatedRects(const RECT *r1, const RECT *r2)=0;
  virtual void sendDdeCommand(const char *application, const char *command, DWORD minimumDelay)=0;
  virtual int getNumComponents()=0;
  virtual GUID getComponentGUID(int c)=0;
  virtual const char *getComponentName(GUID componentGuid)=0;
  virtual CfgItem *compon_getCfgInterface(GUID componentGuid)=0;
  virtual int comp_notifyScripts(const char *s, int i1, int i2)=0;
  virtual int autopopup_registerGuid(GUID g, const char *desc, const char *prefered_container=NULL, int container_flag=0)=0;
  virtual int autopopup_registerGroupId(const char *groupid, const char *desc, const char *prefered_container=NULL, int container_flag=0)=0;
  virtual void autopopup_unregister(int id)=0;
  virtual int skinwnd_toggleByGuid(GUID g, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0)=0;
  virtual int skinwnd_toggleByGroupId(const char *groupid, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0)=0;
  virtual RootWnd *skinwnd_createByGuid(GUID g, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0, int starthidden=0, int *isnew = NULL)=0;
  virtual RootWnd *skinwnd_createByGroupId(const char *groupid, const char *prefered_container=NULL, int container_flag=0, RECT *sourceanimrect=NULL, int transcient=0, int starthidden=0, int *isnew = NULL)=0;
  virtual void skinwnd_destroy(RootWnd *w, RECT *destanimrect=NULL)=0;
  virtual int skinwnd_getNumByGuid(GUID g)=0;
  virtual RootWnd *skinwnd_enumByGuid(GUID g, int n)=0;
  virtual int skinwnd_getNumByGroupId(const char *groupid)=0;
  virtual RootWnd *skinwnd_enumByGroupId(const char *groupid, int n)=0;
  virtual void skinwnd_attachToSkin(RootWnd *w, int side, int size)=0;
  virtual ScriptObject *skin_getContainer(const char *container_name)=0;
  virtual ScriptObject *skin_getLayout(ScriptObject *container, const char *layout_name)=0;
  virtual int loadSkinFile(const char *xmlfile)=0;
  virtual void unloadSkinPart(int skinpart)=0;

  protected:
    RECVS_DISPATCH;
};

// {0119AC90-3464-4fca-BD70-389C6479BB89}
static const GUID skinApiServiceGuid = 
{ 0x119ac90, 0x3464, 0x4fca, { 0xbd, 0x70, 0x38, 0x9c, 0x64, 0x79, 0xbb, 0x89 } };

extern api_skin *skinApi;

#endif // __API_SYSCB_H
