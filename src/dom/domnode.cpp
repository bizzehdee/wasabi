#include "precomp.h"
#include "domnode.h"
#include "domelem.h"
#include "domattr.h"
#include "domtext.h"
#include "domdoc.h"
#include "domdocfragment.h"
#include "domcomment.h"
#include "domcdata.h"
#include "domentity.h"
#include "domxmlmanager.h"

DOMNode::DOMNode(): m_owner(NULL),
                    m_line(0) {
}

DOMNode::DOMNode(const char *nodename): m_owner(NULL), 
                                        m_nodeName(nodename),
                                        m_line(0) {
}


DOMNode::~DOMNode()  {
  m_issued.deleteAll();
}

void DOMNode::processName()  {
  String temp = m_nodeName;
  int pos = temp.lFindChar(':');
  if ( pos > 0 ) {
    m_nodePrefix = temp.lSpliceChar(':');
    m_nodeLocalName = temp;
  }
  else {
    m_nodePrefix = "";
    m_nodeLocalName = m_nodeName;
  }
}

int DOMNode::loadXml(const char *S, int flags, int* ks){
  DOMXmlManager myXmlManager(this,flags);
  Node * realparent = getParent();
  setParent(NULL);
  XmlReader::registerCallback("*",&myXmlManager);
  int res = 0;
  if ( STRNINCMP(S, "buf:") ) {
    String incpath= S;
    const char * file = Std::filename(S);
    int fnlen = STRLEN(file);
    incpath.trunc(-fnlen);
    res = XmlReader::loadFile(file,incpath,&myXmlManager);
  }
  else {
    res = XmlReader::loadFile(S,NULL,&myXmlManager);
  }
  XmlReader::unregisterCallback(&myXmlManager);
  setParent(realparent);
  return !m_error;
}


DOMNode *DOMNode::xmlTestPath(const char * xmlpath, const char * xmltag, const char * file, const char *incpath, int line) {
  DOMElement * newNode = new DOMElement(xmltag);
  if ( newNode )  {
    newNode->m_owner = ( m_type == DOCUMENT_NODE ) ? (DOMDocument *)this : m_owner;
    m_issued.addItem(newNode);
    newNode->setIssuer(this);
    addChild(newNode);
    newNode->setSource(file, incpath, line);
  }
  return newNode;
}

void DOMNode::xmlSetTag(const char * xmlpath, const char * xmltag) {
  m_nodeName = xmltag;
}

void DOMNode::xmlSetData(const char * xmlpath, const char * xmltag, const char * xmldata, const char *file, const char *incpath, int line) {
  DOMText * text = new DOMText(xmldata);
  if ( text ) {
    m_issued.addItem(text);
    text->setIssuer(this);
    addChild(text);
    text->setSource(file, incpath, line);
  }
}

void DOMNode::xmlSetParams(const char * xmlpath, const char * xmltag, XmlReaderParams * xmlparams) {
  int i;
  int nb = xmlparams->getNbItems();
  for (i=0;i<nb;i++)  {
    DOMAttr * attr = new DOMAttr(xmlparams->getItemName(i),(DOMElement *)this);
    if ( attr ) {
      attr->setParent(this);
      attr->setValue(xmlparams->getItemValue(i));
      m_issued.addItem(attr);
      attr->setIssuer(this);
      m_attrs.addItem(attr);
    }
  }
}

void DOMNode::xmlComplete(const char * xmlpath, const char * xmltag) {}

void DOMNode::setError(){ m_error = 1 ;}


DOMNode *DOMNode::appendChild(DOMNode *newChild)  {
  if (newChild->getNodeType() == DOCUMENT_FRAGMENT_NODE) {
    foreach(newChild->ChildList())
      DOMNode *child = (DOMNode *)(newChild->ChildList().getfor());
      insertNodeAt(child, -1);
    endfor
  } else {
    insertNodeAt(newChild, -1);
  }
  return newChild;
}


DOMNode *DOMNode::cloneNode(bool deep)  {
  if ( m_type == DOCUMENT_NODE && !deep ) return NULL; // are you looking for trouble ??
  DOMNode * res = NULL;
  switch(m_type)  {
  case ATTRIBUTE_NODE:
    res = new DOMAttr(m_nodeName);
    break;
  case TEXT_NODE:
    res = new DOMText(m_nodeValue);
    break;
  case ELEMENT_NODE:
    res = new DOMElement(m_nodeName);
    break;
  case DOCUMENT_NODE:
    res = new DOMDocument();
    break;
  case DOCUMENT_FRAGMENT_NODE:
    res = new DOMDocumentFragment();
    break;
  case COMMENT_NODE:
    res = new DOMComment(m_nodeValue);
    break;
  case ENTITY_NODE:
    res = new DOMEntity(m_nodeName);
    break;
  case CDATA_SECTION_NODE:
    res = new DOMCDATASection(m_nodeValue);
    break;
  }
  if ( res ) {
    res->m_nodeName = m_nodeName;
    res->m_nodePrefix = m_nodePrefix;
    res->m_namespaceURI = m_namespaceURI;
    res->m_nodeValue = m_nodeValue;
    res->m_owner = m_owner;
    res->m_error = m_error;
    res->m_type = m_type;

    DOMNode * item = NULL;
    DOMNode * itemclone = NULL;
    foreach(m_attrs)
      item = (DOMNode *)m_attrs.getfor();
      itemclone = (DOMNode *)item->cloneNode(deep);
      if ( itemclone ) {
        res->m_issued.addItem(itemclone);
        res->setIssuer(this);
        res->m_attrs.addItem(itemclone);
        ((DOMAttr *)itemclone)->setOwnerElement((DOMElement *)res);
      }
    endfor

    if ( deep ) {
      PtrList<Node> children(ChildList());
      foreach(children)
        item = (DOMNode *) children.getfor();
        itemclone = (DOMNode *) item->cloneNode(deep);
        if ( itemclone )  {
          res->m_issued.addItem(itemclone);
          res->setIssuer(this);
          res->addChild(itemclone);
          if ( m_type == DOCUMENT_NODE )
            itemclone->m_owner = (DOMDocument *)res;
        }
      endfor

    }
    else {
      res->m_attrs.copyFrom(m_attrs);
      res->ChildList().copyFrom(ChildList());
    }
  }
  return res;
}


DOMNamedNodeMap *DOMNode::getAttributes()  {
  return &m_attrs;
}


DOMNodeList *DOMNode::getChildNodes()  {
  m_nodelist.removeAll();
  PtrList<Node> list = ChildList();
  m_nodelist.setMinimumSize(list.getNumItems());
  foreach(list)
    DOMNode * node = (DOMNode *) list.getfor();
    m_nodelist.addItem((DOMNode *)node);
  endfor
  return &m_nodelist;
}


DOMNode *DOMNode::getFirstChild()  {

  return (DOMNode *) enumChild(0);
}


DOMNode *DOMNode::getLastChild()  {

  return (DOMNode *) enumChild(getNumChildren()-1);
}


const char *DOMNode::getLocalName()  {

  return m_nodeLocalName;
}


const char *DOMNode::getNamespaceURI()  {

  return m_namespaceURI;
}


DOMNode *DOMNode::getNextSibling()  {
  if( !m_owner ) return NULL;
  int pos = m_owner->ChildList().searchItem((Node *)this);
  return (DOMNode *) m_owner->ChildList().enumItem(pos+1);
}


const char *DOMNode::getNodeName()  {

  return m_nodeName;
}


short DOMNode::getNodeType()  {

  return m_type;
}


const char *DOMNode::getNodeValue()  {

  return m_nodeValue;
}


DOMDocument *DOMNode::getOwnerDocument()  {

  return (DOMDocument *) m_owner;
}


DOMNode *DOMNode::getParentNode()  {

  return (DOMNode *) getParent();
}

const char *DOMNode::getPrefix()  {

  return m_nodePrefix;
}


DOMNode *DOMNode::getPreviousSibling()  {
  if( !m_owner ) return NULL;
  int pos = m_owner->ChildList().searchItem((Node *)this);
  return (DOMNode *) m_owner->ChildList().enumItem(pos-1);
}


bool DOMNode::hasAttributes()  {

  return m_attrs.getNumItems() != 0;
}


bool DOMNode::hasChildNodes()  {

  return getNumChildren() != 0;
}


DOMNode *DOMNode::insertBefore(DOMNode *newChild, DOMNode *refChild)  {
  if ( !newChild ) return NULL;
  int pos = ChildList().searchItem((Node *)refChild);
  DOMNode *res = NULL;
  if ( pos >= 0 ) {
    if (newChild->getNodeType() == DOCUMENT_FRAGMENT_NODE) {
      foreach(ChildList())
        DOMNode *child = (DOMNode *)(ChildList().getfor());
        insertNodeAt(child, pos++);
      endfor
    }  else {
      insertNodeAt(newChild, pos);
    }
    res = newChild;
  }
  return res;
}

void DOMNode::insertNodeAt(DOMNode *node, int pos) {
  if (pos != -1)
    ChildList().addItem((Node *)node, pos);
  else
    addChild((Node *)node);
  // fix ownership
  node->getIssuer()->unIssue(node);
  m_issued.addItem(node);
  node->setIssuer(this);
  ((Node *)node)->setParent((Node *)this);
  ((DOMNode *)node)->m_owner = (m_type == DOCUMENT_NODE) ? (DOMDocument *)this : m_owner;
}

bool DOMNode::isSupported(const char *feature, const char *version)  {

  return false;
}


void DOMNode::normalize()  {

}


DOMNode *DOMNode::removeChild(DOMNode *oldChild)  {
  ChildList().removeItem((Node *)oldChild);
  // Do not unissue the child here! we keep the issuer around in order to
  // automatically delete the old child node when the parent node (this node)
  // goes away. if the old child node is going to be reinjected in a node 
  // somewhere else (including in a new document), the issuer will be changed
  // then. If you wish to manually delete the node for whatever reason, and
  // need to prevent the old parent node (this node) from trying to delete it
  // when it goes away, call oldParent.unIssue(oldChild) manually.
  return oldChild;
}


DOMNode *DOMNode::replaceChild(DOMNode *newChild, DOMNode *oldChild)  {
  if ( !newChild ) return NULL;
  int pos = ChildList().searchItem((Node *)oldChild);
  if ( pos > 0 ) {
    DOMNode * oldItem = (DOMNode *) ChildList().enumItem(pos);
    ChildList().setItem((Node *)newChild,pos);
    // fix ownership
    ((Node *)newChild)->setParent((Node *)this);
    ((DOMNode *)newChild)->m_owner = m_type == DOCUMENT_NODE ? (DOMDocument *)this : m_owner;
    return oldItem;
  }
  return NULL;
}


void DOMNode::setNodeValue(const char *nodeValue)  {
  m_nodeValue = nodeValue;
}


void DOMNode::setPrefix(const char *prefix)  {
  m_nodePrefix = prefix;
}

void DOMNode::unIssue(DOMNode *node) {
  m_issued.removeItem(node);
  node->setIssuer(NULL);
}

void DOMNode::setIssuer(DOMNode *node) {
  m_issuer = node;
}

DOMNode *DOMNode::getIssuer() {
  return m_issuer;
}

void DOMNode::addObject(GUID objectGuid, void *objectPointer) {
  m_objects.addItem(objectGuid, objectPointer);
}

void DOMNode::removeObject(GUID objectGuid) {
  m_objects.delItem(objectGuid);
}

void *DOMNode::findObjectByGuid(GUID objectGuid) {
  return m_objects.getItemByValue(objectGuid, NULL);
}

const char *DOMNode::getId() {
  if (!m_cachedId) {
    DOMAttr *attr = (DOMAttr *)m_attrs.getNamedItem("id");
    m_cachedId = attr ? attr->getValue() : NULL;
  }
  return m_cachedId;
}

void DOMNode::clearIdCache() {
  m_cachedId = NULL;
}

void DOMNode::setSource(const char *file, const char *incpath, int line) {
  m_file = file;
  m_incpath = incpath;
  m_line = line;
}

const char *DOMNode::getSourceFile() {
  return m_file;
}

const char *DOMNode::getIncludePath() {
  return m_incpath;
}

int DOMNode::getSourceLine() {
  return m_line;
}
