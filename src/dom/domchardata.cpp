#include "precomp.h"
#include "domchardata.h"


void DOMCharData::appendData(const char *arg)  {
  m_nodeValue += arg;
}


void DOMCharData::deleteData(int offset, int count)  {
 String temp = m_nodeValue.rSplit(offset);
 m_nodeValue += temp.rSplit(count);
}


const char *DOMCharData::getData()  {
  return m_nodeValue.getValue();
}


int DOMCharData::getLength()  {
  return m_nodeValue.len();
}


void DOMCharData::insertData(int offset, const char *arg)  {
  String temp = m_nodeValue.rSplit(offset);
  m_nodeValue += arg;
  m_nodeValue += temp;
}


void DOMCharData::replaceData(int offset, int count, const char *arg)  {
  String temp = m_nodeValue.rSplit(offset);
  m_nodeValue += arg;
  m_nodeValue += temp.rSplit(count);
}


void DOMCharData::setData(const char *data)  {
  m_nodeValue = data;
}


const char *DOMCharData::substringData(int offset, int count)  {
  return m_nodeValue.substr(offset,count);
}

