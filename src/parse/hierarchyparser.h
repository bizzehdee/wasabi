#ifndef __PARAMPARSE_H
#define __PARAMPARSE_H

#include <parse/pathparse.h>
#include <bfc/string/string.h>

#include "node.h"

// typedef NodeC<String> HPNode   // OH YAH, YOU CAN'T FWD REF TYPEDEFS.  STOOOOOOPID.
class HPNode : public NodeC<String> {
public:
  HPNode( const String & myPayload, NodeC<String> * myParent = NULL ) : NodeC<String>(myPayload, myParent) {}
};

class COMEXP HierarchyParser {
public:    
  HierarchyParser(const char *str = NULL, const char *_sibling=";", const char *_escape="\\", const char *_parent_open="(", const char *_parent_close=")") ;
  ~HierarchyParser();

  HPNode *findGuid(GUID g);
  HPNode *findString(const char *str);

  int hasGuid(GUID g) { return findGuid(g) != NULL; }
  int hasString(const char *str) { return findString(str) != NULL; }

  HPNode *rootNode() { return rootnode; }

private:
  HPNode *rootnode;
  int myalloc;

  String sibling;
  String escape;
  String parent_open;
  String parent_close;

  HierarchyParser(HPNode *_rootnode, const char *_sibling=";", const char *_escape="\\", const char *_parent_open="(", const char *_parent_close=")");
  void processSibling(const char *sibling);


  inline int isSibling(char c) {
    return sibling.lFindChar(c) != -1;
  }
  inline int isEscape(char c) {
    return escape.lFindChar(c) != -1;
  }
  inline int isParentOpen(char c) {
    return parent_open.lFindChar(c) != -1;
  }
  inline int isParentClose(char c) {
    return parent_close.lFindChar(c) != -1;
  }
};

#endif
