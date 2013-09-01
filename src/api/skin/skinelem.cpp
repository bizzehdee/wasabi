#include "precomp.h"
#include "skinelem.h"
#include "dom/wasabidom.h"
#include "parse/pathparse.h"
#include "bfc/map.h"
#include "wnd/region.h"
#include "img/imgldr.h"
#include "bfc/file/readdir.h"

DOMDocument *m_doc = NULL;
String m_skinsPath;
String m_skinPath;
String m_skinName;
int m_skinIterator = 0;
bool cached = false;
class myRegSrv;
PtrListInsertSorted<ARGB32,SortByPtrVal> bitmapcache;
Map<String,myRegSrv *> regmap;

class myRegSrv: public RegionServerI {
public: 
  myRegSrv(Region * reg): m_reg(reg) {}
  Region *getRegion() {return m_reg;}
private:
  Region *m_reg;
};

void SkinElementsMgr::setSkinsPath(const char *path)  {
  m_skinsPath = path;
}


int SkinElementsMgr::loadSkin(const char *name)  {
  if ( m_doc )  {

  }
  DOMParser p;
  m_skinName = name;
  String skinPath = m_skinsPath;
  skinPath += m_skinName;
  skinPath += DIRCHARSTR;
  m_skinPath = skinPath;
  skinPath += "skin.xml";
  m_doc = p.parseFile(skinPath,1);
  if(m_doc && m_doc->getDocumentElement()) m_skinIterator++;
  return m_skinIterator;
}

int SkinElementsMgr::cacheSkin(bool force) {
  if ( cached && !force ) return 0;
  if ( m_doc ) {
    //DOMNodeList * list = m_doc->getElementsByTagName("bitmap");
    //foreach(list)
    //  DOMElement * node = (DOMElement *)list->getfor();
    
    // SMARTER : load the png files directly
    ReadDir skinDir(m_skinPath);
    while (skinDir.next())  { 
      if ( skinDir.isDir() ) {
        String dir = m_skinPath + skinDir.getFilename();
        dir += DIRCHARSTR;
        ReadDir skinSubDir(dir,"*.png");
        while ( skinSubDir.next() ) {
          int dummy0;
          int dummy1;
          int dummy2;
          int dummy3;
          int dummy4;
          int dummy5;
          int dummy6;
          ARGB32 * bits = imageLoader::requestBitmap( dir + skinSubDir.getFilename() , &dummy0, &dummy1, &dummy2, &dummy3, &dummy4, &dummy5, &dummy6);
          if (bits) 
            bitmapcache.addItem(bits);
          Std::usleep(10);
        }
      }
    }
    //endfor

    cached = true;
    return 1;
  }
  return 0;
}

int *SkinElementsMgr::getSkinIterator()  {
  return &m_skinIterator;
}

int SkinElementsMgr::getLayerFromId(const char *id)  {

  return 0;
}

COLORREF SkinElementsMgr::skin_getColorElement(char *type, const char **color_grp)  {
  if(m_doc) {   
    DOMElement * node = m_doc->getElementById(type);
    if (node ) {
      const char *color = node->getAttribute("value");
      if ( color ) {
       PathParser p(color,",");
       int r = atoi(p.enumStringSafe(0,"150"));
       int g = atoi(p.enumStringSafe(1,"220"));
       int b = atoi(p.enumStringSafe(2,"150"));
       return RGB(r,g,b);
      }
    }
  }
  return RGB(255,0,0);
}


const char *SkinElementsMgr::getElementAlias(const char *id)  {
  if(m_doc) {   
    DOMElement * node = m_doc->getElementById(id);
    if (node && !STRCMP(node->getNodeName(),"elementalias")) {
      DOMAttr * attr = node->getAttributeNode("target");
      if( attr && attr->getSpecified() )
        return attr->getValue();
    }
  }
  return NULL;
}


const char *SkinElementsMgr::getBitmapFilename(const char *id, int *x, int *y, int *subw, int *subh,int *d, const char **rootpath, XmlReaderParams **params) {
  if(m_doc) {   
    DOMElement * node = m_doc->getElementById(id);
    if (node ) {
      const char *rx = node->getAttribute("x");
      const char *ry = node->getAttribute("y");
      const char *rw = node->getAttribute("w");
      const char *rh = node->getAttribute("h");
      if ( rx && ry && rw && rh ) {
        if(x) *x = atoi(rx);
        if(y) *y = atoi(ry);
        if(subw) *subw = atoi(rw);
        if(subh) *subh = atoi(rh);
      }
      if( rootpath ) *rootpath = m_skinPath;
      //TODO: pass the XmlParams around too
      return node->getAttribute("file");
    }
  }
  return NULL;
}

const char *SkinElementsMgr::getSkinPath() {
  return m_skinPath;
}

RegionServer *SkinElementsMgr::requestSkinRegion(const char *id)  {
  myRegSrv *srv = NULL;
  regmap.getItem(id,&srv);
  return srv;
}

void SkinElementsMgr::cacheSkinRegion(const char *id, Region *r)  {
  if ( regmap.getItem(id) || !r) return;
  myRegSrv *srv = new myRegSrv(new RegionI(r->getOSHandle()));
  regmap.addItem(id,srv);
}
