#include "precomp.h"
#include "attrstr.h"

#include <bfc/memblock.h>

const char *_string::getValue() {
  int l = getDataLen();
  if (l <= 0) return "";
  MemBlock<char> mb(l+2);
  getData(mb.getMemory(), l+2);
  returnval = mb;
  return returnval;
}
