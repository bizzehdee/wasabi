/*

  Nullsoft WASABI Source File License

  Copyright 1999-2003 Nullsoft, Inc.

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.

*/
#include "precomp.h"
#include "xmlobject.h"

#define CBCLASS XmlObjectI
START_DISPATCH;
  CB(SETXMLPARAM,      setXmlParam);
  CB(GETNUMXMLPARAMS,  getNumXmlParams);
  CB(GETXMLPARAMNAME,  getXmlParamName);
  CB(GETXMLPARAMVALUE, getXmlParamValue);
  CB(GETXMLPARAM,      getXmlParam);
END_DISPATCH;

XmlObjectI::XmlObjectI() {
  handlepos = 0;
}

XmlObjectI::~XmlObjectI() {
  params.deleteAll();
}

void XmlObjectI::addXmlParam(int xmlhandle, const char *xmlattribute, int xmlattributeid, int isrequired) {
  params.addItem(new XmlObjectParam(xmlhandle, xmlattribute, xmlattributeid, isrequired));
}

int XmlObjectI::setXmlParamById(int xmlhandle, int xmlattribute, const char *param, const char *value) {
  return 0;
}

int XmlObjectI::setXmlParam(const char *param, const char *value) {
  int pos = -1;
  int r = 0;
  params.findItem(param, &pos);
  if (pos >= 0) {
    XmlObjectParam *xuop = params.enumItem(pos);
    ASSERT(xuop != NULL);
    r = setXmlParamById(xuop->getXmlHandle(), xuop->getXmlAttributeId(), param, value);
    xuop->setLastValue(value);
  } else {
    onUnknownXmlParam(param, value);
  }
  return r;
}

int XmlObjectI::getNumXmlParams() {
  return params.getNumItems();
}

const char *XmlObjectI::getXmlParamName(int n) {
  return params.enumItem(n)->getXmlAttribute();
}

const char *XmlObjectI::getXmlParamValue(int n) {
  return params.enumItem(n)->getLastValue();
}

int XmlObjectI::getXmlParam(const char *param) {
  int pos=-1;
  params.findItem(param, &pos);
  return pos;
}

const char *XmlObjectI::getXmlParamByName(const char *paramname) {
  int pos = getXmlParam(paramname);
  if (pos < 0) return NULL;
  return getXmlParamValue(pos);
}

int XmlObjectI::newXmlHandle() {
  return handlepos++;
}

int XmlObjectI::onUnknownXmlParam(const char *paramname, const char *strvalue) {
  return 0;
}
