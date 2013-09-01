#ifndef __DOMENTITY_H
#define __DOMENTITY_H

#include "domnode.h"

class DOMEntity: public DOMNode {
public:
  DOMEntity(const char *notationName) {
    m_nodeName = notationName;
    m_type = ENTITY_NODE;
  }
  //For unparsed entities, the name of the notation for the entity.
  const char *getNotationName() { return m_nodeName;  }
  //The public identifier associated with the entity, if specified.
  const char *getPublicId() { return m_publicId; }
  //The system identifier associated with the entity, if specified.
  const char *getSystemId() { return m_systemId; }
private:
  String m_publicId;
  String m_systemId;

};

#endif