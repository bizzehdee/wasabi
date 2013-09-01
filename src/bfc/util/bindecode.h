#ifndef BINDECODE_H
#define BINDECODE_H

#include <bfc/memblock.h>

class BinDecode {
public:
  BinDecode() {}
  virtual ~BinDecode() { m_encodeddata.deleteAll(); }

  void resetData() { m_encodeddata.deleteAll(); m_mem.setSize(0); }
  void addDecodeData(const char *data);
  void *decode();
  void *getDecodedData() { return m_mem.getMemory(); }

private:
  MemBlock<char> m_mem;
  PtrList<String> m_encodeddata;
};

#endif
