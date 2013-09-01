#include <precomp.h>
#include "bindecode.h"

void BinDecode::addDecodeData(const char *data) {
  m_encodeddata.addItem(new String(data));
}

#define HEXTO4BITS(c) (((c) >= '0' && (c) <= '9') ? ((c) - '0') : ((c) - 'A'))

void *BinDecode::decode() {
  int size = 0;
  foreach(m_encodeddata)
    size += m_encodeddata.getfor()->len();
  endfor
  m_mem.setSize(size/2);
  foreach(m_encodeddata)
    String *str = m_encodeddata.getfor();
    unsigned char *p = (unsigned char *)str->getNonConstVal();
    unsigned char *d = (unsigned char *)m_mem.getMemory();
    while (*p) {
      unsigned char a = TOUPPER(*p);
      unsigned char b = TOUPPER(*(p+1));
      *d = (HEXTO4BITS(a) << 4) | HEXTO4BITS(b);
      d++; p+=2;
    }
  endfor;
  return m_mem.getMemory();
}

