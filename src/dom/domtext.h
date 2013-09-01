#ifndef __DOMTEXT_H
#define __DOMTEXT_H

#include "domchardata.h"

class DOMText: public DOMCharData {
public:
  DOMText(const char *data);
public:
  //Breaks this node into two nodes at the specified offset, keeping both in the tree as siblings.
  DOMText *splitText(int offset);
};


#endif