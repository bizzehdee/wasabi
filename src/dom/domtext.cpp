#include "precomp.h"
#include "domtext.h"
#include "domdoc.h"

DOMText::DOMText(const char *data) {
  m_type = TEXT_NODE;
  m_nodeName = "#text";
  setData(data);
}

DOMText *DOMText::splitText(int offset)  {
  DOMText *res = new DOMText(m_nodeValue.rSplit(offset));
  if ( res )  {
    m_issued.addItem(res);
    if ( m_owner ) {
      ((DOMDocument *)m_owner)->insertBefore(res, getNextSibling());
    }
  }
  return res;
}