#ifndef __DOMNODELIST_H
#define __DOMNODELIST_H

#include "domdefs.h"

class DOMNodeList : public PtrList<DOMNode> {
public:
  DOMNode *item(int i) { return enumItem(i); }
  int getLength() { return getNumItems(); }
};


#endif
