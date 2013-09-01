#include "precomp.h"
#include "domelem.h"
#include "domattr.h"

DOMElement::DOMElement(const char *tagName)  {
  m_type = ELEMENT_NODE;
  m_nodeName = tagName;
  processName();
}


DOMElement::DOMElement(const char *namespaceURI, const char *qualifiedName) {
  m_type = ELEMENT_NODE;
  m_nodeName = qualifiedName;
  m_namespaceURI = namespaceURI;
  processName();
}


DOMElement::~DOMElement() {


}

const char *DOMElement::getAttribute(const char *name){
  DOMAttr * theAttr = (DOMAttr *) m_attrs.findItem(name);
  if ( theAttr ) 
    return theAttr->getNodeValue();
  return (const char *) NULL;
}


DOMAttr *DOMElement::getAttributeNode(const char *name){
  return (DOMAttr *) m_attrs.findItem(name);
}


DOMAttr *DOMElement::getAttributeNodeNS(const char *namespaceURI, const char *localName){
  foreach(m_attrs)
    DOMNode * node = m_attrs.getfor();
    if ( node->getLocalName() == localName && node->getNamespaceURI() == namespaceURI )
      return (DOMAttr *) node;    
  endfor
  return NULL;
}


const char *DOMElement::getAttributeNS(const char *namespaceURI, const char *localName){
  DOMAttr * theAttr = getAttributeNodeNS(namespaceURI, localName);
  if ( theAttr ) 
    return theAttr->getNodeValue();
  return (const char *) NULL;
}


DOMNodeList *DOMElement::getElementsByTagName(const char *tagname)  {
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


DOMNodeList *DOMElement::getElementsByTagNameNS(const char *namespaceURI, const char *localName)  {
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


DOMNodeList *DOMElement::getDirectChildByTagName(const char *name){
  m_nodelist.removeAll();
  PtrList<Node> list = ChildList();
  foreach(list)
    DOMNode * node = (DOMNode *)list.getfor();
    if ( node->getNodeName() == name )
      m_nodelist.addItem(node);
  endfor
  return &m_nodelist;
}


DOMNodeList *DOMElement::getDirectChildByTagNameNS(const char *namespaceURI, const char *localName){
  m_nodelist.removeAll();
  PtrList<Node> list = ChildList();
  foreach(list)
    DOMNode * node = (DOMNode *)list.getfor();
    if ( node->getLocalName() == localName && node->getNamespaceURI() == namespaceURI )
      m_nodelist.addItem(node);
  endfor
  return &m_nodelist;
}


const char *DOMElement::getTagName(){

  return m_nodeName;
}


bool DOMElement::hasAttribute(const char *name){

  return getAttributeNode(name) != NULL;
}


bool DOMElement::hasAttributeNS(const char *namespaceURI, const char *localName){

  return getAttributeNodeNS(namespaceURI,localName) != NULL;
}


void DOMElement::removeAttribute(const char *name){
  DOMAttr * node = getAttributeNode(name);
  if ( node ) {
    bool isIssued = m_issued.haveItem((DOMNode *) node) != 0;
    removeAttributeNode(node);
    if ( isIssued )
      delete node;
  }
}


DOMAttr *DOMElement::removeAttributeNode(DOMAttr *oldAttr){
  if ( !oldAttr ) return NULL;
  m_attrs.removeItem((DOMNode *) oldAttr);
  m_issued.removeItem((DOMNode *) oldAttr);
  return oldAttr;
}


void DOMElement::removeAttributeNS(const char *namespaceURI, const char *localName){
  DOMAttr * node = getAttributeNodeNS(namespaceURI,localName);
  bool isIssued = m_issued.haveItem((DOMNode *) node) != 0;
  if ( node ) {
    removeAttributeNode(node);
    if ( isIssued )
      delete node;
  }
}


void DOMElement::setAttribute(const char *name, const char *value){
  removeAttribute(name);
  DOMAttr * node = new DOMAttr(name);
  if ( node ) {
    m_attrs.addItem((DOMNode *)node);
    m_issued.addItem((DOMNode *)node);
    node->setValue(value);
  }
}


DOMAttr *DOMElement::setAttributeNode(DOMAttr *newAttr){
  if ( !newAttr ) return NULL;
  removeAttribute(newAttr->getName());
  m_attrs.addItem(newAttr);
  return newAttr;
}


DOMAttr *DOMElement::setAttributeNodeNS(DOMAttr *newAttr){
  if ( !newAttr ) return NULL;
  removeAttributeNS(newAttr->getNamespaceURI(), newAttr->getLocalName());
  m_attrs.addItem(newAttr);
  return newAttr;
}


void DOMElement::setAttributeNS(const char *namespaceURI, const char *qualifiedName, const char *value){
  //String localName = qualifiedName;
  //localName.rSpliceChar(':');
  //removeAttributeNS( namespaceURI, localName.empty() ? qualifiedName : localName );
  // The above could have worked but let's protect the user from himself :) e.g full check

  foreach(m_attrs)
    DOMNode * node = m_attrs.getfor();
    if ( node->getNodeName() == qualifiedName && node->getNamespaceURI() == namespaceURI ) {
        bool isIssued = m_issued.haveItem(node) != 0;
        removeAttributeNode((DOMAttr *) node);
        if ( isIssued )
          delete node;
        // if we did our job correctly we can
        break;
    }
  endfor

  DOMAttr * node = new DOMAttr(namespaceURI,qualifiedName);
  if ( node ) {
    m_attrs.addItem((DOMNode *)node);
    m_issued.addItem((DOMNode *)node);
    node->setValue(value);
  }

}

