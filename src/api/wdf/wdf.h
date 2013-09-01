#ifndef WDF_H
#define WDF_H

#include <dom/domdoc.h>

class DefinitionsFramework {
  public:
    DefinitionsFramework();
    virtual ~DefinitionsFramework();

    // parse a document into a fragment
    DOMDocumentFragment *parseDocument(const char *file);
    // add a wax fragment to the registry
    DOMDocumentFragment *addDefinitions(DOMDocumentFragment *docFragment);
    DOMDocumentFragment *addDefinitions(const char *file);
    // returns the most recent definition of <tagname id="id"/> or NULL if no match exists
    DOMNode *getTopLevelNode(const char *tagname, const char *id);
    DOMNode *getPreviousTopLevelNode(DOMNode *next);
    DOMNode *getNextTopLevelNode(DOMNode *previous);
    // returns all the nodes that are the most recent definitions of any defined id, for a particular tag
    // note that the return value is valid for the lifetime of the return scope, do not hold on to it, 
    // make a copy of the list if you need.
    PtrList<DOMNode> *getTopLevelNodes(const char *tagname);

    // get full registry document
    DOMDocument *getRegistry();

  private:
    void registerTopLevelNode(DOMNode *node);
    DOMDocument *m_registry;

    // EntryById associates a string (id) to a list of DOMNodes, the last node takes precedence in findTopLevelNode.
    class EntryById : public Map<String, PtrList<DOMNode> *, StringComparator2> {};
    // The toplevelnodes map associates a string (tagname) to an EntryById, so we can search on both (id and tagname) efficiently.
    Map<String, EntryById *, StringComparator2> m_topLevelNodes;

    // temporary storage
    PtrList<DOMNode> m_nodelist;
    PtrList<DOMDocumentFragment> m_subfragments;
};


#endif
