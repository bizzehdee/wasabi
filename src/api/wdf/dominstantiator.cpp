#include <precomp.h>
#include <dom/domnode.h>
#include "dominstantiator.h"

DOMInstantiator::DOMInstantiator() {
  m_framework = NULL;
  m_topLevel = NULL;
}

void DOMInstantiator::setFramework(DefinitionsFramework *framework) {
  m_framework = framework;
}

DefinitionsFramework *DOMInstantiator::getFramework() {
  return m_framework;
}

void *DOMInstantiator::instantiateNode(DOMNode *node, DOMInstantiator *instantiator) {
  if (!m_framework) return NULL;
  return doChainedInstantiation(node, instantiator, NULL); 
}

void *DOMInstantiator::doChainedInstantiation(DOMNode *node, DOMInstantiator *instantiator, void *parentObj, bool onlyChildren, void *childrensParentOverride) {
  void *parentOverride = childrensParentOverride;
  void *object = NULL;
  if (!onlyChildren) {
    // instantiate this object
    object = instantiate(node, parentObj, &parentOverride);
    if (!m_topLevel) m_topLevel = object;
    // bypass this branch if not an object
    if (!object) 
      return NULL;
    // link to parent
    if (parentObj)
      link(node->getParentNode(), parentObj, node, object);
  }
  // recurse on all children
  DOMNodeList *children = node->getChildNodes();
  for (int i=0; i<children->getLength(); i++) {
    DOMNode *child = children->item(i);
    doChainedInstantiation(child, 
                           instantiator, 
                           parentOverride ? parentOverride : object);
  }
  return object;
}

