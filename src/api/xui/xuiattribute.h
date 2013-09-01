#ifndef XUIATTRIBUTE_H
#define XUIATTRIBUTE_H

class XUIAttribute {
public:
  XUIAttribute(const char *attributeName, const char *attributeValue) :
    m_name(attributeName),
    m_value(attributeValue)
  {}
  const char *getName() { return m_name; }
  const char *getValue() { return m_value; }
  void setValue(const char *val) { m_value = val; }

private:
  String m_name;
  String m_value;
};

class XUIAttributeSortSearch {
public:
  static int compareItem(void *p1, void *p2) {
    return STRICMP((static_cast<XUIAttribute *>(p1))->getName(), (static_cast<XUIAttribute *>(p2))->getName());
  }
  static int compareAttrib(const char *attrib, void *item) {
    return STRICMP(attrib, (static_cast<XUIAttribute *>(item))->getName());
  }
};

class XUIAttributeList : public PtrListInsertSorted<XUIAttribute, XUIAttributeSortSearch> {
public:
  int hasAttribute(const char *attributeName) {
    return (getAttribute(attributeName) != NULL);
  }

  XUIAttribute *getAttribute(const char *attributeName) {
    return findItem(attributeName);
  }

  // addAttribute should never be called with an attribute name that is already in the list !
  // searchResult should always be NULL, it is there to remind you to do the dup check yourself before calling addAttribute
  // (yes, you could bypass this by always sending NULL no matter what, but that would be extremely dumb, do not do that.)
  XUIAttribute *addAttribute(const char *attributeName, const char *attributeValue, XUIAttribute *searchResult) {
    if (searchResult != NULL) {
      DebugString("searchResult should always be NULL, do not add an existing attribute! LOGIC ERROR");
      return NULL;
    }
    return addItem(new XUIAttribute(attributeName, attributeValue));
  }
};

#endif
