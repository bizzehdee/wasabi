#ifndef __DOMIMPL_H
#define __DOMIMPL_H

#include "defnoderesults.h"

class DOMImplementation {
public:
  DOMImplementation();
public:
 //Creates a DOM Document object of the specified type with its document element.
 DOMDocument *createDocument(String namespaceURI, String qualifiedName, DocumentType *doctype);
 //Creates an empty DocumentType node.
 DOMDocumentType *createDocumentType(String qualifiedName, String publicId, String systemId);
 //Test if the DOM implementation implements a specific feature.         
 bool hasFeature(String feature, String version);
};


#endif