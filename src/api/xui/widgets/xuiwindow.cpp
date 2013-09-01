#include <precomp.h>
#include <bfc/string/stringdict.h>
#include "xuiwindow.h"

_DECLARE_XUISERVICE(XUIWindowSvc);

enum {
  WINDOWXUIOBJECT_ATTRIBUTE_TITLE,
};

BEGIN_STRINGDICTIONARY(_WINDOWXUIOBJECT_ATTRIBUTES);
SDI("title",       WINDOWXUIOBJECT_ATTRIBUTE_TITLE);
END_STRINGDICTIONARY(_WINDOWXUIOBJECT_ATTRIBUTES, windowXuiObjectAttributes);

void XUIWindow::onAttributeChanged(const char *attributeName, const char *attributeValue) {
  XUIWindow_PARENT::onAttributeChanged(attributeName, attributeValue);
  switch (windowXuiObjectAttributes.getId(attributeName)) {
    case WINDOWXUIOBJECT_ATTRIBUTE_TITLE:
      m_wnd->setName(attributeValue);
      break;
  }
}

