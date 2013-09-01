#include <precomp.h>
#include <bfc/string/stringdict.h>
#include <wnd/wnds/statictextwnd.h>
#include "xuiedit.h"

// Service declaration "edit"

_DECLARE_XUISERVICE(XUIEditSvc);

// XUI object

enum {
  XUITEXTBOX_ATTRIBUTE_VALUE,
  XUITEXTBOX_ATTRIBUTE_TEXTCOLOR,
  XUITEXTBOX_ATTRIBUTE_DISABLEDBGCOLOR,
  XUITEXTBOX_ATTRIBUTE_MULTILINE,
  XUITEXTBOX_ATTRIBUTE_READONLY,
  XUITEXTBOX_ATTRIBUTE_PASSWORD,
};

BEGIN_STRINGDICTIONARY(_XUITEXTBOX_ATTRIBUTES);
SDI("value",            XUITEXTBOX_ATTRIBUTE_VALUE);
SDI("textcolor",        XUITEXTBOX_ATTRIBUTE_TEXTCOLOR);
SDI("disabledbgcolor",  XUITEXTBOX_ATTRIBUTE_DISABLEDBGCOLOR);
SDI("multiline",        XUITEXTBOX_ATTRIBUTE_MULTILINE);
SDI("readonly",         XUITEXTBOX_ATTRIBUTE_READONLY);
SDI("password",         XUITEXTBOX_ATTRIBUTE_PASSWORD);
END_STRINGDICTIONARY(_XUITEXTBOX_ATTRIBUTES, xuiEditAttributes);

XUIEdit::XUIEdit() {
  m_myvalueupdate = 0;
  m_buffer.setSize(1024);
}

XUIEdit::~XUIEdit() {
}

void XUIEdit::onSetWindow() {
  XUIEdit_PARENT::onSetWindow();
  m_edit = static_cast<EditWnd*>(m_wnd);
  m_edit->setBuffer(m_buffer.getMemory(), 1024);
}

void XUIEdit::onAttributeChanged(const char *attributeName, const char *attributeValue) {
  XUIEdit_PARENT::onAttributeChanged(attributeName, attributeValue);
  COLORREF color;
  if (STRCASEEQL(attributeName, "bgcolor")) {
    if (parseColorAttribute(attributeValue, &color))
      m_edit->setBackgroundColor(RGBTOBGR(color & 0xFFFFFF));
  } else {
    switch (xuiEditAttributes.getId(attributeName)) {
      case XUITEXTBOX_ATTRIBUTE_VALUE: {
        if (!m_myvalueupdate) {
          STRNCPY(m_buffer.getMemory(), attributeValue, 1023);
          m_buffer.getMemory()[1023] = 0;
          m_edit->setBuffer(m_buffer.getMemory(), 1024);
        }
        break;
      }
      case XUITEXTBOX_ATTRIBUTE_TEXTCOLOR:
        if (parseColorAttribute(attributeValue, &color))
          m_edit->setTextColor(RGBTOBGR(color & 0xFFFFFF));
        break;
      case XUITEXTBOX_ATTRIBUTE_DISABLEDBGCOLOR:
        if (parseColorAttribute(attributeValue, &color))
          m_edit->setDisabledBackgroundColor(RGBTOBGR(color & 0xFFFFFF));
        break;
      case XUITEXTBOX_ATTRIBUTE_MULTILINE: 
        m_edit->setMultiline(parseBoolAttribute(attributeValue));
        break;
      case XUITEXTBOX_ATTRIBUTE_READONLY: 
        m_edit->setReadOnly(parseBoolAttribute(attributeValue));
        break;
      case XUITEXTBOX_ATTRIBUTE_PASSWORD: 
        m_edit->setPassword(parseBoolAttribute(attributeValue));
        break;
    }
  }
}

void XUIEdit::selectAll() {
  m_edit->selectAll();
}

void XUIEdit::onEdit(const char *value) {
  m_myvalueupdate = 1;
  setAttribute("value", m_buffer.getMemory());
  m_myvalueupdate = 0;
  eventSource_onEdit(m_buffer.getMemory());
}

void XUIEdit::onIdleEdit(const char *value) {
  eventSource_onIdleEdit(m_buffer.getMemory());
}


