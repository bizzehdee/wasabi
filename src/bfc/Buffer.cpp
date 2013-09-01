#include <precomp.h>
#include "Buffer.h"

Buffer::Buffer() {
  size = 0;
  buf = NULL;
}

Buffer::~Buffer() {
  if(getSize() > 0) {
    MEMZERO(buf, size); //nuke the buffer
    //CUT ZeroMemory(buf, size); //besides, use std_mem stuff
    freeMem();
  }
}

void Buffer::addData(void *mem, unsigned long memsize) {
  ASSERTPR(memsize > 0, "memsize must be greater than 0");
  ASSERTPR(mem != NULL, "mem pointer cannot be NULL");

  //stop people from modifying our data while we are adding to it
  INCRITICALSECTION(real_cs);

  //new, much faster version
  buf = REALLOC(buf, size+memsize);
  MEMCPY(((char *)buf)+size, mem, memsize);

  size += memsize;
}

void *Buffer::getBufPtr() {
  return buf;
}

unsigned long Buffer::getBuf(void *memptr, unsigned long start, unsigned long memsize) {
  ASSERTPR(memsize < (size - start), "memsize must be less than size minus start");
  ASSERTPR(memptr != NULL, "memptr must be alloced before now");
  ASSERTPR(!buf, "there needs to be a buffer");

  INCRITICALSECTION(real_cs);
 
  if(memptr) {
    MEMCPY(memptr, ((char *)buf)+start, MIN(memsize,size-start));
    return MIN(memsize,size-start);
  }

  return 0;
}

unsigned long Buffer::getSize() {
  return size;
}

//this should move the mem from the end start+memsize
//to start, then realloc to a new size.
//allows to remove blocks of mem from the buffer
//uses critsec to be thread save.
unsigned long Buffer::removeFromBuf(unsigned long start, unsigned long memsize) {
  ASSERTPR(start < size, "have to start before the end");
  ASSERTPR(start+memsize <= size, "cant get past end of the buffer");
  ASSERTPR(buf != NULL, "there needs to be a buffer");

  INCRITICALSECTION(real_cs);

  MEMMOVE(((char *)buf)+(start+memsize), ((char *)buf)+start, size-(start+memsize));
  size -= memsize;
  buf = REALLOC(buf, size);
  return size;
}

void Buffer::freeMem() {
  //if we are removing everything, 
  //we should stop people from trying 
  //to access it while we are doing it
  INCRITICALSECTION(real_cs);

  size = 0;
  FREE(buf);
}