#ifndef _DRAG_H
#define _DRAG_H

#include <bfc/std.h>
#include "rootwnd.h"

class NOVTABLE DragInterface {
public:
  // (called on dest) when dragged item enters the winder
  virtual int dragEnter(RootWnd *sourceWnd)=0;
  // (called on dest) during the winder
  virtual int dragOver(int x, int y, RootWnd *sourceWnd)=0;
  // (called on src)
  virtual int dragSetSticky(RootWnd *wnd, int left, int right, int up, int down)=0;
  // (called on dest) when dragged item leaves the winder
  virtual int dragLeave(RootWnd *sourceWnd)=0;
  // (called on dest) here is where we actually drop it
  virtual int dragDrop(RootWnd *sourceWnd, int x, int y)=0;

  // must be called from within dragDrop();			(receiver)
  virtual const char *dragGetSuggestedDropTitle(void)=0;
  // must be called from within your dragEnter, Over, Leave, or Drop
  // return the slot # if you support this form of the data, -1 otherwise
  // nitems can be NULL if you're just checking validity
  virtual int dragCheckData(const char *type, int *nitems=NULL)=0;
  // fetches a specific pointer that was stored
  virtual void *dragGetData(int slot, int itemnum)=0;
  // return the number of data type slots stored
  virtual int dragGetNumTypes()=0;
  // return the name of the type for this slot
  virtual const char *dragGetType(int slot)=0;
  virtual int dragCheckOption(int option)=0;
};

class DragInterfaceI : public DragInterface {
public:
  // (called on dest) when dragged item enters the winder
  virtual int dragEnter(RootWnd *sourceWnd) { return 0; }
  // (called on dest) during the winder
  virtual int dragOver(int x, int y, RootWnd *sourceWnd) { return 0; }
  // (called on src)
  virtual int dragSetSticky(RootWnd *wnd, int left, int right, int up, int down) { return 0; }
  // (called on dest) when dragged item leaves the winder
  virtual int dragLeave(RootWnd *sourceWnd) { return 0; }
  // (called on dest) here is where we actually drop it
  virtual int dragDrop(RootWnd *sourceWnd, int x, int y) { return 0; }

  // must be called from within dragDrop();			(receiver)
  virtual const char *dragGetSuggestedDropTitle(void) { return NULL; }
  // must be called from within your dragEnter, Over, Leave, or Drop
  // return the slot # if you support this form of the data, -1 otherwise
  // nitems can be NULL if you're just checking validity
  virtual int dragCheckData(const char *type, int *nitems=NULL) { return 0; }
  // fetches a specific pointer that was stored
  virtual void *dragGetData(int slot, int itemnum) { return 0; }
  // return the number of data type slots stored
  virtual int dragGetNumTypes() { return 0; }
  // return the name of the type for this slot
  virtual const char *dragGetType(int slot) { return NULL; }
  virtual int dragCheckOption(int option) { return 0; }
};

class DI {
public:
  DI(RootWnd *rw) {
    if (rw == NULL) di = NULL;
    else di = rw->getDragInterface();
  }
  int dragEnter(RootWnd *sourceWnd) {
    return di ? di->dragEnter(sourceWnd) : 0;
  }
  int dragOver(int x, int y, RootWnd *sourceWnd) {
    return di ? di->dragOver(x, y, sourceWnd) : 0;
  }
  int dragSetSticky(RootWnd *wnd, int left, int right, int up, int down) {
    return di ? di->dragSetSticky(wnd, left, right, up, down) : 0;
  }
  int dragLeave(RootWnd *sourceWnd) {
    return di ? di->dragLeave(sourceWnd) : 0;
  }
  int dragDrop(RootWnd *sourceWnd, int x, int y) {
    return di ? di->dragDrop(sourceWnd, x, y) : 0;
  }

  const char *dragGetSuggestedDropTitle(void) {
    return di ? di->dragGetSuggestedDropTitle() : NULL;
  }
  int dragCheckData(char *type, int *nitems=NULL) {
    return di ? di->dragCheckData(type, nitems) : 0;
  }
  void *dragGetData(int slot, int itemnum) {
    return di ? di->dragGetData(slot, itemnum) : NULL;
  }
  // return the number of data type slots stored
  int dragGetNumTypes() {
    return di ? di->dragGetNumTypes() : 0;
  }
  // return the name of the type for this slot
  virtual const char *dragGetType(int slot) {
    return di ? di->dragGetType(slot) : NULL;
  }
  int dragCheckOption(int option) {
    return di ? di->dragCheckOption(option) : 0;
  }
private: 
  DragInterface *di;
};

#endif
