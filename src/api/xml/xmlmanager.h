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
#ifndef _XMLMANAGER_H
#define _XMLMANAGER_H

#include "api/xml/xmlreader.h"
#include "api/xml/xmlstrparams.h"
#include "bfc/ptrlist.h"

//
//
// You can, if you like, ignore the contents of this object.  They are listed
// here in the header only because the object is implemented as a template.
//
// Comments here are provided only as a courtesy to the reader.
//
// Please read the file "xmlmanagedobj.h" for the rules governing objects
// placed into this assembly container.
//
template <class TBaseclass>
class XmlManager : public XmlReaderCallbackI {
public:
  // There is no void constructor.  You MUST give this
  // container its firstObject upon construction.
  XmlManager(TBaseclass * firstObject) : 
      nullPath(), handlingNullPath( 0 ), objectStack(), XmlReaderCallbackI()
  {
     objectStack.addItem( firstObject );  // stack push
  }

  virtual void	xmlReaderOnStartElementCallback(const char *xmlpath, const char *xmltag, XmlReaderParams *params)
  {
    // handlingNullPath is set when the TBaseclass object returns NULL when
    // asked to test the newly incoming tag and path.
    if (!handlingNullPath) {
      TBaseclass * topObject = objectStack.getLast(); // stack read
      TBaseclass * newObject = topObject->xmlTestPath(xmlpath, xmltag, xmlGetFileName(), xmlGetIncludePath(), xmlGetFileLine());

      if (newObject == NULL) {
        // we ignore any new start-elements until we receive the end-element
        // for the path at which we began handlingNullPath
        handlingNullPath = 1;
        nullPath = xmlpath;
      } else {
        // otherwise, we push newObject down on our internal stack and
        // send him the beginning of his info.
        objectStack.addItem( newObject );  // stack push
        newObject->xmlSetTag(xmlpath,xmltag);
        newObject->xmlSetParams(xmlpath,xmltag,params);
      }
    }
  }

  virtual void	xmlReaderOnEndElementCallback(const char *xmlpath, const char *xmltag) {
    if (!handlingNullPath) {
      // Tell the topObject that it's done now.
      TBaseclass * topObject = objectStack.getLast(); // stack read
      topObject->xmlComplete(xmlpath,xmltag);
      // And pop the stack.
      objectStack.removeLastItem(); // stack pop
    } else {
      if (nullPath == xmlpath) {
        // we're now done handling the nullPath
        handlingNullPath = 0;
      }
    }
  }

  virtual void	xmlReaderOnCharacterDataCallback(const char *xmlpath, const char *xmltag, const char *xmldata) {
    if (!handlingNullPath) {
      // Tell the topObject that it's got data
      TBaseclass * topObject = objectStack.getLast(); // stack read
      topObject->xmlSetData(xmlpath,xmltag,xmldata,xmlGetFileName(),xmlGetIncludePath(),xmlGetFileLine());
    }
  }

  virtual void xmlReaderOnError(const char *filename, int linenum, const char *incpath, int errcode, const char *errstr) {
		TBaseclass * topObject = objectStack.getFirst();
		topObject->setError();
  }

  virtual int xmlReaderDisplayErrors() { return 0; }

	// ========================================================================
private:
  String                nullPath;
  int                   handlingNullPath;
	PtrList<TBaseclass>   objectStack;
};



#endif//_XMLMANAGER_H