#ifndef _WASABI_DOMNODE_H
#define _WASABI_DOMNODE_H

#include "bfc/std.h"
#include "bfc/string/string.h"
#include "bfc/guid.h"
#include "bfc/map.h"

#include "parse/node.h"
#include "api/xml/xmlobject.h"

#include "api/xml/xmlreader.h"
#include "api/xml/xmlmanager.h"
#include "api/xml/xmlmanagedobj.h"
#include "bfc/string/bigstring.h"

#include "domnodelist.h"
#include "domnodemap.h"
#include "domdefs.h"

class DOMNode: public Node, public XmlObjectI {

public:
  DOMNode();
  DOMNode(const char *nodename);
  virtual ~DOMNode();

public:

  // Node
  //
  //Adds the node newChild to the end of the list of children of this node.
  virtual DOMNode *appendChild(DOMNode *newChild);
  //Returns a duplicate of this node, i.e., serves as a generic copy constructor for nodes.
  virtual DOMNode *cloneNode(bool deep);
  //A NamedNodeMap containing the attributes of this node (if it is an Element) or null otherwise.
  virtual DOMNamedNodeMap *getAttributes();
  //A NodeList that contains all children of this node.
  virtual DOMNodeList *getChildNodes();
  //The first child of this node.
  virtual DOMNode *getFirstChild();
  //The last child of this node.
  virtual DOMNode *getLastChild();
  //Returns the local part of the qualified name of this node.
  virtual const char *getLocalName();
  //The namespace URI of this node, or null if it is unspecified.
  virtual const char *getNamespaceURI();
  //The node immediately following this node.
  virtual DOMNode	*getNextSibling();
  //The name of this node, depending on its type;
  virtual const char *getNodeName();
  //A code representing the type of the underlying object, as defined above.
  virtual short getNodeType();
  //The value of this node, depending on its type; see the table above.
  virtual const char *getNodeValue();
  //The Document object associated with this node.
  virtual DOMDocument *getOwnerDocument();
  //The parent of this node.
  virtual DOMNode *getParentNode();
  //The namespace prefix of this node, or null if it is unspecified.
  virtual const char *getPrefix();
  //The node immediately preceding this node.
  virtual DOMNode *getPreviousSibling();
  //Returns whether this node (if it is an element) has any attributes.
  virtual bool hasAttributes();
  //Returns whether this node has any children.
  virtual bool hasChildNodes();
  //Inserts the node newChild before the existing child node refChild.
  virtual DOMNode *insertBefore(DOMNode *newChild, DOMNode *refChild);
  //Tests whether the DOM implementation implements a specific feature and that feature is supported by this node.
  virtual bool isSupported(const char *feature, const char *version);
  //Puts all Text nodes in the full depth of the sub-tree underneath this Node, including attribute nodes, into a "normal" form where only structure (e.g., elements, comments, processing instructions, CDATA sections, and entity references) separates Text nodes, i.e., there are neither adjacent Text nodes nor empty Text nodes.
  virtual void normalize();
  //Removes the child node indicated by oldChild from the list of children, and returns it.
  virtual DOMNode *removeChild(DOMNode *oldChild);
  //Replaces the child node oldChild with newChild in the list of children, and returns the oldChild node.
  virtual DOMNode	*replaceChild(DOMNode *newChild, DOMNode *oldChild);
  //The value of this node, depending on its type; see the table above.
  virtual void setNodeValue(const char *nodeValue);
  //The namespace prefix of this node, or null if it is unspecified.
  virtual void setPrefix(const char *prefix);

//private:
  int loadXml(const char *S, int flags = 0, int* ks=NULL);
  virtual DOMNode * xmlTestPath(const char * xmlpath, const char * xmltag, const char *file, const char *incpath, int line);
  virtual void xmlSetTag(const char * xmlpath, const char * xmltag);
  virtual void xmlSetData(const char * xmlpath, const char * xmltag, const char * xmldata, const char *file, const char *incpath, int line);
  virtual void xmlSetParams(const char * xmlpath, const char * xmltag, XmlReaderParams * xmlparams);
  virtual void xmlComplete(const char * xmlpath, const char * xmltag);
  virtual void setError();

  void processName();
  DOMNode *getIssuer();
  void setIssuer(DOMNode *node);
  void unIssue(DOMNode *node);

  void addObject(GUID objectGuid, void *objectPointer);
  void removeObject(GUID objectGuid);
  void *findObjectByGuid(GUID objectGuid);
  const char *getId();
  void clearIdCache();
  void setSource(const char *file, const char *incpath, int line);
  const char *getSourceFile();
  const char *getIncludePath();
  int getSourceLine();

private:
  void insertNodeAt(DOMNode *node, int pos);
 
protected:
  String m_nodeName;
  String m_nodePrefix;
  String m_namespaceURI;
  String m_nodeValue;
  String m_nodeLocalName;
  DOMDocument *m_owner;
  DOMNodeList m_issued;
  DOMNode *m_issuer;
  DOMNodeList m_nodelist;
  DOMNamedNodeMap m_attrs;
  int m_error;
  int m_type;
  String m_cachedId;
  Map<GUID, void *, GUIDSort> m_objects;

  // TODO: make this use StringTable instead, to minimize the amount of storage required by all the
  // references to the source file. The problem is where do we put the table ? If in the owner document
  // then we have to be careful about node reparenting, and if we make a global stringtable for all docs,
  // then who frees its content. For now a String will do.
  String m_file;
  String m_incpath;
  int m_line;
};

class SortNodeByName {
public:
  // comparator for sorting
  static int compareItem(DOMNode *p1, DOMNode* p2) {
    return STRICMP(p1->getNodeName(), p2->getNodeName());
  }
  // comparator for searching
  static int compareAttrib(const char *attrib, DOMNode *item) {
    return STRICMP(attrib, item->getNodeName());
  }
};


#endif
