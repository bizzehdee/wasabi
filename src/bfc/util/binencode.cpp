#include <precomp.h>
#include "binencode.h"

#define CHARTOHEX(c) ( ((c) < 10) ? ((c) + '0') : ((c)-10 + 'A') )

void BinEncode::encode(void *mem, int size, int linelength) {
  m_lines.deleteAll();
  unsigned char *p = (unsigned char *)mem;
  int n = size;
  char enc[3]="XX";
  while (n) {
    String line;
    for (int i=0;i<linelength && n > 0;i++) {
      enc[0] = CHARTOHEX(*p >> 4);
      enc[1] = CHARTOHEX(*p & 0xF);
      line.cat(enc);
      p++;
      n--;
    }
    m_lines.addItem(new String(&line));
  }
}

