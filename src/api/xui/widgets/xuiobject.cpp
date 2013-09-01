#include <precomp.h>
#include <wnd/basewnd.h>
#include <bfc/string/stringdict.h>
#include "xuiobject.h"
#include <draw/bitmap.h>
#include <wnd/canvas.h>
#include "../xuiscript.h"
#include <dom/domdocfragment.h>
#include <dom/domnode.h>
#include <dom/domattr.h>
#include <wnd/region.h>
#include <api/xui/xuiinstantiator.h>
#include "../xuidndmanager.h"

_DECLARE_XUISERVICE(XUISpacerSvc);
_DECLARE_XUISERVICE(XUIBoxSvc);

// todo: register that to a local repository, no need to have it go 
// in the list of services, besides, we want to force it to link
_DECLARE_XUISERVICE(XUIGenericContainerSvc);

enum {
  XUIOBJECT_ATTRIBUTE_X,
  XUIOBJECT_ATTRIBUTE_Y,
  XUIOBJECT_ATTRIBUTE_W,
  XUIOBJECT_ATTRIBUTE_H,
  XUIOBJECT_ATTRIBUTE_MINW,
  XUIOBJECT_ATTRIBUTE_MINH,
  XUIOBJECT_ATTRIBUTE_MAXW,
  XUIOBJECT_ATTRIBUTE_MAXH,
  XUIOBJECT_ATTRIBUTE_HIDDEN,
  XUIOBJECT_ATTRIBUTE_ID,
  XUIOBJECT_ATTRIBUTE_BGCOLOR,
  XUIOBJECT_ATTRIBUTE_BGSTYLE,
  XUIOBJECT_ATTRIBUTE_HIGHLIGHTCOLOR,
  XUIOBJECT_ATTRIBUTE_SHADOWCOLOR,
  XUIOBJECT_ATTRIBUTE_OUTLINECOLOR,
  XUIOBJECT_ATTRIBUTE_OUTLINESIZE,
  XUIOBJECT_ATTRIBUTE_IMAGE,
  XUIOBJECT_ATTRIBUTE_IMAGEALPHA,
  XUIOBJECT_ATTRIBUTE_DISABLEDIMAGEALPHA,
  XUIOBJECT_ATTRIBUTE_WANTFOCUS,
  XUIOBJECT_ATTRIBUTE_FOCUSMARGIN,
  XUIOBJECT_ATTRIBUTE_MOUSETHROUGH,
  XUIOBJECT_ATTRIBUTE_DISABLED,
  XUIOBJECT_ATTRIBUTE_MARGIN,
  XUIOBJECT_ATTRIBUTE_FLEX,
  XUIOBJECT_ATTRIBUTE_DEBUG,
};

BEGIN_STRINGDICTIONARY(_XUIOBJECT_ATTRIBUTES);
SDI("id",                 XUIOBJECT_ATTRIBUTE_ID);
SDI("x",                  XUIOBJECT_ATTRIBUTE_X);
SDI("y",                  XUIOBJECT_ATTRIBUTE_Y);
SDI("w",                  XUIOBJECT_ATTRIBUTE_W);
SDI("h",                  XUIOBJECT_ATTRIBUTE_H);
SDI("minw",               XUIOBJECT_ATTRIBUTE_MINW);
SDI("minh",               XUIOBJECT_ATTRIBUTE_MINH);
SDI("maxw",               XUIOBJECT_ATTRIBUTE_MAXW);
SDI("maxh",               XUIOBJECT_ATTRIBUTE_MAXH);
SDI("hidden",             XUIOBJECT_ATTRIBUTE_HIDDEN);
SDI("bgcolor",            XUIOBJECT_ATTRIBUTE_BGCOLOR);
SDI("bgstyle",            XUIOBJECT_ATTRIBUTE_BGSTYLE);
SDI("highlightcolor",     XUIOBJECT_ATTRIBUTE_HIGHLIGHTCOLOR);
SDI("shadowcolor",        XUIOBJECT_ATTRIBUTE_SHADOWCOLOR);
SDI("outlinecolor",       XUIOBJECT_ATTRIBUTE_OUTLINECOLOR);
SDI("outlinesize",        XUIOBJECT_ATTRIBUTE_OUTLINESIZE);
SDI("image",              XUIOBJECT_ATTRIBUTE_IMAGE);
SDI("imagealpha",         XUIOBJECT_ATTRIBUTE_IMAGEALPHA);
SDI("disabledimagealpha", XUIOBJECT_ATTRIBUTE_DISABLEDIMAGEALPHA);
SDI("wantfocus",          XUIOBJECT_ATTRIBUTE_WANTFOCUS);
SDI("focusmargin",        XUIOBJECT_ATTRIBUTE_FOCUSMARGIN);
SDI("mousethrough",       XUIOBJECT_ATTRIBUTE_MOUSETHROUGH);
SDI("disabled",           XUIOBJECT_ATTRIBUTE_DISABLED);
SDI("margin",             XUIOBJECT_ATTRIBUTE_MARGIN);
SDI("flex",               XUIOBJECT_ATTRIBUTE_FLEX);
SDI("debug",              XUIOBJECT_ATTRIBUTE_DEBUG);
END_STRINGDICTIONARY(_XUIOBJECT_ATTRIBUTES, xuiObjectAttributes);

XUIObject::XUIObject() {
  m_ownerService = NULL;
  m_x = m_y = m_w = m_h = m_minw = m_minh = m_maxw = m_maxh = AUTOWH;
  m_parent = NULL;
  m_draggingWindow = 0;
  m_mouseInArea = 0;
  m_captureTypes = 0;
  m_framework = NULL;
  m_image = NULL;
  m_bgcolor = 0;
  m_bgstyle = XUI_BGSTYLE_FLAT;
  #ifdef WIN32
  m_highlightcolor = RGBTOBGR(GetSysColor(COLOR_3DHIGHLIGHT)) | 0xFF000000;
  m_shadowcolor = RGBTOBGR(GetSysColor(COLOR_3DSHADOW)) | 0xFF000000;
  #else
  m_highlightcolor = 0xFF00FFFF;
  m_shadowcolor = 0xFF808080;
  #endif
  m_outlinecolor = 0;
  MEMSET(m_outlinesize, 0, sizeof(m_outlinesize));
  m_blender = NULL;
  m_imagealpha = 255;
  m_disabledimagealpha = -1;
  m_wantFocus = 0;
  m_mousethrough = -1;
  m_focusrectmargin[0] = m_focusrectmargin[1] = m_focusrectmargin[2] = m_focusrectmargin[3] = 0;
  m_flex_orientation = FLEXBOX_ORIENTATION_HORIZONTAL;
  m_flex = 0;
  m_isInstantiationElement = 0;
  m_debugColor = 0;
  m_dndDragging = false;
  m_dndlbuttondown = false;
  m_trieddnd = false;
}

XUIObject::~XUIObject() {
  ASSERT(m_children.getNumItems() == 0);
  ASSERT(m_scripts.getNumItems() == 0);
  m_attrforwarders.deleteAll();
  delete m_image;
  m_groupdefsFragments.deleteAll();
  // m_groupdefs contains only references to m_groupdefsFragments contents, no need to free anything in it as its lists are automatically freed
}

void XUIObject::setAttribute(const char *attributeName, const char *attributeValue) {
  XUIAttribute *attrib = m_attributes.getAttribute(attributeName);
  if (attrib) {
    attrib->setValue(attributeValue);
  } else {
    m_attributes.addAttribute(attributeName, attributeValue, attrib);
  }
  onAttributeChanged(attributeName, attributeValue);
  eventSource_onAttributeChanged(attributeName, attributeValue);
}

const char *XUIObject::getAttribute(const char *attributeName) {
  XUIAttribute *attrib = m_attributes.getAttribute(attributeName);
  if (attrib) {
    return attrib->getValue();
  }
  return NULL;
}

int XUIObject::getAttributeInt(const char *attributeName, int defaultValue) {
  XUIAttribute *attrib = m_attributes.getAttribute(attributeName);
  if (attrib) {
    const char *v = attrib->getValue();
    if (!v) return defaultValue;
    return atoi(v);
  }
  return defaultValue;
}

bool XUIObject::getAttributeBool(const char *attributeName, bool defaultValue) {
  XUIAttribute *attrib = m_attributes.getAttribute(attributeName);
  if (attrib) {
    const char *v = attrib->getValue();
    if (!v) return (bool)defaultValue;
    return (bool)parseBoolAttribute(v);
  }
  return defaultValue;
}

bool XUIObject::hasAttribute(const char *attributeName) {
  return (bool)m_attributes.hasAttribute(attributeName);
}

int XUIObject::getNumAttributes() {
  return m_attributes.getNumItems();
}

const char *XUIObject::enumAttributeName(int i) {
  XUIAttribute *attr = m_attributes.enumItem(i);
  if (attr) return attr->getName();
  return NULL;
}

const char *XUIObject::enumAttributeValue(int i) {
  XUIAttribute *attr = m_attributes.enumItem(i);
  if (attr) return attr->getValue();
  return NULL;
}

void XUIObject::onAttributeChanged(const char *attributeName, const char *attributeValue) {
  ASSERT(m_wnd);
  COLORREF color;
  int intval;
      
  switch (xuiObjectAttributes.getId(attributeName)) {
    case XUIOBJECT_ATTRIBUTE_ID:
      m_id = attributeValue;
      break;
    case XUIOBJECT_ATTRIBUTE_X:
      //parseCoordinateAttribute(getAttribute("x"), &m_x_attr, &m_fx_attr, COORDINATE_FROM_X);
      m_x = atoi(attributeValue);
      if (m_wnd->isInited())
        updateCoordinates();
      break;
    case XUIOBJECT_ATTRIBUTE_Y:
      //parseCoordinateAttribute(getAttribute("y"), &m_y_attr, &m_fy_attr, COORDINATE_FROM_Y);
      m_y = atoi(attributeValue);
      if (m_wnd->isInited())
        updateCoordinates();
      break;
    case XUIOBJECT_ATTRIBUTE_W:
      //parseCoordinateAttribute(getAttribute("w"), &m_w_attr, &m_fw_attr, COORDINATE_FROM_0);
      m_w = atoi(attributeValue);
      if (m_wnd->isInited())
        updateCoordinates();
      break;
    case XUIOBJECT_ATTRIBUTE_H:
      //parseCoordinateAttribute(getAttribute("h"), &m_h_attr, &m_fh_attr, COORDINATE_FROM_0);
      m_h = atoi(attributeValue);
      if (m_wnd->isInited())
        updateCoordinates();
      break;
    case XUIOBJECT_ATTRIBUTE_MINW:
      //parseCoordinateAttribute(getAttribute("minw"), &m_minw_attr, &m_fminw_attr, COORDINATE_FROM_0);
      m_minw = atoi(attributeValue);
      if (m_wnd->isInited())
        updateCoordinates();
      break;
    case XUIOBJECT_ATTRIBUTE_MINH:
      //parseCoordinateAttribute(getAttribute("minh"), &m_minh_attr, &m_fminh_attr, COORDINATE_FROM_0);
      m_minh = atoi(attributeValue);
      if (m_wnd->isInited())
        updateCoordinates();
      break;
    case XUIOBJECT_ATTRIBUTE_MAXW:
      //parseCoordinateAttribute(getAttribute("maxw"), &m_maxw_attr, &m_fmaxw_attr, COORDINATE_FROM_0);
      m_maxw = atoi(attributeValue);
      if (m_wnd->isInited())
        updateCoordinates();
      break;
    case XUIOBJECT_ATTRIBUTE_MAXH:
      //parseCoordinateAttribute(getAttribute("maxh"), &m_maxh_attr, &m_fmaxh_attr, COORDINATE_FROM_0);
      m_maxh = atoi(attributeValue);
      if (m_wnd->isInited())
        updateCoordinates();
      break;
    case XUIOBJECT_ATTRIBUTE_HIDDEN:
      intval = parseBoolAttribute(attributeValue);
      if (!m_wnd->isInited()) m_wnd->setStartHidden(intval);
      else m_wnd->setVisible(!intval);
      break;
    case XUIOBJECT_ATTRIBUTE_BGCOLOR:
      parseColorAttribute(attributeValue, &m_bgcolor);
      invalidate();
      break;
    case XUIOBJECT_ATTRIBUTE_SHADOWCOLOR:
      parseColorAttribute(attributeValue, &m_shadowcolor);
      invalidate();
      break;
    case XUIOBJECT_ATTRIBUTE_HIGHLIGHTCOLOR:
      parseColorAttribute(attributeValue, &m_highlightcolor);
      invalidate();
      break;
    case XUIOBJECT_ATTRIBUTE_BGSTYLE:
      m_bgstyle = parseBGStyleAttribute(attributeValue);
      invalidate();
      break;
    case XUIOBJECT_ATTRIBUTE_OUTLINECOLOR: 
      parseColorAttribute(attributeValue, &m_outlinecolor);
      invalidate();
      break;
    case XUIOBJECT_ATTRIBUTE_OUTLINESIZE: {
      parseCSVIntAttribute(attributeValue, m_outlinesize, 4, -999999);
      if (m_outlinesize[0] == -999999) m_outlinesize[0] = 0;
      for (int i=1;i<4;i++) {
        if (m_outlinesize[i] == -999999) m_outlinesize[i] = m_outlinesize[i-1];
      }
      invalidate();
      break;
    }
    case XUIOBJECT_ATTRIBUTE_DEBUG:
      parseColorAttribute(attributeValue, &m_debugColor);
      invalidate();
      break;
    case XUIOBJECT_ATTRIBUTE_IMAGE:
      setBackgroundImage(attributeValue);
      invalidate();
      break;
    case XUIOBJECT_ATTRIBUTE_IMAGEALPHA:
      m_imagealpha = atoi(attributeValue);
      invalidate();
      break;
    case XUIOBJECT_ATTRIBUTE_DISABLEDIMAGEALPHA:
      m_disabledimagealpha = atoi(attributeValue);
      invalidate();
      break;
    case XUIOBJECT_ATTRIBUTE_WANTFOCUS:
      m_wantFocus = parseBoolAttribute(attributeValue);
      break;
    case XUIOBJECT_ATTRIBUTE_MOUSETHROUGH:
      m_mousethrough = parseBoolAttribute(attributeValue, 1);
      break;
    case XUIOBJECT_ATTRIBUTE_DISABLED:
      m_wnd->setEnabled(!parseBoolAttribute(attributeValue));
      break;
    case XUIOBJECT_ATTRIBUTE_FOCUSMARGIN:
      parseCSVIntAttribute(attributeValue, m_focusrectmargin, 4, 0);
      invalidate();
      break;
    case XUIOBJECT_ATTRIBUTE_MARGIN: {
      int v[4];
      parseCSVIntAttribute(attributeValue, v, 4, 0);
      m_wnd->setMargins(v[0], v[1], v[2], v[3]);
      invalidate();
      break;
    }
    case XUIOBJECT_ATTRIBUTE_FLEX: {
      if (STRCASEEQL(attributeValue, "*")) m_flex = -1;
      else m_flex = atoi(attributeValue);
      updateCoordinates();
      break;
    }
  }
}

void XUIObject::initChild(XUIObject *xo) {
  RootWnd *xownd = xo->getRootWnd();
  xownd->setParent(m_wnd);
  xownd->init(m_wnd);
}

void XUIObject::onInit() {
  eventSource_onInit();
  ASSERT(m_wnd);

  foreach(m_children)
    initChild(m_children.getfor());
  endfor;

  if (!getParent()) {
    int w = m_w;
    int h = m_h;
    if (w == AUTOWH) w = getDefaultSize(XUI_DEFAULTSIZE_WIDTH);
    if (h == AUTOWH) h = getDefaultSize(XUI_DEFAULTSIZE_HEIGHT);
    int x = m_x;
    int y = m_y;
    if (x == AUTOWH) x = 0;
    if (y == AUTOWH) y = 0;
    m_wnd->resize(x, y, w, h);
  }

  reflow();
}

void XUIObject::onResize() {
  if (!m_wnd->isPostOnInit()) return;
  eventSource_onResize();
  reflow();
}

void XUIObject::updateCoordinates() {
  if (m_parent) m_parent->reflow();
  else reflow();
}

void XUIObject::addChild(XUIObject *child, XUIObject *insertBefore) {
  int pos = -1;
  if (insertBefore) {
    pos = m_children.searchItem(insertBefore);
  }
  if (!child) return;
  XUIObject *prevparent = child->getParent();
  if (prevparent) {
    prevparent->removeChild(child);
  }
  m_children.addItem(child, pos);
  child->setParent(this);
  if (getRootWnd()->isPostOnInit()) {
    child->getRootWnd()->init(getRootWnd());
    reflow();
  }
}

void XUIObject::removeChild(XUIObject *child) {
  m_children.removeItem(child);
  child->setParent(NULL);
}

void XUIObject::destroyChild(XUIObject *child) {
  int pos = m_children.searchItem(child);
  if (pos < 0) return;
  m_children.removeByPos(pos);
  svc_xuiObject *svc = child->getOwnerService();
  if (svc == NULL) {
    DebugString("Owner service is NULL for XUIObject %s", getTagName());
  } else {
    svc->destroy(child);
  }
  reflow();
}

void XUIObject::destroyChildren(PtrList<XUIObject> *deletelist) {
  foreach(*deletelist)
    XUIObject *child = deletelist->getfor();
    int pos = m_children.searchItem(child);
    if (pos < 0) return;
    m_children.removeByPos(pos);
    svc_xuiObject *svc = child->getOwnerService();
    if (svc == NULL) {
      DebugString("Owner service is NULL for XUIObject %s", getTagName());
    } else {
      svc->destroy(child);
    }
  endfor;
  reflow();
}

XUIObject *XUIObject::createObject(const char *tagname) {
  if (!m_framework) return NULL;
  XUIInstantiator xui_instantiator(m_framework);
  
  DOMNode node(tagname);
  
  return xui_instantiator.instantiateXUINode(&node);
}

int XUIObject::getNumChildren() {
  return m_children.getNumItems();
}

XUIObject *XUIObject::enumChild(int i) {
  return m_children.enumItem(i);
}

void XUIObject::setParent(XUIObject *parent) {
  if (parent)
    ASSERT(m_parent == NULL);
  else 
    ASSERT(m_parent != NULL);
  m_parent = parent;
}

XUIObject *XUIObject::getParent() {
  return m_parent;
}

const char *XUIObject::getTagName() {
  return m_tagname;
}

void XUIObject::setTagName(const char *tagname) {
  m_tagname = tagname;
}

void XUIObject::onLeftButtonDown(int x, int y) {
  m_trieddnd = false;
  eventSource_onLeftButtonDown(x, y);
  int _x = x;
  int _y = y;
  m_wnd->clientToScreen(&_x, &_y);
  m_draganchor_x = _x;
  m_draganchor_y = _y;
  if (getAttributeBool("dragwindow")) {
    m_draggingWindow = 1;
  }
  if (getAttributeBool("dndsource")) {
    beginCapture(CAPTURETYPE_DND);
    m_dndlbuttondown = true;
  }
  XUIObject *o = this;
  while (1) {
    o = o->getParent();
    if (!o) break;
    if (o->wantFocus()) 
      o->focus();
  }
}

bool XUIObject::pointInRect(int x, int y) {
  return m_wnd->isMouseOver(x, y);
}

bool XUIObject::pointInRegion(int x, int y) {
  int over = pointInRect(x, y);
  if (over) {
    RootWnd *child = m_wnd->findRootWndChild(x, y);
    over &= (child == NULL || child == m_wnd);
  }
  return over;
}

void XUIObject::onMouseMove(int x, int y) {
  if (m_dndDragging) return;
  int _x = x;
  int _y = y;
  m_wnd->clientToScreen(&_x, &_y);
  if (pointInRegion(x, y)) {
    if (!m_mouseInArea) {
      m_mouseInArea = 1;
      beginCapture(CAPTURETYPE_AREA);
      onMouseEnter();
    }
  } else {
    if (m_mouseInArea) {
      m_mouseInArea = 0;
      endCapture(CAPTURETYPE_AREA);
      onMouseLeave();
      // prevent further mousemove event processing, because we would not have gotten here 
      // had we not captured the mouse, and we only capture the mouse to implement the 
      // mouseenter/mouseleave events, not to cause any more mousemove events than normal.
      return;
    }
  }
  eventSource_onMouseMove(x, y);
  if (m_draggingWindow) {
    int ox = _x - m_draganchor_x;
    int oy = _y - m_draganchor_y;
    RECT r;
    m_wnd->getRootParent()->getWindowRect(&r);
    m_wnd->getRootParent()->move(r.left + ox, r.top + oy);
    m_draganchor_x = _x;
    m_draganchor_y = _y;
  }
  if (m_dndlbuttondown && !m_trieddnd) {
    if (ABS(_x - m_draganchor_x) >= 4 ||
        ABS(_y - m_draganchor_y) >= 4) {
      m_trieddnd = true;
      onBeginDrag(NULL);
      m_wnd->handleDrag();
    }
  }
}

void XUIObject::onLeftButtonUp(int x, int y) {
  m_trieddnd = false;
  eventSource_onLeftButtonUp(x, y);
  if (m_draggingWindow) {
    m_draggingWindow = 0;
  }
  if (m_dndlbuttondown) {
    endCapture(CAPTURETYPE_DND);
    m_dndlbuttondown = false;
  }
}

void XUIObject::onRightButtonDown(int x, int y) {
  eventSource_onRightButtonDown(x, y);
}

void XUIObject::onRightButtonUp(int x, int y) {
  eventSource_onRightButtonUp(x, y);
}

void XUIObject::onMiddleButtonDown(int x, int y) {
  eventSource_onMiddleButtonDown(x, y);
}

void XUIObject::onMiddleButtonUp(int x, int y) {
  eventSource_onMiddleButtonUp(x, y);
}
void XUIObject::onMouseWheelDown(int click, int lines) {
  eventSource_onMouseWheelDown(click, lines);
}

void XUIObject::onMouseWheelUp(int click, int lines) {
  eventSource_onMouseWheelUp(click, lines);
}

void XUIObject::beginCapture(int captureType) {
  if (!m_captureTypes)
    m_wnd->beginCapture();
  m_captureTypes |= (1 << captureType);
}

void XUIObject::endCapture(int captureType) {
  m_captureTypes &= ~(1 << captureType);
  if (!m_captureTypes)
    m_wnd->endCapture();
}

void XUIObject::beginMouseCapture() {
  beginCapture(CAPTURETYPE_USER);
}

void XUIObject::endMouseCapture() {
  endCapture(CAPTURETYPE_USER);
}

void XUIObject::setBackgroundImage(const char *image) {
  if (m_framework)
    image = getImageFile(m_framework, image);
  m_image = new Bitmap(image);
}

void XUIObject::onPaint(Canvas *canvas) {
  RECT cr;
  m_wnd->getClientRect(&cr);
  COLORREF color = m_bgcolor;
  if (color != 0) {
    canvas->fillRect(&cr, (color & 0xFFFFFF), (color & 0xFF000000) >> 24);
  }
  if (m_bgstyle != XUI_BGSTYLE_NONE) {
    COLORREF c1, c2;
    if (m_bgstyle == XUI_BGSTYLE_RAISED) {
      c1 = m_highlightcolor;
      c2 = m_shadowcolor;
    } else if (m_bgstyle == XUI_BGSTYLE_SUNKEN) {
      c1 = m_shadowcolor;
      c2 = m_highlightcolor;
    }
    if (m_bgstyle != XUI_BGSTYLE_FLAT) {
      { RECT tr={cr.left,cr.top,cr.right,cr.top+1};
      canvas->fillRect(&tr, (c1 & 0xFFFFFF), (c1 & 0xFF000000) >> 24); }
      { RECT tr={cr.left, cr.top, cr.left+1,cr.bottom}; 
      canvas->fillRect(&tr, (c1 & 0xFFFFFF), (c1 & 0xFF000000) >> 24); }
      { RECT tr={cr.right-1,cr.top,cr.right,cr.bottom};
      canvas->fillRect(&tr, (c2 & 0xFFFFFF), (c2 & 0xFF000000) >> 24); }
      { RECT tr={cr.left, cr.bottom-1,cr.right,cr.bottom};
      canvas->fillRect(&tr, (c2 & 0xFFFFFF), (c2 & 0xFF000000) >> 24); }
    }
  }
  if (m_image) {
    int alpha = m_imagealpha;
    if (m_disabledimagealpha != -1 && !m_wnd->isEnabled()) alpha = m_disabledimagealpha;
    m_image->stretchToRectAlpha(canvas, &cr, alpha, m_blender);
  }
  color = m_outlinecolor;
  if (color != 0 && m_outlinesize != 0) {
    { RECT tr={cr.left,cr.top,cr.right,cr.top+m_outlinesize[1]};
    canvas->fillRect(&tr, (color & 0xFFFFFF), (color & 0xFF000000) >> 24); }
    { RECT tr={cr.right-m_outlinesize[2],cr.top,cr.right,cr.bottom};
    canvas->fillRect(&tr, (color & 0xFFFFFF), (color & 0xFF000000) >> 24); }
    { RECT tr={cr.left, cr.bottom-m_outlinesize[3],cr.right,cr.bottom};
    canvas->fillRect(&tr, (color & 0xFFFFFF), (color & 0xFF000000) >> 24); }
    { RECT tr={cr.left, cr.top, cr.left+m_outlinesize[0],cr.bottom}; 
    canvas->fillRect(&tr, (color & 0xFFFFFF), (color & 0xFF000000) >> 24); }
  }
  if (m_wnd->gotFocus()) {
    cr.left += m_focusrectmargin[0];
    cr.top += m_focusrectmargin[1];
    cr.right -= m_focusrectmargin[2];
    cr.bottom -= m_focusrectmargin[3];
    DrawFocusRect(canvas->getHDC(), &cr);
  }
  if (m_debugColor != 0) {
    RegionI clip;
    canvas->getClipRgn(&clip);
    canvas->selectClipRgn(NULL);
    RECT ncr;
    m_wnd->getNonClientRect(&ncr);
    canvas->drawRect(&ncr, FALSE, m_debugColor, 255);
    canvas->selectClipRgn(&clip);
  }
}

XUIObject *XUIObject::getElementById(const char *id, bool local) {
  FindElementById fbi(id);
  return findElement(&fbi, local);
}

XUIObject *XUIObject::getElementByTagName(const char *tag, bool local) {
  FindElementByTagName fbt(tag);
  return findElement(&fbt, local);
}

XUIObject *XUIObject::getElementByAttribute(const char *attrName, const char *attrValue, bool local) {
  FindElementByAttribute fba(attrName, attrValue);
  return findElement(&fba, local);
}

XUIObject *XUIObject::findElement(FindElementCallback *cb, bool local) {
  foreach(m_children)
    XUIObject *child = m_children.getfor();
    if (cb->testElement(child)) return child;
    if (!local && !child->isInstantiationElement()) {
      XUIObject *o = child->findElement(cb, false);
      if (o) return o;
    }
  endfor;
  return NULL;
}

void XUIObject::findElements(FindElementCallback *cb, PtrList<XUIObject> *result, bool local) {
  foreach(m_children)
    XUIObject *child = m_children.getfor();
    if (cb->testElement(child)) 
      result->addItem(child);
    if (!local && !child->isInstantiationElement()) {
      child->findElements(cb, result, false);
    }
  endfor;
}

#define BELOWOREQ_WITHAUTOWH(a, b) (((a < b) && b != AUTOWH) || (a == AUTOWH))
#define ABOVEOREQ_WITHAUTOWH(a, b) (((a > b) && a != AUTOWH) || (b == AUTOWH))

#define MAX_WITHAUTOWH(a, b) (ABOVEOREQ_WITHAUTOWH(a, b) ? a : b)

int XUIObject::getDefaultSize(int which) {
  int default_size = AUTOWH;

  // if we have a size explicitly defined, use that, of course.
  if (which == XUI_DEFAULTSIZE_WIDTH) { if (m_w != AUTOWH) default_size = m_w; }
  else { if (m_h != AUTOWH) default_size = m_h; }

  // otherwise, calculate the ideal size our children would like
  if (default_size == AUTOWH) {
    int default_from_children = AUTOWH;

    RECT cr;
    m_wnd->getClientRect(&cr);
    int margins[4];
    m_wnd->getMargins(&margins[0], &margins[1], &margins[2], &margins[3]);

    int isflex = (m_flex_orientation == which);
    int totalflex = AUTOWH;

    foreach(m_children)
      XUIObject *xuichild = m_children.getfor();
      int childoffset = (which == XUI_DEFAULTSIZE_WIDTH) ? xuichild->getAttributeX() : xuichild->getAttributeY();
      // if this is a flex orientation, we need to add the default size of each flex child
      // but do not add the size of fixed pos objects, handle that just like if this wasn't a flex orientation
      if (isflex && childoffset == AUTOWH) {
        // todo: add support for flex minw/h here, and in reflow()
        int size = xuichild->getDefaultSize(which);
        if (size != AUTOWH) {
          if (totalflex == AUTOWH) totalflex = 0;
          totalflex += size;
        }
      } else {
        // not a flex orientation, just use the dimension of the biggest child, adjusting for its offset in parent
        int size = xuichild->getDefaultSize(which);
        if (childoffset != AUTOWH) size += childoffset;
        default_from_children = MAX_WITHAUTOWH(default_from_children, size);
      }
    endfor;

    // use the largest value between flex and non-flex objects
    default_from_children = MAX_WITHAUTOWH(default_from_children, totalflex);

    // since this function returns the default size of this object, and since our children
    // are located inside our margin, add the size of the margin to what the children need.
    if (default_from_children != AUTOWH) {
      if (which == XUI_DEFAULTSIZE_WIDTH) default_from_children += margins[0] + margins[2];
      else default_from_children += margins[1] + margins[3];
    }

    // if we found a default size needed by our children, use it
    if (default_from_children != AUTOWH) default_size = default_from_children;
  }

  // if we still don't have a value, use what the object suggests
  int suggested_which = (which == XUI_DEFAULTSIZE_WIDTH) ? SUGGESTED_W : SUGGESTED_H;
  if (default_size == AUTOWH)
    default_size = m_wnd->getPreferences(suggested_which);

  // constrain the default size to the explicitly specified min/max, override the
  // explicitly specified w/h if needed (minwh/maxwh take precedence over wh)
  if (which == XUI_DEFAULTSIZE_WIDTH) {
    if (default_size != AUTOWH && m_minw != AUTOWH) default_size = MAX(default_size, m_minw);
    if (default_size != AUTOWH && m_maxw != AUTOWH) default_size = MIN(default_size, m_maxw);
  } else {
    if (default_size != AUTOWH && m_minh != AUTOWH) default_size = MAX(default_size, m_minh);
    if (default_size != AUTOWH && m_maxh != AUTOWH) default_size = MIN(default_size, m_maxh);
  }

  // if we didn't get anything at all, but we do have a min value, use that
  if (default_size == AUTOWH) {
    if (which == XUI_DEFAULTSIZE_WIDTH) { if (m_minw != AUTOWH) return m_minw; }
    else { if (m_minh != AUTOWH) return m_minh; }
  }

  // done
  return default_size;
}

int XUIObject::getPreferences(int which) {
  if (m_image) {
    int margins[4];
    m_wnd->getMargins(&margins[0], &margins[1], &margins[2], &margins[3]);
    if (which == SUGGESTED_W) return m_image->getWidth() + margins[0] + margins[2];
    if (which == SUGGESTED_H) return m_image->getHeight() + margins[1] + margins[3];
  }
  return AUTOWH;
}

int XUIObject::isMouseThrough() {
  if (m_mousethrough == -1) {
    if (m_parent) 
      return m_parent->isMouseThrough();
    return 0;
  }
  return m_mousethrough;
}

int XUIObject::isDndThrough() {
  return !getAttributeBool("dndtarget");
}

void XUIObject::addAttribForwarding(XUIObject *source, const char *sourceattr, const char *mappedattr) {
  m_attrforwarders.addItem(new XUIAttribForwarder(source, sourceattr, this, mappedattr));
}

// apply the flex rules to all children
void XUIObject::reflow() {
  int totaldimension;
  RECT r;
  m_wnd->getClientRect(&r);
  int clientwidth = r.right-r.left;
  int clientheight = r.bottom-r.top;
  int offset;
  // get metrics for this object
  switch (m_flex_orientation) {
    case FLEXBOX_ORIENTATION_VERTICAL:
      offset = r.top;
      totaldimension = r.bottom - r.top;
      break;
    case FLEXBOX_ORIENTATION_HORIZONTAL:
      offset = r.left;
      totaldimension = r.right - r.left;
      break;
  }
  
  int w,h,i;
  int totalflex = 0;
  // first pass: add up all the flex values, and determine how much to divide up for the children in the second pass
  for (i=0;i<getNumChildren();i++) {
    RECT cr;
    int min, max;
    XUIObject *child = enumChild(i);
    child->getRootWnd()->getNonClientRect(&cr);
    int flex = child->getFlex();
    if (flex < 0) continue;
    switch (m_flex_orientation) {
      case FLEXBOX_ORIENTATION_VERTICAL:
        h = child->getAttributeH();
        if (h == AUTOWH && flex == 0) {
          h = child->getDefaultSize(XUI_DEFAULTSIZE_HEIGHT);
        }
        if (h != AUTOWH && flex == 0) {
          if (child->getAttributeY() == AUTOWH) {
            min = child->getAttributeMinH();
            max = child->getAttributeMaxH();
            if (min != AUTOWH) h = MAX(h, min);
            if (max != AUTOWH) h = MIN(h, max);
            h = MIN(h, clientheight);
            totaldimension -= h;
          }
          continue;
        }
        break;
      case FLEXBOX_ORIENTATION_HORIZONTAL:
        w = child->getAttributeW();
        if (w == AUTOWH && flex == 0) {
          w = child->getDefaultSize(XUI_DEFAULTSIZE_WIDTH);
        }
        if (w != AUTOWH && flex == 0) {
          if (child->getAttributeX() == AUTOWH) {
            min = child->getAttributeMinW();
            max = child->getAttributeMaxW();
            if (min != AUTOWH) h = MAX(h, min);
            if (max != AUTOWH) h = MIN(h, max);
            w = MIN(w, clientwidth);
            totaldimension -= w;
          }
          continue;
        }
        break;
    }
    totalflex += flex;
  }
  for (i=0;i<getNumChildren();i++) {
    int size;
    int pos;
    int opos;
    int min, max, omin, omax;
    int osize;
    RECT cr;
    XUIObject *child = enumChild(i);
    child->getRootWnd()->getNonClientRect(&cr);
    int flex = child->getFlex();
    switch (m_flex_orientation) {
      case FLEXBOX_ORIENTATION_VERTICAL:
        size = child->getAttributeH();
        if (size == AUTOWH && flex == 0) {
          size = child->getDefaultSize(XUI_DEFAULTSIZE_HEIGHT);
          size = MIN(size, clientheight);
        }
        min = child->getAttributeMinH();
        max = child->getAttributeMaxH();
        pos = child->getAttributeY();
        if (pos != AUTOWH) pos += r.top;
        opos = child->getAttributeX();
        if (opos != AUTOWH) opos += r.left;
        else opos = r.left;
        osize = child->getAttributeW();
        osize = (osize == AUTOWH) ? clientwidth : osize;
        omin = child->getAttributeMinW();
        omax = child->getAttributeMaxW();
        break;
      case FLEXBOX_ORIENTATION_HORIZONTAL:
        size = child->getAttributeW();
        if (size == AUTOWH && flex == 0) {
          size = child->getDefaultSize(XUI_DEFAULTSIZE_WIDTH);
          size = MIN(size, clientwidth);
        }
        min = child->getAttributeMinW();
        max = child->getAttributeMaxW();
        pos = child->getAttributeX();
        if (pos != AUTOWH) pos += r.left;
        opos = child->getAttributeY();
        if (opos != AUTOWH) opos += r.top;
        else opos = r.top;
        osize = child->getAttributeH();
        osize = (osize == AUTOWH) ? clientheight : osize;
        omin = child->getAttributeMinH();
        omax = child->getAttributeMaxH();
        break;
    }
    if (flex > 0) {
      size = (int)(((float)flex / (float)totalflex) * (float)totaldimension + 0.5f);
    } else if (flex < 0) {
      size = totaldimension; // todo: only add enough to go to the other edge, ie if pos > 0 then size passes right/bottom edge
      if (m_flex_orientation == FLEXBOX_ORIENTATION_VERTICAL) {
        if (pos == AUTOWH) pos = r.top;
      } else {
        if (pos == AUTOWH) pos = r.left;
      }
    }
    if (min != AUTOWH) size = MAX(size, min);
    if (max != AUTOWH) size = MIN(size, max);
    if (omin != AUTOWH) osize = MAX(omin, osize);
    if (omax != AUTOWH) osize = MIN(omax, osize);
    switch (m_flex_orientation) {
      case FLEXBOX_ORIENTATION_VERTICAL:
        osize = MIN(osize, clientwidth);
        child->getRootWnd()->resize(opos, (pos == AUTOWH) ? offset : pos, osize, size);
        break;
      case FLEXBOX_ORIENTATION_HORIZONTAL:
        osize = MIN(osize, clientheight);
        child->getRootWnd()->resize((pos == AUTOWH) ? offset : pos, opos, size, osize);
        break;
    }
    if (pos == AUTOWH)
      offset += size;
  }

}

void XUIObject::registerScript(XUIScript *script) {
  m_scripts.addItem(script);
}

int XUIObject::getNumScripts() {
  return m_scripts.getNumItems();
}

XUIScript *XUIObject::enumScript(int n) {
  return m_scripts.enumItem(n);
}

void XUIObject::onDestroy() {
  eventSource_onDestroy(); 
  js_onDestroy();
  foreach(m_children)
    XUIObject *child = m_children.getfor();
    svc_xuiObject *svc = child->getOwnerService();
    if (svc == NULL) {
      DebugString("Owner service is NULL for XUIObject %s", getTagName());
    } else {
      svc->destroy(child);
    }
  endfor;
  m_children.removeAll();
  foreach(m_scripts)
    XUIScript *script = m_scripts.getfor();
    delete script;
  endfor;
  m_scripts.removeAll();
}

void XUIObject::registerLocalGroupDefinition(DOMNode *node) {
  DOMNamedNodeMap *attributes = node->getAttributes();
  DOMAttr *attr = static_cast<DOMAttr*>(attributes->getNamedItem("id"));
  const char *id = NULL;
  if (attr) id = attr->getNodeValue();

  // groupdef has no id, can't register it, ignoring.
  if (!id) return;

  // clone the node, and insert it into a local document fragment. this
  // changes the node ownership, so it'll be deleted when the fragment
  // goes away, not when the original document does
  DOMDocumentFragment *fragment = new DOMDocumentFragment(); 
  DOMNode *clone = node->cloneNode(true);
  fragment->appendChild(clone);

  // record the fragment pointer so we remember to delete it, and all its cloned nodes along with it
  m_groupdefsFragments.addItem(fragment);

  // register the cloned node in our map of id->list entries, at the end of the list, so it overrides any previous such id
  PtrList<DOMNode> *groupId = m_groupdefs.getItemByValue(String(id), NULL);
  if (!groupId) {
    groupId = new PtrList<DOMNode>;
    m_groupdefs.addItem(String(id), groupId);
  }
  groupId->addItem(clone);
}

DOMNode *XUIObject::getLocalGroupDefinition(const char *id) {
  PtrList<DOMNode> *groupId = m_groupdefs.getItemByValue(String(id), NULL);
  if (!groupId) {
    if (m_parent) 
      return m_parent->getLocalGroupDefinition(id);
    return NULL;
  }
  return groupId->getLast();
}

PtrList<XUIObject> *XUIObject::getElementsById(const char *id, bool local) {
  m_xuiobjects_returnlist.removeAll();
  FindElementById fbi(id);
  findElements(&fbi, &m_xuiobjects_returnlist, local);
  return &m_xuiobjects_returnlist;
}

PtrList<XUIObject> *XUIObject::getElementsByTagName(const char *tag, bool local) {
  m_xuiobjects_returnlist.removeAll();
  FindElementByTagName fbt(tag);
  findElements(&fbt, &m_xuiobjects_returnlist, local);
  return &m_xuiobjects_returnlist;
}

PtrList<XUIObject> *XUIObject::getElementsByAttribute(const char *attrName, const char *attrValue, bool local) {
  m_xuiobjects_returnlist.removeAll();
  FindElementByAttribute fba(attrName, attrValue);
  findElements(&fba, &m_xuiobjects_returnlist, local);
  return &m_xuiobjects_returnlist;
}

void XUIObject::onBeginDrag(XUIDndSession *s) {
  m_dndDragging = true;
  XUIDndSession *session = g_xuiDndManager->newSession(this);
  ASSERT(session);
  eventSource_onBeginDrag(session);
}

void XUIObject::onEndDrag(XUIDndSession *s, bool success) {
  XUIDndSession *session = g_xuiDndManager->getSession();
  ASSERT(session);
  m_dndDragging = false;
  eventSource_onEndDrag(session, success);
  g_xuiDndManager->endSession();
}

void XUIObject::dragEnter(RootWnd *dragSource) {
  XUIDndSession *session = g_xuiDndManager->getSession();
  ASSERT(session);
  session->resetAcceptDrop();
  onDragEnter(session);
  // check again, since callback may have caused the session to be deleted (ie, via an alert->cancelCapture->dragComplete(failed))
  session = g_xuiDndManager->getSession();
  if (session) session->onDragEnter(this);
}

void XUIObject::dragLeave(RootWnd *dragSource) {
  XUIDndSession *session = g_xuiDndManager->getSession();
  ASSERT(session);
  onDragLeave(session);
  // check again, since callback may have caused the session to be deleted (ie, via an alert->cancelCapture->dragComplete(failed))
  session = g_xuiDndManager->getSession();
  if (session) session->onDragLeave(this);
  // check again, since callback may have caused the session to be deleted (ie, via an alert->cancelCapture->dragComplete(failed))
  session = g_xuiDndManager->getSession();
  if (session) session->resetAcceptDrop();
}

int XUIObject::dragOver(RootWnd *dragSource, int x, int y) {
  XUIDndSession *session = g_xuiDndManager->getSession();
  ASSERT(session);
  onDragOver(session, x, y);
  // check again, since callback may have caused the session to be deleted (ie, via an alert->cancelCapture->dragComplete(failed))
  session = g_xuiDndManager->getSession();
  if (session) session->onDragOver(this, x, y);
  return session->dropAccepted();
}

void XUIObject::dragDrop(RootWnd *dragSource, int x, int y) {
  XUIDndSession *session = g_xuiDndManager->getSession();
  ASSERT(session);
  session->setDropTarget(this);
  onDragDrop(session, x, y);
  // check again, since callback may have caused the session to be deleted (ie, via an alert->cancelCapture->dragComplete(failed))
  session = g_xuiDndManager->getSession();
  if (session) session->onDragDrop(this, x, y);
}

void XUIObject::dragComplete(int success) {
  onEndDrag(NULL, success != 0);
}

void XUIObject::addDragObject(const char *type, void *obj) {
  m_wnd->addDragItem(type, obj);
}

XUIObject *XUIObject::getLastChild() {
  return m_children.getLast();
}

XUIObject *XUIObject::getFirstChild() {
  return m_children.getFirst();
}
