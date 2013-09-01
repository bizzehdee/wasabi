#ifndef __DOMXMLMANAGER_H
#define __DOMXMLMANAGER_H

#include "domnode.h"

// add flags for parsing options
enum {
  DOMXMLMANAGER_ALLOW_INCLUDE = 1,
  DOMXMLMANAGER_WANT_COMMENTS = 1<<1, // for example
};

class DOMXmlManager: public XmlManager<DOMNode> {
public:
  DOMXmlManager(DOMNode *managed, int flags): XmlManager<DOMNode>(managed), m_flags(flags){}
  virtual int xmlReaderIncludeFiles() { return m_flags& DOMXMLMANAGER_ALLOW_INCLUDE; }
private:
  int m_flags;
};

#endif