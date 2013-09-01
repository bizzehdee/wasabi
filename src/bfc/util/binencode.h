#ifndef BINENCODE_H
#define BINENCODE_H

#include <bfc/string/string.h>

class BinEncode {
public:
  BinEncode() {}
  virtual ~BinEncode() { m_lines.deleteAll(); }

  void encode(void *mem, int size, int linelength);
  int getNumLines() { return m_lines.getNumItems(); }
  const char *enumLine(int n) { return m_lines.enumItem(n)->getValue(); }

private:
  PtrList<String> m_lines;
};

#endif
