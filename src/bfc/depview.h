#ifndef _DEPVIEW_H
#define _DEPVIEW_H

#include <bfc/depend.h>
#include <bfc/map.h>

// this handles classes that use the getDependencyPtr() method instead of a
// public cast

template <class VT>
class NOVTABLE DependentViewerTPtr : private DependentViewerI {
protected:
  DependentViewerTPtr(VT *first=NULL) {
    _addViewItem(first);
  }

  void viewer_addViewItem(VT *item) { _addViewItem(item); }
  void viewer_delViewItem(VT *item) { _delViewItem(item); }
  using DependentViewerI::viewer_delAllItems;

  VT *viewer_enumViewItem(int which) {
    return lookup(DependentViewerI::viewer_enumViewItem(which));
  }
  using DependentViewerI::viewer_getNumItems;
  int viewer_haveItem(VT *item) {
    return DependentViewerI::viewer_haveItem(item->getDependencyPtr());
  }

  // item you were looking at is gone: WARNING: pointer no longer valid!
  virtual int viewer_onItemDeleted(VT *item) { return 1; }
  // item you are looking at issuing an event
  virtual int viewer_onEvent(VT *item, int event, int param2, void *ptr, int ptrlen) { return 1; }

protected:
  virtual int viewer_onItemDeleted(Dependent *item) {
    return viewer_onItemDeleted(lookup(item));
  }
  virtual int viewer_onEvent(Dependent *item, const GUID *classguid, int event, int param, void *ptr, int ptrlen) {
    if (*classguid != *VT::depend_getClassGuid()) return 0;	// filter namespace
    return viewer_onEvent(lookup(item), event, param, ptr, ptrlen);
  }

private:
  VT *lookup(Dependent *d) {
    if (d == NULL) return NULL;
    VT *vt=NULL;
    int r = ptrmap.getItem(d, &vt);
    ASSERT(r);
    ASSERT(vt->getDependencyPtr() == d);
    return vt;
  }
  void _addViewItem(VT *item) {
    if (item == NULL) return;
    Dependent *d = item->getDependencyPtr();
    if (d == NULL) return;
    ptrmap.addItem(d, item);
    DependentViewerI::viewer_addViewItem(d);
  }
  void _delViewItem(VT *item) {
    if (item == NULL) return;
    Dependent *dep = NULL;
    ptrmap.reverseGetItem(item, &dep);
    int r = ptrmap.reverseDelItem(item);
    ASSERT(r);
    DependentViewerI::viewer_delViewItem(dep);
  }

  Map<Dependent *, VT *> ptrmap;
};

#endif
