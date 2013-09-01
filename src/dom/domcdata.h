#ifndef __DOMCDATA_H
#define __DOMCDATA_H

#include "domtext.h"

class DOMCDATASection: public DOMText {
public:
  DOMCDATASection(const char *data): DOMText(data) {
    m_nodeName = "#cdata-section";
    m_type = CDATA_SECTION_NODE;
  }
};

#endif