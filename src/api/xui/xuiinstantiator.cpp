#include <precomp.h>
#include <dom/domnodemap.h>
#include <dom/domattr.h>
#include <dom/domchardata.h>
#include "xuielement.h"
#include <api/service/svcs/svc_xuiobject.h>
#include "xuijsscript.h"
#include "xuiinstantiator.h"
#include "xuiwnd.h"
#include <wnd/virtualwnd.h>
#include <api/wdf/wdf.h>
#include <parse/pathparse.h>

#define XUISVC_GENERICCONTAINER "#genericContainer"

XUIInstantiator::XUIInstantiator(DefinitionsFramework *framework) : DOMInstantiator() {
  DOMInstantiator::setFramework(framework);
  m_scripts = new PtrList<XUIScript>;
  m_bindingParentNode = NULL;
  m_thisBindingChildren = NULL;
  m_thisBindingRoot = NULL;
}

XUIInstantiator::~XUIInstantiator() {
  // only delete the list, not the scripts, which have been registered into their respective parents
  // and will go away when their parent goes away
  delete m_scripts;
}

svc_xuiObject *XUIInstantiator::getServiceForTag(const char *xmltag) {
  svc_xuiObject *svc = NULL;

  XuiObjectSvcEnum xuiservices(xmltag);
  return xuiservices.getNext(FALSE);
}

XUIElement *XUIInstantiator::_instantiate(DOMNode *node, void *parentObj, void **parentOverrideForNodeChildren, PtrList<XUIScript> **bindingParentScriptsToPop) {
  const char *xmltag = node->getNodeName();

  if (STRCASEEQL(xmltag, "object")) {
    DOMNamedNodeMap *attrs = node->getAttributes();
    DOMNode *classAttr = attrs->getNamedItem("tag");
    if (classAttr) {
      xmltag = classAttr->getNodeValue();
    }
  }
  
  int type = node->getNodeType();
  if (type == TEXT_NODE ||
      type == CDATA_SECTION_NODE) {
    return m_textFragments.addItem(new XUIInstantiatorText());
  } else if (STRCASEEQL(xmltag, "script")) {
    return m_scripts->addItem(new XUIJSScript());
  } else if (STRCASEEQL(xmltag, "binding") ||
             // do not instantiate groupdefs unless they are at the top of the instantiation chain,
             // groupdefs inside objects are just definitions !
             (!parentObj && STRCASEEQL(xmltag, "groupdef"))) {

    svc_xuiObject *svc = getServiceForTag(XUISVC_GENERICCONTAINER);
    ASSERT(svc);
    XUIObject *xo = svc->instantiate(XUISVC_GENERICCONTAINER, NULL);
    m_thisBindingRoot = xo;

    DOMNode *ancestor = getBindingAncestor(node);
    if (ancestor) {
      XUIElement *e = xo;
      doChainedInstantiation(ancestor, this, NULL, true, e);
      setAttributes(ancestor, e);
    }
    return xo;
  } else if (parentObj && STRCASEEQL(xmltag, "groupdef")) {
    // local groupdef definition, register it in the parent object
    XUIObject *xo = static_cast<XUIObject*>(parentObj);
    // ignore children nodes, do not instantiate them !
    xo->registerLocalGroupDefinition(node);
    return NULL;
  } else if (STRCASEEQL(xmltag, "children")) {
    svc_xuiObject *svc = getServiceForTag(XUISVC_GENERICCONTAINER);
    ASSERT(svc);
    XUIObject *xo = svc->instantiate(XUISVC_GENERICCONTAINER, NULL);
    m_thisBindingChildren = xo;
    return xo;
  } else {
    svc_xuiObject *svc = getServiceForTag(xmltag);

    XUIObject *xo = NULL;

    if (svc != NULL) {
      return svc->instantiate(xmltag, NULL);
    } else {
      // try to find a matching binding
      DOMNode *bindingNode = getFramework()->getTopLevelNode("binding", xmltag);
      if (bindingNode) {

        m_thisBindingChildrenStack.push(m_thisBindingChildren);
        m_thisBindingChildren = NULL;
        m_bindingParentNodeStack.push(m_bindingParentNode);
        m_bindingParentNode = node;
        
        // save current list of scripts
        PtrList<XUIScript> *parentScripts = m_scripts;
        
        // make a new one for the binding
        m_scripts = new PtrList<XUIScript>; 

        XUIObject *prevBindingRoot = m_thisBindingRoot;
        m_thisBindingRoot = NULL;
        
        // instantiate binding and content
        XUIElement *content = reinterpret_cast<XUIElement *>(doChainedInstantiation(bindingNode, this, NULL));

        m_thisBindingRoot = prevBindingRoot;
        
        // do not execute and pop the scripts here, attributes and tagname have not been set yet, 
        // so notify the caller to do so after everything has been initialized on the object
        *bindingParentScriptsToPop = parentScripts;

        *parentOverrideForNodeChildren = m_thisBindingChildren;
        m_thisBindingChildrenStack.pop(&m_thisBindingChildren);
        m_bindingParentNodeStack.pop(&m_bindingParentNode);

        setAttributes(bindingNode, content);
        
        return content;
      }
    }
  }
  return NULL;
}

void *XUIInstantiator::instantiate(DOMNode *node, void *parentObj, void **parentOverrideForNodeChildren) {
  PtrList<XUIScript> *bindingParentScriptsToPop = NULL;
  XUIElement *xe = _instantiate(node, parentObj, parentOverrideForNodeChildren, &bindingParentScriptsToPop);
  if (xe) {
    switch (xe->getElementType()) {
      case XUIElement::XUIELEMENT_XUIOBJECT: {
        XUIObject *xo = static_cast<XUIObject *>(xe);
        // tell the object the name of the xml node that was used to instantiate it, for getElementByTagName, etc.
        xo->setTagName(node->getNodeName());
        // give the object the framework that was used to instantiate it, so it can use that to look for more resources (ie, images, etc.)
        xo->setFramework(m_framework);
        // let the object know that it is at the top of an instantiation (ie, binding, groups), so that getElementById does not continue through it.
        if (bindingParentScriptsToPop) {
          xo->setInstantiationElement();
        }
        // set all attributes
        setAttributes(node, xo);
        break;
      }
    }
  }
  if (bindingParentScriptsToPop) {
    // install toplevel "binding" object for the binding's scripts,
    // and the toplevel window as "window" (if there is no top level
    // object yet, we are it, so expose our parent as "window)
    PtrList<JSObjectDef> defs;
    defs.addItem(new JSObjectDef(static_cast<XUIObject*>(xe), "binding"));
    if (m_topLevel)
      defs.addItem(new JSObjectDef(reinterpret_cast<XUIObject*>(m_topLevel), "window"));
    else {
      ASSERT(0); // m_bindingParentNode is not an XUIOBJECT !
      defs.addItem(new JSObjectDef(reinterpret_cast<XUIObject*>(m_bindingParentNode), "window"));
    }
    foreach(*m_scripts)
      m_scripts->getfor()->onReady(defs);
    endfor;
    // delete only the list, the scripts have been registered to their parent element,
    // they will be deleted when the element goes away
    delete m_scripts;
    // restore parent list of scripts
    m_scripts = bindingParentScriptsToPop;
  }
  return xe;
}

void XUIInstantiator::link(DOMNode *parentNode, void *parentObject, DOMNode *childNode, void *childObject) {
  XUIElement *eChild = reinterpret_cast<XUIElement*>(childObject);
  XUIElement *eParent = reinterpret_cast<XUIElement*>(parentObject);
  if (eChild->getElementType() == XUIElement::XUIELEMENT_XUISCRIPT) {
    XUIJSScript *script = reinterpret_cast<XUIJSScript *>(childObject);
    XUIObject *xuiParent = reinterpret_cast<XUIObject*>(parentObject);
    xuiParent->registerScript(script);
  }
  switch (eParent->getElementType()) {
    case XUIElement::XUIELEMENT_XUISCRIPT: {
      if (eChild->getElementType() == XUIElement::XUIELEMENT_TEXT) {
        XUIJSScript *script = reinterpret_cast<XUIJSScript *>(parentObject);
        DOMCharData *textNode = reinterpret_cast<DOMCharData *>(childNode);
        script->appendCode(textNode->getData(), textNode->getSourceFile(), parentNode->getSourceLine());
      }
      break;
    }
    case XUIElement::XUIELEMENT_XUIOBJECT: {
      if (eChild->getElementType() == XUIElement::XUIELEMENT_XUIOBJECT) {
        XUIObject *xuiParent = reinterpret_cast<XUIObject*>(parentObject);
        XUIObject *xuiChild = reinterpret_cast<XUIObject*>(childObject);
        xuiParent->addChild(xuiChild);
      }
      break;
    }
  }
}

void XUIInstantiator::setAttributes(DOMNode *node, XUIElement *object) {
  DOMNamedNodeMap *attributes = node->getAttributes();
  XUIObject *xuiNode = reinterpret_cast<XUIObject*>(object);
  for (int i=0; i<attributes->getLength(); i++) {
    DOMAttr *attribute = static_cast<DOMAttr *>(attributes->item(i));
    const char *attrname = attribute->getName();
    const char *attrvalue = attribute->getValue();
    if (STRCASEEQL(attrname, "inherit") && 
        m_bindingParentNode != NULL) {
      PathParser ppExpr(attrvalue, ",;");
      for (int i=0;i<ppExpr.getNumStrings();i++) {
        const char *expr = ppExpr.enumString(i);
        const char *eq = STRCHR(expr, '=');
        String attr;
        String mappedattr;
        if (eq) {
          attr.cat(eq+1);
          mappedattr.catn(expr, eq-expr);
        } else {
          attr = expr;
          mappedattr = expr;
        }
        // first copy the current attribues, from the dom node
        DOMNamedNodeMap *parentAttrs = m_bindingParentNode->getAttributes();
        DOMNode *attrNode = parentAttrs->getNamedItem(attr);
        if (attrNode) {
          const char *attrvalue = attrNode->getNodeValue();
          xuiNode->setAttribute(mappedattr, attrvalue);
        }
        // second, install a listener on the xui object, to catch new attribute values dynamically
        xuiNode->addAttribForwarding(m_thisBindingRoot, attr, mappedattr);
      }
    } else {
      xuiNode->setAttribute(attrname, attrvalue);
    }
  }
}

XUIObject *XUIInstantiator::instantiateXUINode(DOMNode *node, const char *topNodeScriptIdentifier, bool autoInit, RootWnd *autoInitParentWindow) {
  if (!getFramework()) return NULL;
  if (!node) return NULL;
  void *top = instantiateNode(node, this);
  if (!top) return NULL;
  XUIElement *e = reinterpret_cast<XUIElement*>(top);
  ASSERT(e->getElementType() == XUIElement::XUIELEMENT_XUIOBJECT);
  XUIObject *topLevel = reinterpret_cast<XUIObject *>(e);
  if (topLevel) topLevel->setInstantiationElement();
  if (topLevel && autoInit) {
    if (!topLevel->customTopLevelInit(autoInitParentWindow)) {
      RootWnd *window = topLevel->getRootWnd();
      if (window) {
        if (autoInitParentWindow)
          window->setParent(autoInitParentWindow);
        window->init(autoInitParentWindow, TRUE);
      }
    }
  }
  foreach(*m_scripts)
    m_scripts->getfor()->onReady(topLevel, topNodeScriptIdentifier);
  endfor;
  m_textFragments.deleteAll();
  return topLevel;
}

void XUIInstantiator::deleteXUIObject(XUIObject *object) {
  if (!object) return;
  svc_xuiObject *svc = object->getOwnerService();
  svc->destroy(object);
}

DOMNode *XUIInstantiator::getBindingAncestor(DOMNode *node) {
  DOMNamedNodeMap *attributes = node->getAttributes();
  DOMAttr *attr = static_cast<DOMAttr*>(attributes->getNamedItem("ancestor"));
  if (attr) {
    const char *ancestor = attr->getNodeValue();
    if (STRCASEEQL(ancestor, "#previous")) {
      return getFramework()->getPreviousTopLevelNode(node);
    } else {
      return getFramework()->getTopLevelNode("binding", ancestor);
    }
  }
  return NULL;
}