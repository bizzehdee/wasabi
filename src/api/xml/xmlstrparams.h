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
#ifndef _XMLSTRPARAMS_H
#define _XMLSTRPARAMS_H

#include "bfc/pair.h"
#include "bfc/ptrlist.h"
#include "bfc/string/string.h"
#include "api/xml/xmlreader.h"
#include "api/xml/xmlparams.h"

// ===========================================================================
//
//		class XmlStrParams
//
// The current params object passed through the XML parser will be
// deleted and cannot easily be used to copy the values for keeping.
// This object will accept an XmlReaderParamsI object and suck all
// the data from it to be kept in a PtrList< Pair<String, String> >
class XmlStrParams {
protected:
	PtrList< Pair<String,String> >				params;
public:
	XmlStrParams() : params() {}
	XmlStrParams(const XmlStrParams & T) : params() {
		int i, num = T.getNumItems();
		for (i = 0; i < num; i ++) {
			addItem(T.getItemName(i), T.getItemValue(i));
		}
	}
	XmlStrParams(XmlReaderParams & T) : params() {
		int i, num = T.getNbItems();
		for (i = 0; i < num; i ++) {
			addItem(T.getItemName(i), T.getItemValue(i));
		}
	}
	~XmlStrParams() {
		params.deleteAll();
	}

	XmlStrParams & operator = (XmlReaderParams & T) {
		int i, num = T.getNbItems();
		for (i = 0; i < num; i ++) {
			addItem(T.getItemName(i), T.getItemValue(i));
		}
    return *this;
	}

	const char * getItemName(int i) const {
		return params.enumItem(i)->a;
	}

	const char * getItemValue(int i) const {
		return params.enumItem(i)->b;
	}

  // Linear search.  Feh.
	const char * getItemValue(const char * name) const {
    int i, num = getNumItems();
    for (i = 0; i < num; i ++) {
      if (params.enumItem(i)->a == name) {
        return params.enumItem(i)->b;
      }
    }    
    return NULL;
  }

  int getItemIndex(const char * name) const {
    int i, num = getNumItems();
    for (i = 0; i < num; i ++) {
      if (params.enumItem(i)->a == name) {
        return i;
      }
    }
    return -1;
  }

	int getNumItems() const {
		return params.getNumItems();
	}

	void addItem(const char * name, const char * value) {
		Pair<String, String> * nextItem = new Pair<String, String>(String(name), String(value));
		params.addItem(nextItem);
	}

	void addItem(const String & name, const String & value) {
		Pair<String, String> * nextItem = new Pair<String, String>(name, value);
		params.addItem(nextItem);
	}
};

#endif//_XMLSTRPARAMS_H
