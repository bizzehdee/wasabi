#include <precomp.h>
#include "depend.h"

DependentViewerI::DependentViewerI() : viewed_items(NULL) { }

DependentViewerI::DependentViewerI(const DependentViewerI &dep) :
  viewed_items(NULL) {
  // watch their items now
  if (dep.viewed_items != NULL) {
    for (int i = 0; i < dep.viewed_items->getNumItems(); i++)
      viewer_addViewItem(dep.viewed_items->enumItem(i));
  }
}

DependentViewerI::~DependentViewerI() {
  if (viewed_items != NULL) {
    // tell viewed items to detach from us
    foreach(viewed_items)
      viewed_items->getfor()->dependent_regViewer(this, FALSE);
    endfor
#if 0
    for (int i = 0; i < viewed_items->getNumItems(); i++) {
      Dependent *item = viewed_items->enumItem(i);
      item->dependent_regViewer(this, FALSE);
    }
#endif
    viewed_items->removeAll();
    delete viewed_items;
  }
}

DependentViewerI &DependentViewerI::operator =(const DependentViewerI &dep) {
  if (this != &dep) {
    viewer_delAllItems();
    // watch their items now
    if (dep.viewed_items != NULL) {
      for (int i = 0; i < dep.viewed_items->getNumItems(); i++)
        viewer_addViewItem(dep.viewed_items->enumItem(i));
    }
  }
  return *this;
}

void DependentViewerI::viewer_addViewItem(Dependent *item) {
  ASSERT(item != NULL);
  if (viewed_items == NULL) viewed_items = new DependentList(8);
  if (viewed_items->haveItem(item)) return;
  viewed_items->addItem(item);
  item->dependent_regViewer(this, TRUE);
}

void DependentViewerI::viewer_delViewItem(Dependent *item) {
  if (item == NULL) return;
  if (viewed_items == NULL || !viewed_items->haveItem(item)) return;
  item->dependent_regViewer(this, FALSE);
  viewed_items->removeItem(item);
  if (viewed_items->getNumItems() == 0) {
    delete viewed_items; viewed_items = NULL;
  }
}

void DependentViewerI::viewer_delAllItems() {
  while (viewed_items != NULL) {
    Dependent *item = viewer_enumViewItem(0);
    if (item == NULL) break;
    viewer_delViewItem(item);
  }
  delete viewed_items;
  viewed_items = NULL;
}

void DependentViewerI::viewer_delAllOfClass(const GUID *guid) {
  if (viewed_items == NULL) return;
  foreach(viewed_items)
    if (viewed_items->getfor()->dependent_getInterface(guid))
      viewer_delViewItem(viewed_items->getfor());
  endfor
}

Dependent *DependentViewerI::viewer_enumViewItem(int which) {
  if (viewed_items == NULL) return NULL;
  return viewed_items->enumItem(which);
}

int DependentViewerI::viewer_getNumItems() {
  if (viewed_items == NULL) return 0;
  return viewed_items->getNumItems();
}

int DependentViewerI::viewer_haveItem(Dependent *item) {
  if (viewed_items == NULL) return 0;
  return viewed_items->haveItem(item);
}

int DependentViewerI::dependentViewer_callback(Dependent *item, const GUID *classguid, int cb, int param1, int param2, void *ptr, int ptrlen) {
  if (viewed_items == NULL || !viewed_items->haveItem(item)) return 0;
  switch (cb) {
    case DependentCB::DEPCB_NOP: return 1;

    case DependentCB::DEPCB_DELETED:
      viewed_items->removeItem(item);
      return viewer_onItemDeleted(item);

    case DependentCB::DEPCB_EVENT:
      return viewer_onEvent(item, classguid, param1, param2, ptr, ptrlen);
    break;
  }
  return 0;
}

DependentI::DependentI(const GUID *classguid) : viewers(NULL), class_guid(classguid ? *classguid : INVALID_GUID) {
}

DependentI::DependentI(const DependentI &dep) : viewers(NULL), class_guid(dep.class_guid) { }

DependentI::~DependentI() {
  if (viewers != NULL) {
    foreach(viewers)
      viewers->getfor()->dependentViewer_callback(this, &class_guid, DependentCB::DEPCB_DELETED);
    endfor
    delete viewers;
  }
}

DependentI& DependentI::operator =(const Dependent &dep) {
  // placeholder code... nobody asked to watch us
  if (this != &dep) {
    viewers = NULL;
  }
  return *this;
}

void DependentI::dependent_regViewer(DependentViewer *viewer, int add) {
  if (viewer == NULL) return;
  if (add) {
    if (viewers == NULL) viewers = new ViewerList(8);
    if (viewers->haveItem(viewer)) return;
    viewers->addItem(viewer);
    viewer->dependentViewer_callback(this, &class_guid, DependentCB::DEPCB_NOP);
  } else {	// delete
    if (viewers == NULL) return;
    viewers->removeItem(viewer);
    if (viewers->getNumItems() == 0) {
      delete viewers; viewers = NULL;
    }
  }
  dependent_onRegViewer(viewer, add);
}

void DependentI::dependent_sendEvent(const GUID *classguid, int event, int param, void *ptr, int ptrlen, DependentViewer *viewer) {
  if (classguid == NULL) classguid = &class_guid;	// use default
  ASSERT(*classguid != INVALID_GUID);
  sendViewerCallbacks(classguid, DependentCB::DEPCB_EVENT, event, param, ptr, ptrlen, viewer);
}

void DependentI::sendViewerCallbacks(const GUID *classguid, int msg, int param1, int param2, void *ptr, int ptrlen, DependentViewer *viewer) {
  if (viewers == NULL) return;
  ASSERT(classguid != NULL);
  foreach(viewers)
    if (viewer == NULL || viewer == viewers->getfor())
      viewers->getfor()->dependentViewer_callback(this, classguid, msg, param1, param2, ptr, ptrlen);
  endfor
}
