#ifndef FINDELEMENT_H
#define FINDELEMENT_H

class FindElementCallback {
public:
  virtual int testElement(XUIObject *element)=0;
};

class FindElementById : public FindElementCallback {
public:
  FindElementById(const char *id) : m_id(id) {}
  virtual int testElement(XUIObject *xo) {
    return STRCASEEQLSAFE(xo->getAttribute("id"), m_id);
  }
private:
  String m_id;
};

class FindElementByTagName : public FindElementCallback {
public:
  FindElementByTagName(const char *tagname) : m_tagname(tagname) {}
  virtual int testElement(XUIObject *xo) {
    return STRCASEEQLSAFE(xo->getTagName(), m_tagname);
  }
private:
  String m_tagname;
};

class FindElementByAttribute : public FindElementCallback {
public:
  FindElementByAttribute(const char *attributeName, const char *attributeValue) : m_attr(attributeName), m_value(attributeValue) {}
  virtual int testElement(XUIObject *xo) {
    return STRCASEEQLSAFE(xo->getAttribute(m_attr), m_value);
  }
private:
  String m_attr;
  String m_value;
};

#endif
