#ifndef __DOMATTR_H
#define __DOMATTR_H

#include "domnode.h"

class DOMAttr: public DOMNode {
public:
  DOMAttr(const char *tagName, DOMElement * owner=NULL);
  DOMAttr(const char *namespaceURI, const char *qualifiedName, DOMElement * owner=NULL);
  virtual ~DOMAttr();
public:
  //Returns the name of this attribute.
  const char *getName();
  // The Element node this attribute is attached to or null if this attribute is not in use.
  DOMElement *getOwnerElement();
  void setOwnerElement(DOMElement *owner);
  // If this attribute was explicitly given a value in the original document, this is true; otherwise, it is false.
  bool getSpecified();
  // On retrieval, the value of the attribute is returned as a string.
  const char *getValue();
  // On retrieval, the value of the attribute is returned as a string.
  void setValue(const char *value);

private:
  DOMElement * m_ownerElement;
};


#endif