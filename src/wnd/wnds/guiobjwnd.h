#ifndef __GUIOBJWND_H
#define __GUIOBJWND_H

#include "wnd/clickwnd.h"
#include "api/script/scriptobj.h"
#include "api/script/guiobject.h"
#include "api/script/rootobj.h"
#include "api/xml/xmlobject.h"
#include "api/service/svcs/svc_xuiobject.h"

// {E5760861-5489-4ffc-BE02-061D9DA6CD1B}
const GUID guiObjectWndGuid = 
{ 0xe5760861, 0x5489, 0x4ffc, { 0xbe, 0x2, 0x6, 0x1d, 0x9d, 0xa6, 0xcd, 0x1b } };

#define GUIOBJECTWND_PARENT ClickWnd

#define XUI_ATTRIBUTE_IMPLIED  XML_ATTRIBUTE_IMPLIED
#define XUI_ATTRIBUTE_REQUIRED XML_ATTRIBUTE_REQUIRED

class COMEXP GuiObjectWnd : public GUIOBJECTWND_PARENT, public RootObjectInstance, public XmlObjectI {
  public:
    GuiObjectWnd();
    virtual ~GuiObjectWnd();

    virtual int onReloadConfig() { return 1; }

    // XmlObject

    virtual int setXmlParamById(int xmlhandle, int attrid, const char *name, const char *value);
    virtual int onUnknownXmlParam(const char *param, const char *value); 
    virtual int newXuiHandle() { return newXmlHandle(); }

    // ClickWnd

    virtual int onRightButtonDown(int x, int y);
    virtual int onRightButtonUp(int x, int y);
    virtual int onLeftButtonDown(int x, int y);
    virtual int onLeftButtonUp(int x, int y);
    virtual int onMouseMove(int x, int y);
    virtual int onLeftButtonDblClk(int x, int y);
    virtual int onRightButtonDblClk(int x, int y);
    virtual int onResize();
    virtual int onActivate();
    virtual int onDeactivate();
    virtual void onEnterArea();
    virtual void onLeaveArea();
    virtual void onSetVisible(int show);
    virtual void onCancelCapture();
    virtual int dragDrop(RootWnd *sourceWnd, int x, int y);
    virtual int acceptExternalDrops() { return 1; }
    virtual int dragEnter(RootWnd *sourceWnd) { return 1; }
    virtual int dragOver(int x, int y, RootWnd *sourceWnd) { return 1; }
    virtual void *getInterface(GUID g);
    virtual int onAction(const char *action, const char *param, int x, int y, int p1, int p2, void *data, int datalen, RootWnd *source);
    virtual const char *getId();
    virtual int onAcceleratorEvent(const char *name);

    virtual void setContent(const char *groupid_orguid, int autoresizefromcontent=0);
    virtual void onNewContent() {}
    virtual void abstract_onNewContent();
    virtual int onUnknownXuiParam(const char *param, const char *value) { return 0; }

    virtual GuiObject *findObject(const char *object_id);
    virtual ScriptObject *findScriptObject(const char *object_id);
    virtual GuiObject *getContent() { return NULL;/*abstract_getContent(); */}
    virtual ScriptObject *getContentScriptObject() { return NULL;/*abstract_getContentScriptObject(); */}
    virtual RootWnd *getContentRootWnd() { return NULL;/*abstract_getContentRootWnd();*/ }

    // BaseWnd

    virtual int onPostOnInit();
    virtual int onChar(unsigned int c);
    virtual int onKeyDown(int vkcode);
    virtual int onKeyUp(int vkcode);

    virtual int onGetFocus();
    virtual int onKillFocus();

    // GuiObjectWnd

    GuiObject *getGuiObject();
    RootObject *getRootObject();
    int cfg_reentry;
  
    // XuiObject
    virtual int setXuiParam(int _xuihandle, int attrid, const char *name, const char *value);

  private:
    GuiObject *my_gui_object;
    int xuihandle;

  public:

  enum {
    GUIOBJECT_ID=0,
    GUIOBJECT_ALPHA,
    GUIOBJECT_ACTIVEALPHA,
    GUIOBJECT_INACTIVEALPHA,
    GUIOBJECT_SYSREGION,
    GUIOBJECT_RECTRGN,
    GUIOBJECT_TOOLTIP,
    GUIOBJECT_SYSMETRICSX,
    GUIOBJECT_SYSMETRICSY,
    GUIOBJECT_SYSMETRICSW,
    GUIOBJECT_SYSMETRICSH,
    GUIOBJECT_MOVE,
    GUIOBJECT_RENDERBASETEXTURE,
    GUIOBJECT_CFGATTR,
    GUIOBJECT_X,
    GUIOBJECT_Y,
    GUIOBJECT_W,
    GUIOBJECT_H,
    GUIOBJECT_VISIBLE,
    GUIOBJECT_RELATX,
    GUIOBJECT_RELATY,
    GUIOBJECT_RELATW,
    GUIOBJECT_RELATH,
    GUIOBJECT_DROPTARGET,
    GUIOBJECT_GHOST,
    GUIOBJECT_NOTIFY,
  };

};

template <class T, char XMLTAG[], char SVCNAME[]>
class XuiObjectSvc : public svc_xuiObjectI {
  public:
   
    int testTag(const char *xmltag) {
      if (STRCASEEQL(xmltag, XMLTAG)) return 1;
      return 0;
    }
    GuiObject *instantiate(const char *xmltag) {
      if (testTag(xmltag)) {
        T *obj = new T;
        ASSERT(obj != NULL);
        return obj->getGuiObject();
      }
      return NULL;
    }
  void destroy(GuiObject *g) {
    T *obj = static_cast<T *>(g->guiobject_getRootWnd());
    delete obj;
  }
  static const char *getServiceName() { return SVCNAME; }
};

template <class T>
class XuiObjectSvc2 : public svc_xuiObjectI {
  public:
    int testTag(const char *xmltag) {
      if (STRCASEEQL(xmltag, T::xuiobject_getXmlTag())) return 1;
      return 0;
    }
    GuiObject *instantiate(const char *xmltag) {
      if (testTag(xmltag)) {
        T *obj = new T;
        ASSERT(obj != NULL);
        return obj->getGuiObject();
      }
      return NULL;
    }
  void destroy(GuiObject *g) {
    T *obj = static_cast<T *>(g->guiobject_getRootWnd());
    delete obj;
  }
  static const char *getServiceName() { return "Xui Object Service"; }
};

#endif
