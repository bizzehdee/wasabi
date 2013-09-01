#include <precomp.h>
#include <dom/domdocfragment.h>
#include "wdf.h"
#include <dom/domxmlmanager.h>
#include <dom/domattr.h>
#include <parse/pathparse.h>

// Todo: clone definitions, so we don't have to rely on the caller holding on to the document containing them!

DefinitionsFramework *g_framework;

DefinitionsFramework::DefinitionsFramework() {
  m_registry = new DOMDocument();
  g_framework = this;
}

DefinitionsFramework::~DefinitionsFramework() {
  m_subfragments.deleteAll();
}

DOMDocument *DefinitionsFramework::getRegistry() {
  return m_registry;
}

DOMDocumentFragment *DefinitionsFramework::parseDocument(const char *file) {
  return new DOMDocumentFragment((const char *)file, DOMXMLMANAGER_ALLOW_INCLUDE);
}

DOMDocumentFragment *DefinitionsFramework::addDefinitions(DOMDocumentFragment *docFragment) {
  if (!docFragment) 
    return NULL;
  DOMNode *topLevelNode = docFragment->getChildNodes()->item(0);
  if (!topLevelNode) return NULL;
  if (STRCASEEQL(topLevelNode->getNodeName(), "WDF:Fragment")) {
    while (topLevelNode->getChildNodes()->getLength() > 0) {
      DOMNode *node = topLevelNode->getFirstChild();
      topLevelNode->removeChild(node);
      m_registry->appendChild(node);
      if (STRCASEEQL(node->getNodeName(), "WDF:Fragment")) {
        DOMNamedNodeMap *attributes = node->getAttributes();
        DOMAttr *attr = static_cast<DOMAttr*>(attributes->getNamedItem("file"));
        if (attr) {
          String file = attr->getNodeValue();
          String path = node->getIncludePath();
          if (!Std::isDirChar(path.lastChar())) path += DIRCHARSTR;
          path += file;
          m_subfragments.addItem(addDefinitions(parseDocument(path)));
        }
      } else {
        registerTopLevelNode(node);
      }
    }
  } else {
    m_registry->appendChild(topLevelNode);
    registerTopLevelNode(topLevelNode);
  }
  return docFragment;
}

DOMDocumentFragment*DefinitionsFramework::addDefinitions(const char *file) {
  return addDefinitions(parseDocument(file));
}

DOMNode *DefinitionsFramework::getTopLevelNode(const char *tagname, const char *id) {
  EntryById *entries = m_topLevelNodes.getItemByValue(String(tagname), NULL);
  if (!entries) return NULL;
  PtrList<DOMNode> *thisId = entries->getItemByValue(String(id), NULL);
  if (!thisId) return NULL;
  return thisId->getLast();
}

DOMNode *DefinitionsFramework::getPreviousTopLevelNode(DOMNode *next) {
  const char *tagname = next->getNodeName();
  DOMNamedNodeMap *attrs = next->getAttributes();
  DOMNode *idn = attrs->getNamedItem("id");
  if (!idn) return NULL;
  const char *id = idn->getNodeValue();
  EntryById *entries = m_topLevelNodes.getItemByValue(String(tagname), NULL);
  if (!entries) return NULL;
  PtrList<DOMNode> *thisId = entries->getItemByValue(String(id), NULL);
  if (!thisId) return NULL;
  int n = thisId->getNumItems();
  foreach_reverse(*thisId)
    if (thisId->getfor() == next)
      return thisId->enumItem(n-1-(foreach_index+1));
  endfor;
  return NULL;
}

DOMNode *DefinitionsFramework::getNextTopLevelNode(DOMNode *previous) {
  const char *tagname = previous->getNodeName();
  DOMNamedNodeMap *attrs = previous->getAttributes();
  DOMNode *idn = attrs->getNamedItem("id");
  if (!idn) return NULL;
  const char *id = idn->getNodeValue();
  EntryById *entries = m_topLevelNodes.getItemByValue(String(tagname), NULL);
  if (!entries) return NULL;
  PtrList<DOMNode> *thisId = entries->getItemByValue(String(id), NULL);
  if (!thisId) return NULL;
  foreach(*thisId)
    if (thisId->getfor() == previous)
      return thisId->enumItem(foreach_index+1);
  endfor;
  return NULL;
}

PtrList<DOMNode> *DefinitionsFramework::getTopLevelNodes(const char *tagname) {
  m_nodelist.removeAll();

  EntryById *entries = m_topLevelNodes.getItemByValue(String(tagname), NULL);
  if (!entries) return NULL;

  for (int i=0;i<entries->getNumPairs();i++) {
    PtrList<DOMNode> *nodes;
    String id;
    entries->enumPair(i, &id, &nodes);
    m_nodelist.addItem(nodes->getLast());
  }
  return &m_nodelist;
}

void DefinitionsFramework::registerTopLevelNode(DOMNode *node) {
  const char *tagname = node->getNodeName();
  if (STRCASEEQL(tagname, "#text")) return;
  const char *id = node->getId();
  if (!id || !*id) return;
  EntryById *entries = m_topLevelNodes.getItemByValue(String(tagname), NULL);
  if (!entries) {
    entries = new EntryById;
    m_topLevelNodes.addItem(tagname, entries);
  }
  PtrList<DOMNode> *thisId = entries->getItemByValue(String(id), NULL);
  if (!thisId) {
    thisId = new PtrList<DOMNode>;
    entries->addItem(String(id), thisId);
  }
  thisId->addItem(node);
}

