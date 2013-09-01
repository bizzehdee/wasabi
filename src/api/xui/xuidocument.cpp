#include <precomp.h>
#include "xuiscript.h"
#include "xuidocument.h"

XUIDocument::XUIDocument() : m_topLevel(NULL) {
}

XUIDocument::~XUIDocument() { 
  m_scripts.deleteAllSafe(); 
}

void XUIDocument::setTopLevelObject(XUIObject *topLevel) { 
  m_topLevel = topLevel; 
}

XUIObject *XUIDocument::getTopLevelObject() { 
  return m_topLevel; 
}

int XUIDocument::getNumScripts() { 
  return m_scripts.getNumItems(); 
}

XUIScript *XUIDocument::enumScripts(int i) { 
  return m_scripts.enumItem(i); 
}

void XUIDocument::addScript(XUIScript *script) { 
  m_scripts.addItem(script); 
}

void XUIDocument::onDocumentLoaded() {
  foreach(m_scripts)
    m_scripts.getfor()->onReady(m_topLevel);
  endfor
}
