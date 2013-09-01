#include <precomp.h>
#include <bfc/string/stringdict.h>
#include <wnd/wnds/statictextwnd.h>
#include "xuilabel.h"

// Service declaration "label"

_DECLARE_XUISERVICE(XUILabelSvc);

// XUI object

enum {
  XUILABEL_ATTRIBUTE_VALUE,
  XUILABEL_ATTRIBUTE_COLOR,
  XUILABEL_ATTRIBUTE_DISABLEDCOLOR,
  XUILABEL_ATTRIBUTE_SIZE,
  XUILABEL_ATTRIBUTE_FONT,
  XUILABEL_ATTRIBUTE_STYLE,
  XUILABEL_ATTRIBUTE_MARGIN,
};

BEGIN_STRINGDICTIONARY(_XUILABEL_ATTRIBUTES);
SDI("value",          XUILABEL_ATTRIBUTE_VALUE);
SDI("color",          XUILABEL_ATTRIBUTE_COLOR);
SDI("disabledcolor",  XUILABEL_ATTRIBUTE_DISABLEDCOLOR);
SDI("size",           XUILABEL_ATTRIBUTE_SIZE);
SDI("font",           XUILABEL_ATTRIBUTE_FONT);
SDI("style",          XUILABEL_ATTRIBUTE_STYLE);
END_STRINGDICTIONARY(_XUILABEL_ATTRIBUTES, xuiLabelAttributes);

void XUILabel::onAttributeChanged(const char *attributeName, const char *attributeValue) {
  XUILabel_PARENT::onAttributeChanged(attributeName, attributeValue);
  switch (xuiLabelAttributes.getId(attributeName)) {
    case XUILABEL_ATTRIBUTE_VALUE:
      m_statictext->setName(attributeValue);
      if (m_statictext->isPostOnInit()) {
        m_statictext->invalidate();
      }
      break;
    case XUILABEL_ATTRIBUTE_COLOR: {
      COLORREF color, shadow;
      int gotcolor, gotshadow;
      parseColorAndShadowAttribute(attributeValue, &color, &shadow, &gotcolor, &gotshadow);
      if (gotcolor) m_statictext->setTextColor(color);
      if (gotshadow) m_statictext->setTextShadowColor(shadow);
      break;
    }
    case XUILABEL_ATTRIBUTE_DISABLEDCOLOR: {
      COLORREF color, shadow;
      int gotcolor, gotshadow;
      parseColorAndShadowAttribute(attributeValue, &color, &shadow, &gotcolor, &gotshadow);
      if (gotcolor) m_statictext->setDisabledTextColor(color);
      if (gotshadow) m_statictext->setDisabledTextShadowColor(shadow);
      break;
    }
    case XUILABEL_ATTRIBUTE_SIZE: {
      int size = atoi(attributeValue);
      m_statictext->setTextSize(size);
      break;
    }
    case XUILABEL_ATTRIBUTE_FONT:
      m_statictext->setTextFont(attributeValue);
      break;
    case XUILABEL_ATTRIBUTE_STYLE:
      m_statictext->setTextStyle(parseFontStyleAttribute(attributeValue));
      break;
  }
}

