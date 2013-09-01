#ifndef __DOMCOMMENT_H
#define __DOMCOMMENT_H

#include "domchardata.h"

class DOMComment: public DOMCharData  {
public:
  DOMComment(const char *data)  {
    m_nodeName = "#comment";
    m_type = COMMENT_NODE;
    setData(data);
  }
};

#endif