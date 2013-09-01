#ifndef XUIOBJECT_H
#define XUIOBJECT_H

#pragma warning(disable: 4800)

#include <wnd/basewnd.h>
#include <wnd/virtualwnd.h>
#include <api/service/svcs/svc_xuiobject.h>
#include <api/xui/xuiwnd.h>
#include "../xuiattribute.h"
#include "../xuiutils.h"
#include "../xuielement.h"
#include "sourcesink/XUIObjectEventSource.h"
#include "js/JSXUIObject.h"
#include "../xuiattrfw.h"
#include "../xuidndsession.h"

// TODO: DISPATCH !

//<?><pset id="jsnoevent" value="false"/>

class DefinitionsFramework;
class Bitmap;
class Blender;
class svc_xuiObject;
class XUIScript;
class FindElementCallback;
class DOMDocumentFragment;
class DOMNode;

class XUIObject : public XUIElement,
                  public JSScriptable,
                  public XUIObjectEventSource,
                  public JSXUIObject {
public:
	XUIObject();
	virtual ~XUIObject();

  svc_xuiObject *getOwnerService() { return m_ownerService; }
  void setOwnerService(svc_xuiObject *svc) { m_ownerService = svc; }
  virtual int getElementType() { return XUIELEMENT_XUIOBJECT; }

  // This takes a BaseWnd on purpose, it is not meant to be dispatched, ever.
  void setBaseWnd(BaseWnd *w) { m_wnd = w; onSetWindow(); }

  RootWnd *getRootWnd() { return m_wnd; }
  virtual void onSetWindow() {};
  void setFramework(DefinitionsFramework *framework) { m_framework = framework; }
  DefinitionsFramework *getFramework() { return m_framework; }

  SCRIPT virtual void setAttribute(const char *attributeName, const char *attributeValue);
  SCRIPT const char *getAttribute(const char *attributeName);
  SCRIPT int getAttributeInt(const char *attributeName, int defaultValue=0);
  SCRIPT bool getAttributeBool(const char *attributeName, bool defaultValue=0);
  SCRIPT bool hasAttribute(const char *attributeName);
  SCRIPT int getNumAttributes();
  SCRIPT const char *enumAttributeName(int i);
  SCRIPT const char *enumAttributeValue(int i);
  // todo: add removeAttribute, make it work with flex

  SCRIPT void addChild(XUIObject *child, XUIObject *insertBefore=NULL);
  SCRIPT void removeChild(XUIObject *child);
  SCRIPT void destroyChild(XUIObject *child);
  SCRIPT void destroyChildren(PtrList<XUIObject> *children);
  SCRIPT XUIObject *createObject(const char *tagname);
  SCRIPT PtrList<XUIObject> &getChildren() { return m_children; }
  SCRIPT int getNumChildren();
  SCRIPT XUIObject *enumChild(int i);
  SCRIPT XUIObject *getLastChild();
  SCRIPT XUIObject *getFirstChild();
  SCRIPT XUIObject *getParent();
  SCRIPT const char *getTagName();
  void setTagName(const char *tagname);
  SCRIPT void bringToFront() { m_wnd->bringToFront(); }
  SCRIPT void sendToBack() { m_wnd->bringToBack(); }
  SCRIPT void focus() { m_wnd->setFocus(); }
  SCRIPT bool isFocused() { return m_wnd->gotFocus(); }
  SCRIPT bool pointInRect(int x, int y);
  SCRIPT bool pointInRegion(int x, int y);
  SCRIPT void invalidate() { m_wnd->invalidate(); }
  SCRIPT int getX() { return m_wnd->clientRect().left; }
  SCRIPT int getY() { return m_wnd->clientRect().left; }
  SCRIPT int getWidth() { RECT r = m_wnd->clientRect(); return r.right - r.left; }
  SCRIPT int getHeight() { RECT r = m_wnd->clientRect(); return r.bottom - r.top; }
  SCRIPT void beginMouseCapture();
  SCRIPT void endMouseCapture();
  SCRIPT XUIObject *getElementById(const char *id, bool local=false);
  SCRIPT XUIObject *getElementByTagName(const char *tagname, bool local=false);
  SCRIPT XUIObject *getElementByAttribute(const char *attrName, const char *attrValue, bool local=false);
  SCRIPT PtrList<XUIObject> *getElementsById(const char *id, bool local=false);
  SCRIPT PtrList<XUIObject> *getElementsByTagName(const char *tag, bool local=false);
  SCRIPT PtrList<XUIObject> *getElementsByAttribute(const char *attrName, const char *attrValue, bool local=false);
  XUIObject *findElement(FindElementCallback *cb, bool local=false);
  void findElements(FindElementCallback *cb, PtrList<XUIObject> *result, bool local=false);
  SCRIPT bool isMouseIn() { return m_mouseInArea; }

  SCRIPT EVENT virtual void onAttributeChanged(const char *attributeName, const char *attributeValue);
  virtual void updateCoordinates();

  SCRIPT bool isDragging() { return m_dndDragging; }
  SCRIPT EVENT void onBeginDrag(XUIDndSession *session);
  SCRIPT EVENT void onEndDrag(XUIDndSession *session, bool completed);
  SCRIPT EVENT void onDragEnter(XUIDndSession *dndSession) { eventSource_onDragEnter(dndSession); }
  SCRIPT EVENT void onDragLeave(XUIDndSession *dndSession) { eventSource_onDragLeave(dndSession); }
  SCRIPT EVENT void onDragOver(XUIDndSession *dndSession, int x, int y) { eventSource_onDragOver(dndSession, x, y); }
  SCRIPT EVENT void onDragDrop(XUIDndSession *dndSession, int x, int y) { eventSource_onDragDrop(dndSession, x, y); }

  // from BaseWnd
  /* noscript */ void dragEnter(RootWnd *dragSource);
  /* noscript */ void dragLeave(RootWnd *dragSource);
  /* noscript */ int dragOver(RootWnd *dragSource, int x, int y);
  /* noscript */ void dragDrop(RootWnd *dragSource, int x, int y);
  /* noscript */ void dragComplete(int);
  /* noscript */ void addDragObject(const char *type, void *obj);

  SCRIPT EVENT virtual void onInit();
  SCRIPT EVENT virtual void onDestroy();
  SCRIPT EVENT virtual void onResize();
  SCRIPT EVENT virtual void onLeftButtonDown(int x, int y);
  SCRIPT EVENT virtual void onLeftButtonUp(int x, int y);
  SCRIPT EVENT virtual void onRightButtonDown(int x, int y);
  SCRIPT EVENT virtual void onRightButtonUp(int x, int y);
  SCRIPT EVENT virtual void onMiddleButtonDown(int x, int y);
  SCRIPT EVENT virtual void onMiddleButtonUp(int x, int y);
  SCRIPT EVENT virtual void onMouseWheelDown(int click, int lines);
  SCRIPT EVENT virtual void onMouseWheelUp(int click, int lines);
  SCRIPT EVENT virtual void onMouseMove(int x, int y);
  SCRIPT EVENT virtual void onMouseEnter() { eventSource_onMouseEnter(); }
  SCRIPT EVENT virtual void onMouseLeave() { eventSource_onMouseLeave(); }
  SCRIPT EVENT virtual void onGetFocus() { eventSource_onGetFocus(); invalidate(); }
  SCRIPT EVENT virtual void onLoseFocus() { eventSource_onLoseFocus(); invalidate(); }
  SCRIPT EVENT virtual void onShow() { eventSource_onShow(); }
  SCRIPT EVENT virtual void onHide() { eventSource_onHide(); }
  SCRIPT EVENT virtual void onActivate() { eventSource_onActivate(); }
  SCRIPT EVENT virtual void onDeactivate() { eventSource_onDeactivate(); }
  SCRIPT EVENT virtual void onEnable() { eventSource_onEnable(); }
  SCRIPT EVENT virtual void onDisable() { eventSource_onDisable(); }
  SCRIPT EVENT virtual void onChar(unsigned int c) { eventSource_onChar(c); }
  SCRIPT EVENT virtual void onKeyDown(unsigned int keyCode) { eventSource_onKeyDown(keyCode); }
  SCRIPT EVENT virtual void onKeyUp(unsigned int keyCode) { eventSource_onKeyUp(keyCode); }
  SCRIPT EVENT virtual void onMinimize() { eventSource_onMinimize(); }
  SCRIPT EVENT virtual void onRestore() { eventSource_onRestore(); }
  SCRIPT EVENT virtual void onMaximize() { eventSource_onMaximize(); }
  SCRIPT EVENT virtual void onCancelCapture() { eventSource_onCancelCapture(); }

  int getAttributeX() { return m_x; }
  int getAttributeY() { return m_y; }
  int getAttributeW() { return m_w; }
  int getAttributeH() { return m_h; }
  int getAttributeMinW() { return m_minw; }
  int getAttributeMinH() { return m_minh; }
  int getAttributeMaxW() { return m_maxw; }
  int getAttributeMaxH() { return m_maxh; }

  // called by the parser for top level windows, return 1 to override init
  virtual int customTopLevelInit(RootWnd *parentWindow) { return 0; }

  void setBackgroundImage(const char *image);
  virtual void onPaint(Canvas *c);
  int getPreferences(int which);
  int isMouseThrough();
  int isDndThrough();
  void addAttribForwarding(XUIObject *source, const char *sourceattr, const char *mappedattr);
  void invalidateDefaultSizeCache();
  void reflow();
  int getFlex() { return m_flex; }
  
  void registerScript(XUIScript *script);
  int getNumScripts();
  XUIScript *enumScript(int n);

  void initChild(XUIObject *xo);

  int isInstantiationElement() { return m_isInstantiationElement; }
  void setInstantiationElement() { m_isInstantiationElement = true; }
  
  void registerLocalGroupDefinition(DOMNode *node);
  DOMNode *getLocalGroupDefinition(const char *id);

  enum {
    XUI_BGSTYLE_NONE   = 0,
    XUI_BGSTYLE_FLAT   = 1,
    XUI_BGSTYLE_SUNKEN = 2,
    XUI_BGSTYLE_RAISED = 3,
  };

  virtual int wantFocus() { return m_wantFocus; }
  enum {
    FLEXBOX_ORIENTATION_HORIZONTAL = 0,
    FLEXBOX_ORIENTATION_VERTICAL   = 1
  };
  enum {
    XUI_DEFAULTSIZE_WIDTH = 0,
    XUI_DEFAULTSIZE_HEIGHT = 1
  };


protected:
  int m_flex_orientation;
  int getDefaultSize(int which);

  BaseWnd *m_wnd;
  int m_mouseInArea;

private:

  void beginCapture(int captureType);
  void endCapture(int captureType);

  int m_x,m_y,m_w,m_h,m_maxw,m_maxh,m_minw,m_minh;

  String m_id;

  XUIAttributeList m_attributes;
  PtrList<XUIObject> m_children;
  XUIObject *m_parent;
  void setParent(XUIObject *parent);

  int m_draganchor_x, m_draganchor_y;
  int m_draggingWindow;
  String m_tagname;
  int m_captureTypes;
  Bitmap *m_image;
  DefinitionsFramework *m_framework;
  int m_imagealpha;
  int m_disabledimagealpha;
  Blender *m_blender;
  COLORREF m_bgcolor;
  int m_bgstyle;
  COLORREF m_highlightcolor;
  COLORREF m_shadowcolor;
  COLORREF m_outlinecolor;
  int m_outlinesize[4];
  int m_wantFocus;
  int m_mousethrough;
  svc_xuiObject *m_ownerService;
  int m_focusrectmargin[4];
  PtrList<XUIAttribForwarder> m_attrforwarders;
  int m_defaultsize_invalid[2];
  int m_defaultsize_cached[2];
  int m_flex;
  PtrList<XUIScript> m_scripts;
  int m_isInstantiationElement;

  class GroupDefById : public Map<String, PtrList<DOMNode> *, StringComparator2> {
  };
  PtrList<DOMDocumentFragment> m_groupdefsFragments;
  GroupDefById m_groupdefs;
  COLORREF m_debugColor;
  PtrList<XUIObject> m_xuiobjects_returnlist;

  enum {
    CAPTURETYPE_USER = 0,
    CAPTURETYPE_AREA = 1,
    CAPTURETYPE_DND  = 2,
  };

  bool m_dndDragging;
  bool m_dndlbuttondown;
  bool m_trieddnd;
};

#include "findelement.h"

MAKE_XUISERVICE_CLASS(XUISpacerSvc, VirtualWnd, XUIObject, "spacer");
MAKE_XUISERVICE_CLASS(XUIBoxSvc, VirtualWnd, XUIObject, "box");
MAKE_XUISERVICE_CLASS(XUIGenericContainerSvc, VirtualWnd, XUIObject, "#genericContainer");

/*

Flex rules for the flex axis :

  - children with flex and no x,y,w,h :
    - all children flex values are added, then parent client size is divided by total, 
      then this value is multiplied by the child flex value to determine its size. each
      child is also moved to follow the previous child.
  - child with flex and no w,h, but with x,y :
    - child gets size according to flex rule, and is moved to specified x,y
  - child with flex and no x,y, but with w,h :
    - child is auto moved to follow the previous auto moved child, but flex value is ignored
  - child with flex and x,y,w,h :
    - child is moved to specified x,y and sized with specified w,h. flex value is ignored
  - child with no flex param, and no x,y,w,h :
    - child gets default size, and is auto moved
  - child with no flex param, no x,y, but with w,h :
    - child is sized using specified values, and is auto moved
  - child with no flex param, no w,h, but with x,y :
    - child gets sized according to flex rule, and is moved to specified x,y
  
  - default size is computed from content + margin, and min/max is applied
  - if a child has no default size (explicit, min, or inherent), size is set to 0
  - when setting size parameters, min/max are applied on top of w,h, so setting size>maxsize sets maxsize

  - using flex="*" sets the size to the parent size, minus the x/y position (the child is extended to the right/bottom parent edge)


Flex rules on the other axis: 

  - if a child has no x (for vbox) or y (for hbox), 0 is used
  - unless a child has a specific size or max size set, size is set to parent size

*/

#endif
