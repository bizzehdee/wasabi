#ifndef _MULTIMAP_H
#define _MULTIMAP_H

#include <bfc/map.h>

// this is a specialized map from a data type to a ptrlist of another type
// I love C++ :) --BU

template <class INDEX, class DATA>
class MultiMap : private Map<INDEX, PtrList<DATA>* > {
  typedef Map<INDEX, PtrList<DATA>* > PMap;
  typedef PtrList<DATA> MList;
public:
  void multiAddItem(const INDEX &a, DATA *ptr, int nodup=FALSE) {
    MList *list = getListForIndex(a, TRUE);
    if (!(nodup && list->haveItem(ptr)))
      list->addItem(ptr);
  }
  using Map<INDEX, PtrList<DATA>* >::getNumItems;
  int multiGetItem(const INDEX &a, int n, DATA **pptr) {
    ASSERT(pptr != NULL);
    MList *list = getListForIndex(a, FALSE);
    if (list == NULL) return 0;	// don't have any

    DATA* dp = list->enumItem(n);
    if (dp == NULL) return 0;	// not on list at that position
    *pptr = dp;
    return 1;
  }
  int multiGetItemDirect(int pos, int n, DATA **pptr) {
    MList *list = PMap::enumItemByPos(pos, NULL);
    if (list == NULL) return 0;
    DATA *rptr = list->enumItem(n);
    if (rptr == NULL) return 0;
    *pptr = rptr;
    return 1;
  }
  int multiGetNumItems(const INDEX &a) {
    MList *list = getListForIndex(a, FALSE);
    if (list == NULL) return 0;	// don't have any
    return list->getNumItems();
  }
  int multiGetNumPairs() { return PMap::getNumPairs(); }
  int multiHaveItem(const INDEX &a, DATA *ptr) {
    MList *list = getListForIndex(a, FALSE);
    if (list == NULL) return 0;	// don't have any
    return list->haveItem(ptr);
  }
  int multiDelItem(const INDEX &a, DATA *ptr) {
    MList *list = getListForIndex(a, FALSE);
    if (list == NULL) return 0;
    return list->delItem(ptr);
  }
  // delete entire list for item
  int multiDelAllForItem(const INDEX &a, int call_destructors=FALSE) {
    MList *list = getListForIndex(a, FALSE);
    if (list == NULL) return 0;
    PMap::delItem(a);
    int r = list->getNumItems();
    if (call_destructors) list->deleteAll();
    delete list;
    return r;
  }
  int multiDeleteAll() {
    for (int i=0;i<PMap::getNumItems();i++) {
      MList *list = PMap::enumItemByPos(i, NULL);
      list->deleteAll();
      delete list;
    }
    PMap::deleteAll();
    return 1;
  }
  int multiRemoveAll() {
    for (int i=0;i<PMap::getNumItems();i++) {
      MList *list = PMap::enumItemByPos(i, NULL);
      list->removeAll();
      delete list;
    }
    PMap::deleteAll();
    return 1;
  }

  void multiDelAllDataEntries(const DATA *ptr) {
    for (int i = 0; ; i++) {
      String index = PMap::enumIndexByPos(i, String(""));
      if (index.isempty()) break;
      MList *list = getListForIndex(index, FALSE);
      ASSERT(list != NULL);
      list->removeEveryItem(ptr);
    }
  }

  const MList *getListForIndex(const INDEX &a) {
    return getListForIndex(a, FALSE);
  }

  void purge() {
    PMap::list.purge();
  }

private:
  MList *getListForIndex(const INDEX &a, int addifnotfound) {
    MList *list = NULL;
    if (PMap::getItem(a, &list) == 0) {
      if (addifnotfound) {
        list = new MList;
        PMap::addItem(a, list);
      }
    }
    return list;
  }
};

#endif
