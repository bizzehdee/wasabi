#ifndef XUIINSTANTIATOR_H
#define XUIINSTANTIATOR_H

#include <api/wdf/dominstantiator.h>
#include "widgets/xuiobject.h"
#include "xuiscript.h"

class XUIInstantiatorText : public XUIElement {
public:
  virtual int getElementType() { return XUIELEMENT_TEXT; }
  virtual void setAttribute(const char *attrname, const char *attrvalue) { }
};


class XUIInstantiator : public DOMInstantiator {
  public:
    XUIInstantiator(DefinitionsFramework *framework=NULL);
    virtual ~XUIInstantiator();

    virtual XUIElement *_instantiate(DOMNode *node, void *parentObj, void **parentOverrideForNodeChildren, PtrList<XUIScript> **bindingParentScriptsToPop);
    virtual void *instantiate(DOMNode *node, void *parentObj, void **parentOverrideForNodeChildren);
    virtual void link(DOMNode *parentNode, void *parentObject, DOMNode *childNode, void *childObject);

    void setAttributes(DOMNode *node, XUIElement *object);
    XUIObject *instantiateXUINode(DOMNode *node, const char *topNodeScriptIdentifier="window", bool autoInit=false, RootWnd *autoInitParentWindow=NULL);
    void deleteXUIObject(XUIObject *object);
  
  private:
    DOMNode *getBindingAncestor(DOMNode *node);
    svc_xuiObject *getServiceForTag(const char *tagname);
    PtrList<XUIInstantiatorText> m_textFragments;
    PtrList<XUIScript> *m_scripts;
    void *m_thisBindingChildren;
    Stack<void*> m_thisBindingChildrenStack;
    DOMNode *m_bindingParentNode;
    Stack<DOMNode *> m_bindingParentNodeStack;
    XUIObject *m_thisBindingRoot;
};

#endif
