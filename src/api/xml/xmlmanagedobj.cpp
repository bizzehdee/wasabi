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

#include "api/xml/xmlmanagedobj.h"

//
//  Now, if you actually derive from XmlManagedObjBase (or even use it
// by itself, without any derivation), the default functionality will
// be to create a new XmlManagedObjBase object for every element extant
// in your XML file, fill each of those objects with their required data,
// and assemble all of the objects into a node-hierarchy that replicates
// the element hierarchy of your XML file.
//
//  If you don't want to do this, don't subclass from this object.  Simply
// create your own object family that provides the following methods and 
// feed your personalized object baseclass to the template.
//
XmlManagedObjBase * XmlManagedObjBase::xmlTestPath(const char * xmlpath, const char * xmltag, const char *file, const char *incpath, int line) {
  // The default functionality here is to simply dump out a new object for every element.
  XmlManagedObjBase * newNode = new XmlManagedObjBase();
  // Which, because we derive from node, we add to our internal child list.
  addChild( newNode );
  // And then return to the XmlManager to become the new "active object"
  return newNode;
}

void XmlManagedObjBase::xmlSetTag(const char * xmlpath, const char * xmltag) {
  tag = xmltag;
}

void XmlManagedObjBase::xmlSetData(const char * xmlpath, const char * xmltag, const char * xmldata, const char *file, int line) {
  data = xmldata;
}

void XmlManagedObjBase::xmlSetParams(const char * xmlpath, const char * xmltag, XmlReaderParams * xmlparams) {
  params = *xmlparams; // note that we're assigning from one class type to another.  
}

void XmlManagedObjBase::xmlComplete(const char * xmlpath, const char * xmltag) {
  // We do nothing!  We don't care!  Hedonism, baby... pure hedonism.
}
