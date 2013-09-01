#ifndef _WASABI_DOMCHARDATA_H
#define _WASABI_DOMCHARDATA_H

#include "domnode.h"


class DOMCharData: public DOMNode {
protected:
  DOMCharData(){}
  virtual ~DOMCharData(){}
public:
 //Append the string to the end of the character data of the node.
 void appendData(const char *arg);
 //Remove a range of byte from the node.
 void deleteData(int offset, int count);
 // The character data of the node that implements this interface.
 const char *getData();
 // The number of bytes that are available through data and the substringData method below.
 int getLength();
 // Insert a string at the specified byte offset.
 void insertData(int offset, const char *arg);
 // Replace the characters starting at the specified 16-bit unit offset with the specified string.
 void replaceData(int offset, int count, const char *arg);
 // The character data of the node that implements this interface.
 void setData(const char *data);
 // Extracts a range of data from the node.
 const char *substringData(int offset, int count);

};

#endif