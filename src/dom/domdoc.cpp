#include "precomp.h"
#include "domdoc.h"
#include "domelem.h"
#include "domattr.h"
#include "domtext.h"


DOMDocument::DOMDocument()  {
  m_type = DOCUMENT_NODE;
  m_nodeName = "#document";
}

DOMDocument::DOMDocument(const char *filename, int flags) {
  m_type = DOCUMENT_NODE;
  m_nodeName = "#document";
  loadXml(filename,flags);
}

DOMDocument::~DOMDocument() {

}

DOMAttr *DOMDocument::createAttribute(const char *name) {
  DOMAttr * res = new DOMAttr(name);
  m_issued.addItem(res);
  return res;
}


DOMAttr *DOMDocument::createAttributeNS(const char *namespaceURI, const char *qualifiedName) {
  DOMAttr * res = new DOMAttr(namespaceURI,qualifiedName);
  m_issued.addItem(res);
  return res;
}


DOMCDATASection *DOMDocument::createCDATASection(const char *data) {

  return NULL;
}


DOMComment *DOMDocument::createComment(const char *data) {

  return NULL;
}


DOMDocumentFragment *DOMDocument::createDocumentFragment() {

  return NULL;
}


DOMElement *DOMDocument::createElement(const char *tagName) {
  DOMElement * res = new DOMElement(tagName);
  m_issued.addItem(res);
  return res;
}


DOMElement *DOMDocument::createElementNS(const char *namespaceURI, const char *qualifiedName) {
  DOMElement * res = new DOMElement(namespaceURI,qualifiedName);
  m_issued.addItem(res);
  return res;
}


DOMEntityReference *DOMDocument::createEntityReference(const char *name) {

  return NULL;
}


DOMProcessingInstruction *DOMDocument::createProcessingInstruction(const char *target, const char *data) {

  return NULL;
}


DOMText *DOMDocument::createTextNode(const char *data) {
  DOMText *res = new DOMText(data);
  m_issued.addItem(res);
  return res;
}


DOMDocumentType *DOMDocument::getDoctype() {

  return NULL;
}


DOMElement *DOMDocument::getDocumentElement() {
  return (DOMElement *) enumChild(0);
}


DOMElement *DOMDocument::getElementById(const char *elementId)  {
  PtrList<Node> list = ChildList();
  foreach(list)
    DOMNode * node = (DOMNode *) list.getfor();
    if ( node->getNodeType() == ELEMENT_NODE ) {
      DOMElement * el = (DOMElement *) node;
      if ( !STRCMPSAFE( el->getAttribute("id") , elementId ) )
        return el;
      el = getSubElementById(el,elementId);
      if ( el )
        return el;
    }  
  endfor
  return NULL;
}


DOMElement *DOMDocument::getSubElementById(DOMElement * el, const char *elementId)  {
  PtrList<Node> list = el->ChildList();
  foreach(list)
    DOMNode * node = (DOMNode *) list.getfor();
    if ( node->getNodeType() == ELEMENT_NODE ) {
      DOMElement * thisElement = (DOMElement *) node;
      if ( !STRCMPSAFE( thisElement->getAttribute("id") , elementId ) )
        return thisElement;
      thisElement = getSubElementById(thisElement,elementId);
      if ( thisElement )
        return thisElement;
    }  
  endfor
  return NULL;

}


DOMNodeList *DOMDocument::getElementsByTagName(const char *tagname)  {
  m_nodelist.removeAll();
  PtrList<Node> list = ChildList();
  foreach(list)
    DOMNode * node = (DOMNode *) list.getfor();
    if ( node->getNodeType() == ELEMENT_NODE ) {
      DOMElement * el = (DOMElement *) node;
      if ( !STRCMPSAFE( el->getTagName() , tagname ) )
        m_nodelist.addItem(node);
      m_nodelist.appendFrom(el->getElementsByTagName(tagname));
    }  
  endfor
  return &m_nodelist;
}


DOMNodeList *DOMDocument::getElementsByTagNameNS(const char *namespaceURI, const char *localName)  {
  m_nodelist.removeAll();
  PtrList<Node> list = ChildList();
  foreach(list)
    DOMNode * node = (DOMNode *) list.getfor();
    if ( node->getNodeType() == ELEMENT_NODE ) {
      DOMElement * el = (DOMElement *) node;
      if ( !STRCMPSAFE( el->getLocalName() , localName ) && !STRCMPSAFE( el->getNamespaceURI() , namespaceURI ))
        m_nodelist.addItem(node);
      m_nodelist.appendFrom(el->getElementsByTagNameNS(namespaceURI,localName));
    }  
  endfor
  return &m_nodelist;
}


DOMImplementation *DOMDocument::getImplementation() {

  return NULL;
}


DOMNode *DOMDocument::importNode(DOMNode *importedNode, bool deep) {

  return NULL;
}

