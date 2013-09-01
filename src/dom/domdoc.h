#ifndef __DOMNDOC_H
#define __DOMNDOC_H

#include "domnode.h"

class DOMDocument: public DOMNode {
public:
  DOMDocument();
  DOMDocument(const char *filename, int flags=0);
  virtual ~DOMDocument();
public:
 // Creates an Attr of the given name.
 DOMAttr *createAttribute(const char *name);
 // Creates an attribute of the given qualified name and namespace URI.         
 DOMAttr *createAttributeNS(const char *namespaceURI, const char *qualifiedName);
 // Creates a CDATASection node whose value is the specified const char *         
 DOMCDATASection *createCDATASection(const char *data);
 // Creates a Comment node given the specified string.         
 DOMComment *createComment(const char *data);
 // Creates an empty DocumentFragment object.         
 DOMDocumentFragment *createDocumentFragment();
 // Creates an element of the type specified.         
 DOMElement *createElement(const char *tagName);
 // Creates an element of the given qualified name and namespace URI.         
 DOMElement *createElementNS(const char *namespaceURI, const char *qualifiedName);
 // Creates an EntityReference object.         
 DOMEntityReference *createEntityReference(const char *name);
 // Creates a ProcessingInstruction node given the specified name and data strings.         
 DOMProcessingInstruction *createProcessingInstruction(const char *target, const char *data);
 // Creates a Text node given the specified string.         
 DOMText *createTextNode(const char *data);
 // The Document Type Declaration (see DOMDocumentType) associated with this document.         
 DOMDocumentType *getDoctype();
 // This is a convenience attribute that allows direct access to the child node that is the root element of the document.          
 DOMElement *getDocumentElement();
 // Returns the Element whose ID is given by elementId.
 DOMElement *getElementById(const char *elementId);
 // Returns a NodeList of all the Elements with a given tag name in the order in which they are encountered in a preorder traversal of the Document tree.          
 DOMNodeList *getElementsByTagName(const char *tagname);
 // Returns a NodeList of all the Elements with a given local name and namespace URI in the order in which they are encountered in a preorder traversal of the Document tree.
 DOMNodeList *getElementsByTagNameNS(const char *namespaceURI, const char *localName);
 // The DOMImplementation object that handles this document.
 DOMImplementation *getImplementation();
 // Imports a node from another document to this document.         
 DOMNode *importNode(DOMNode *importedNode, bool deep);

private:
 DOMElement *getSubElementById(DOMElement * el, const char *elementId);

};

#endif