#ifndef __DOMDOCFRAGMENT_H
#define __DOMDOCFRAGMENT_H

#include "domnode.h"

class DOMDocumentFragment: public DOMNode  {
public:
  DOMDocumentFragment() { 
    m_nodeName = "#document-fragment";
    m_type = DOCUMENT_FRAGMENT_NODE;
  }
  DOMDocumentFragment(const char *file, int flags=0) { 
    m_nodeName = "#document-fragment";
    m_type = DOCUMENT_FRAGMENT_NODE;
    loadXml(file,flags); 
  }
};

#endif