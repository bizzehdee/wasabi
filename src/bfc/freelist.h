#ifndef _FREELIST_H
#define _FREELIST_H

#include <bfc/std.h>
#include <bfc/memblock.h>
#include <bfc/ptrlist.h>

// actual implementation
class FreelistPriv {
protected:
  FreelistPriv();
  ~FreelistPriv();

  void *getRecord(int typesize, int blocksize, int initialblocksize);
  void freeRecord(void *rec);

private:
  int total_allocated;
  typedef unsigned char MBT;
  class FLMemBlock : public MemBlock<MBT> {
  public:
    FLMemBlock(int siz) : MemBlock<MBT>(siz) {
      freelist = getMemory();
      nallocated = 0;
    }
    void *freelist;
    int nallocated;	// # of records assigned from block
  };
  PtrList< FLMemBlock > blocks; // the blocks of mem we alloc from
};

// type-safe template
static const int DEF_BLOCKSIZE=250;
template
<class T, int BLOCKSIZE=DEF_BLOCKSIZE, int INITIALBLOCKSIZE=BLOCKSIZE, int ALIGNMENT=4>
class Freelist : private FreelistPriv {
public:
  // just returns the memory, you have to call constructor manually
  T *getRecord() {
    return static_cast<T *>(FreelistPriv::getRecord(itemsize(), BLOCKSIZE, INITIALBLOCKSIZE));
  }

  // creates object via default constructor
  T *newRecord() {
    T *ret = getRecord();
#ifdef FORTIFY
#undef new
#endif
    new(ret) T;
#ifdef FORTIFY
#define new ZFortify_New
#endif
    return ret;
  }

  // creates object via 1-parameter constructor
  template<class P1>
  T *newRecord(P1 p1) {
    T *ret = getRecord();
#ifdef FORTIFY
#undef new
#endif
    new(ret) T(p1);
#ifdef FORTIFY
#define new ZFortify_New
#endif
    return ret;
  }

  // creates object via 2-parameter constructor
  template<class P1, class P2>
  T *newRecord(P1 p1, P2 p2) {
    T *ret = getRecord();
#ifdef FORTIFY
#undef new
#endif
    new(ret) T(p1, p2);
#ifdef FORTIFY
#define new ZFortify_New
#endif
    return ret;
  }

  // creates object via 3-parameter constructor
  template<class P1, class P2, class P3>
  T *newRecord(P1 p1, P2 p2, P3 p3) {
    T *ret = getRecord();
#ifdef FORTIFY
#undef new
#endif
    new(ret) T(p1, p2, p3);
#ifdef FORTIFY
#define new ZFortify_New
#endif
    return ret;
  }

  // just frees it, you have to call destructor manually
  void freeRecord(T *record) { FreelistPriv::freeRecord(record); }

  // calls delete for you, and frees it
  void deleteRecord(T *record) {
    if (record != NULL) {
      record->~T();
      freeRecord(record);
    }
  }

  void deletePtrList(PtrList<T> *list) {
    ASSERT(list != NULL);
    for (int i = 0; i < list->getNumItems(); i++) {
      deleteRecord(list->enumItem(i));
    }
    list->removeAll();
  }

protected:
  int itemsize() { return (sizeof(T) + (ALIGNMENT-1)) - (sizeof(T) % ALIGNMENT); }
};

#endif
