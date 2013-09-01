#ifndef _MEMBLOCK_H
#define _MEMBLOCK_H

#include <bfc/std.h>
#include <wasabicfg.h>

#ifdef WASABI_DEBUG
extern int memblocks_totalsize;
#endif

class VoidMemBlock {
protected:
  VoidMemBlock(int size=0, const void *data=NULL);
  ~VoidMemBlock();

  void *setSize(int newsize, int noinit=FALSE);
  void *setMinimumSize(int newminsize, int increment=0);
  void *getMemory() const { return mem; }
  void setMemory(const void *data, int datalen, int offsetby=0);
  int getSize() const;

  int isMine(void *ptr);

  void terminate(int character='\0');

  void zeroMemory();

protected:
  void *mem;
  int size;
};

// just a convenient resizeable block of memory wrapper
// doesn't handle constructors or anything, meant for int, char, etc.
template <class T>
class MemBlock : protected VoidMemBlock {
public:
  MemBlock(int _size = 0, const T *data = NULL) : VoidMemBlock(_size*sizeof(T), data) {}
  MemBlock(const MemBlock<T> &source) : VoidMemBlock(source.getSizeInBytes(), source.getMemory()) {}
  MemBlock(const MemBlock<T> *source) : VoidMemBlock(source->getSizeInBytes(), source->getMemory()) {}
  MemBlock<T>& operator=(const MemBlock<T> &source) {
    if (this == &source) return *this;
    append(source.getMemory(), source.getSize());
    return *this;
  }

  T *setSize(int newsize, int noinit=FALSE) {
    return static_cast<T*>(VoidMemBlock::setSize(newsize * sizeof(T), noinit));
  }
  T *setMinimumSize(int newminsize, int increment = 0) {
    return static_cast<T*>(VoidMemBlock::setMinimumSize(newminsize * sizeof(T), increment*sizeof(T)));
  }

  T *getMemory() const { return static_cast<T *>(VoidMemBlock::getMemory()); }
  T *getMemory(int offset) const { return static_cast<T *>(VoidMemBlock::getMemory())+offset; } // note pointer arithmetic using type T
  T *m() const { return getMemory(); }
  T *m(int offset) const { return getMemory(offset); }
  operator T *() const { return getMemory(); }
  T& operator() (unsigned int ofs) { return getMemory()[ofs]; }
  const T& operator() (unsigned int ofs) const { return getMemory()[ofs]; }
  int isNull() const { return (getMemory() == NULL); }

  void copyTo(T *dest) { MEMMOVE(dest, getMemory(), getSizeInBytes()); } // using MEMMOVE since we could be copying right in the middle of our buffer

  int getSize() const { return VoidMemBlock::getSize()/sizeof(T); }// # of T's
  int getSizeInBytes() const { return VoidMemBlock::getSize(); }

  int isMine(T *ptr) { return VoidMemBlock::isMine(ptr); }

  void setMemory(const T *data, int datalen, int offsetby = 0) { VoidMemBlock::setMemory(data, datalen*sizeof(T), offsetby*sizeof(T)); }	// # of T's
  void zeroMemory() { VoidMemBlock::zeroMemory(); }

  void terminate(int character='\0') { VoidMemBlock::terminate(character); }

  void reverse() {
    const int nitems = getSize();
    if (nitems <= 1) return;
    MemBlock<T> nm(this);
    T *mymem = getMemory(), *revmem = nm.getMemory();
    for (int i = 0, j = nitems-1; j >= 0; i++, j--) {
      MEMMOVE(mymem+i, revmem+j, sizeof(T));
    }
  }

  void append(const T *data, int num=1) {
    int prev_size = getSize();
    setSize(prev_size + num, TRUE);	// noinit
    MEMCPY(m()+prev_size, data, num*sizeof(T));
  }
};

// just a convenience class for a 2d MemBlock<>
template <class T>
class MemMatrix : public MemBlock<T> {
public:
  MemMatrix(int _width=0, int _height=0, const T *data = NULL) :
    MemBlock<T>(_width * _height, data), width(_width), height(_height) {}

  T *setSize(int _width, int _height, int noinit=FALSE) {
    MemBlock<T>::setSize(_width * _height, noinit);
    width = _width;
    height = _height;
    return MemBlock<T>::getMemory();
  }

  T *resize(int _width, int _height) {
    MemMatrix<T> newblock(_width, _height);
    int mw = MIN(width, _width);
    int mh = MIN(height, _height);
    for (int y=0;y<mw;y++) {
      for (int x=0;x<mw;x++) {
        *(newblock.m2(x, y)) = *m2(x, y);
      }
    }
    MemBlock<T>::setSize(0);
    this->mem = newblock.mem;
    this->size = newblock.size;
    newblock.size = 0;
    newblock.mem = NULL;
    width = _width;
    height = _height;
    return MemBlock<T>::getMemory();
  }

  int getWidth() const { return width; }
  int getHeight() const { return height; }

  // () access i.e. obj(x,y)
  T& operator() (unsigned int x, unsigned int y) {
    return *MemBlock<T>::m(x + y * width);
  }
  const T& operator() (unsigned int x, unsigned int y) const {
    return *MemBlock<T>::m(x + y * width);
  }
  T& getRef(unsigned int x, unsigned int y) {
    return *MemBlock<T>::m(x + y * width);
  }
  T* m2(int x=0, int y=0) const { return MemBlock<T>::m(y*width+x); }

private:
  int width, height;
};


#endif
