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
#ifndef _XMLMANAGEDOBJ_H
#define _XMLMANAGEDOBJ_H

#include "parse/node.h"
#include "api/xml/xmlstrparams.h"

// ============================================================================
//
//  class XmlManagedObjBase
//
// This object, while provided as a baseclass for use with the XmlManager object,
// is only a representational example of an object to be used with XmlManager.
//
// Because XmlManager is a template, you may use any object you like with the
// system.  However, you must provide all of the Test, Set, and Complete methods
// as they are specified within this object.
//
// Of course, you should feel free to use this object as a real baseclass for
// your own functionality if you so desire the functionality provided by this
// class.
//
// PLEASE NOTE:  The comments provided in THIS header only serve to describe the
// basic functionality a proper XmlManaged baseclass must provide.  The comments
// in the CPP file describe the default functionality offered by this specific
// class if used as a baseclass.  Be sure to read them if you inherit from
// this class.
//
class XmlManagedObjBase : public Node {
public:
  // As the XmlReader sends a StartElement call to the XmlManager,
  // the XmlManager asks the current XmlManagedObject what to do 
  // with the incoming path.  There are three valid return values
  // for this method.  You may return:
  //
  //     1 -- A new pointer that you create (and that you will 
  //          have to destruct).  This object will become the
  //          new "current object."
  //     2 -- Your 'this' pointer.  This object will remain the
  //          "current object."
  //     3 -- NULL.  By returning NULL, this path and all elements
  //          beneath this path will be ignored by the reader.
  virtual XmlManagedObjBase * xmlTestPath(const char * xmlpath, const char * xmltag, const char *file, const char *incpath, int line);

  //
  // The pointer returned by the 'TestPath' method will be immediately called
  // on its SetTag and SetParams methods.  Later, the object will be called on
  // its SetData and Complete methods, but it very well might be called again
  // upon 'TestPath' if the XML data drops to a deeper level of hierarchy.
  virtual void xmlSetTag(const char * xmlpath, const char * xmltag);
  virtual void xmlSetData(const char * xmlpath, const char * xmltag, const char * data, const char *file, int line);
  virtual void xmlSetParams(const char * xmlpath, const char * xmltag, XmlReaderParams * params);
  virtual void xmlComplete(const char * xmlpath, const char * xmltag);
  //
  //  IMPORTANT NOTE:
  //    If you return 'this' from xmlTestPath, your object will receive MULTIPLE
  //  calls upon its "Set" and "Complete" methods.  One call upon each method,
  //  for each time the same pointer is returned by TestPath. 
  //
  //    You must bear in mind that the order of these calls will depend entirely 
  //  on the hierarchy structure of your XML file, and that you are likely to
  //  receive multiple xmlSetTag calls, say, before receiving multiple calls
  //  to the xmlComplete method.

public:
  String        tag;
  String        data;
  XmlStrParams  params;
};

#endif//_XMLMANAGEDOBJ_H