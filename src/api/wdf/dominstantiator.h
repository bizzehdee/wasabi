#ifndef DOMINSTANTIATOR_H
#define DOMINSTANTIATOR_H

#include "dom/domdefs.h"

class DOMInstantiator;
class DefinitionsFramework;

class DOMInstantiator {
  public:
    DOMInstantiator();
    void setFramework(DefinitionsFramework *framework);
  
    virtual void *instantiate(DOMNode *node, void *parentObj, void **parentOverrideForNodeChildren)=0;
    virtual void link(DOMNode *parentNode, void *parentObject, DOMNode *childNode, void *childObject)=0;

    void *instantiateNode(DOMNode *node, DOMInstantiator *instantiator);
    void *instantiateNodeChildren(DOMNode *node, DOMInstantiator *instantiator, void *parentObject);
    DefinitionsFramework *getFramework();

  protected:
    void *doChainedInstantiation(DOMNode *node, DOMInstantiator *instantiator, void *parentObj, bool onlyChildren=false, void *childrensParentOverride=NULL);
    DefinitionsFramework *m_framework;
    void *m_topLevel;
};

#endif
