#ifndef XUIDOCUMENT_H
#define XUIDOCUMENT_H

class XUIScript;
class XUIObject;

class XUIDocument {
public:
  XUIDocument();
  virtual ~XUIDocument();
  
  void setTopLevelObject(XUIObject *topLevel);
  XUIObject *getTopLevelObject();
  
  int getNumScripts();
  XUIScript *enumScripts(int i);
  void addScript(XUIScript *script);

  virtual void onDocumentLoaded();


private:
  XUIObject *m_topLevel;
  PtrList<XUIScript> m_scripts;

};

#endif
