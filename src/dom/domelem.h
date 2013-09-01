#ifndef __DOMNELEM_H
#define __DOMNELEM_H

#include "domnode.h"

class DOMElement: public DOMNode {
public:
  DOMElement(const char *tagName);
  DOMElement(const char *namespaceURI, const char *qualifiedName);
  virtual ~DOMElement();
public:
  // Retrieves an attribute value by name.
  const char *getAttribute(const char *name);
  // Retrieves an attribute node by name.         
  DOMAttr *getAttributeNode(const char *name);
  // Retrieves an Attr node by local name and namespace URI.         
  DOMAttr *getAttributeNodeNS(const char *namespaceURI, const char *localName);
  // Retrieves an attribute value by local name and namespace URI.         
  const char *getAttributeNS(const char *namespaceURI, const char *localName);
  // Returns a NodeList of all descendant Elements with a given tag name, in the order in which they are encountered in a preorder traversal of this Element tree.         
  DOMNodeList *getElementsByTagName(const char *name);
  // Returns a NodeList of all the descendant Elements with a given local name and namespace URI in the order in which they are encountered in a preorder traversal of this Element tree.         
  DOMNodeList *getElementsByTagNameNS(const char *namespaceURI, const char *localName);
  // The name of the element.         
  const char *getTagName();
  // Returns true when an attribute with a given name is specified on this element or has a default value, false otherwise.         
  bool 	hasAttribute(const char *name);
  // Returns true when an attribute with a given local name and namespace URI is specified on this element or has a default value, false otherwise.         
  bool 	hasAttributeNS(const char *namespaceURI, const char *localName);
  // Removes an attribute by name.         
  void removeAttribute(const char *name);
  // Removes the specified attribute node.         
  DOMAttr *removeAttributeNode(DOMAttr *oldAttr);
  // Removes an attribute by local name and namespace URI.         
  void removeAttributeNS(const char *namespaceURI, const char *localName);
  // Adds a new attribute.         
  void setAttribute(const char *name, const char *value);
  // Adds a new attribute node.         
  DOMAttr *setAttributeNode(DOMAttr *newAttr);
  // Adds a new attribute.         
  DOMAttr *setAttributeNodeNS(DOMAttr *newAttr);
  // Adds a new attribute.         
  void setAttributeNS(const char *namespaceURI, const char *qualifiedName, const char *value);

  // will just inspect it own children
  DOMNodeList *getDirectChildByTagName(const char *name);
  DOMNodeList *getDirectChildByTagNameNS(const char *namespaceURI, const char *localName);
};


#endif