#ifndef __WASABI_BUFFER_H_
#define __WASABI_BUFFER_H_

#include <bfc/std.h>
#include <bfc/assert.h>
#include <bfc/critsec.h>

/*
  Generic Threadsafe Buffer.
*/

class Buffer {
public:
  Buffer();
  virtual ~Buffer();

  void addData(void *mem, unsigned long memsize);
  unsigned long getBuf(void *memptr, unsigned long start, unsigned long size);
  unsigned long getSize();
  unsigned long removeFromBuf(unsigned long start, unsigned long memsize);

  void *getBufPtr();

  void freeMem();

private:
  CriticalSection real_cs;
  unsigned long size;
  void *buf;
};

#endif //!__WASABI_BUFFER_H_
