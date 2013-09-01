#ifndef _XMLPARSE_H
#define _XMLPARSE_H

class XMLParse {
private:
	void *parser;

public:
  XMLParse();
  virtual ~XMLParse();

	virtual void SetUserData(void *param);
	virtual void SetElementHandler(void (*start)(void *userData, const char *name, const char **atts),
																 void (*end)(void *userData, const char *name));
	virtual void SetCharacterDataHandler(void (*handler)(void *userData,const char *s, int len));
	virtual int Parse(const char *s, int len, int isFinal);
	virtual const char *ErrorString(int code);
	virtual int GetErrorCode();
	virtual int GetCurrentLineNumber();

};

#endif
