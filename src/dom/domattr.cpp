#include "precomp.h"
#include "domattr.h"
#include "domelem.h"

DOMAttr::DOMAttr(const char *tagName, DOMElement * owner)  {
  m_type = ATTRIBUTE_NODE;
  m_nodeName = tagName;
  setOwnerElement(owner);
  processName();
}


DOMAttr::DOMAttr(const char *namespaceURI, const char *qualifiedName, DOMElement *owner) {
  m_type = ATTRIBUTE_NODE;
  m_nodeName = qualifiedName;
  m_namespaceURI = namespaceURI;
  setOwnerElement(owner);
  processName();
}


DOMAttr::~DOMAttr() {


}

const char *DOMAttr::getName()  {
  return m_nodeName;
}


DOMElement *DOMAttr::getOwnerElement() {
  return (DOMElement *) m_owner;
}


void DOMAttr::setOwnerElement(DOMElement *owner) {
  m_ownerElement = owner;
  if ( m_ownerElement )
    m_owner = m_ownerElement->getOwnerDocument();
}


bool DOMAttr::getSpecified()  {
  return m_nodeValue.v() != NULL;
}

const char *DOMAttr::getValue()  {
  return m_nodeValue;
}

void DOMAttr::setValue(const char *value)  {
  m_nodeValue = value;
  if (STRCASEEQL(m_nodeName, "id")) 
    getParentNode()->clearIdCache();
}

