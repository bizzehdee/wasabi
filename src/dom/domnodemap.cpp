#include <precomp.h>
#include "domnode.h"
#include "domnodemap.h"

int DOMNodeSortByName::compareItem(DOMNode *p1, DOMNode* p2) { 
  return STRCMP(p1->getNodeName(), p2->getNodeName()); 
}

int DOMNodeSortByName::compareAttrib(const char *attrib, DOMNode *item) { 
  return STRCMP(attrib, item->getNodeName()); 
}

DOMNode *DOMNamedNodeMap::getNamedItem(const char *name) { 
  return findItem(name); 
}

DOMNode *DOMNamedNodeMap::setNamedItem(const char *name) { 
  return addItem(new DOMNode(name)); 
}

DOMNode *DOMNamedNodeMap::removeNamedItem(const char *name) { 
  DOMNode *item = findItem(name);
  if (item != NULL)
    removeItem(item);
  return item;
}

DOMNode *DOMNamedNodeMap::item(int i) { 
  return enumItem(i); 
}

int DOMNamedNodeMap::getLength() { 
  return getNumItems(); 
}
