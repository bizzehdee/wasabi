#ifndef __DOMNODEMAP_H
#define __DOMNODEMAP_H

#include "domdefs.h"

class DOMNodeSortByName {
public:
  static int compareItem(DOMNode *p1, DOMNode* p2);
  static int compareAttrib(const char *attrib, DOMNode *item);
};

class DOMNamedNodeMap : public PtrListInsertSorted<DOMNode, DOMNodeSortByName> {
public:
  DOMNode *getNamedItem(const char *name);
  DOMNode *setNamedItem(const char *name);
  DOMNode *removeNamedItem(const char *name);
  DOMNode *item(int i);
  int getLength();
};

#endif
