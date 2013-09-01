#ifndef _HTMLPARSE_H__
#define _HTMLPARSE_H__

#include <bfc/string/string.h>
#include <bfc/ptrlist.h>
#include <bfc/std.h>

#define BOE_ARRAY " \t\r\n="
#define B_ARRAY " \t\r\n"
#define BOS_ARRAY " \t\r\n/"

typedef struct {
  String attrName;
  String attrVal;
} nodeAttr;

class htmlParse {
public:
  enum {
    NODE_TYPE_START,
    NODE_TYPE_ELEMENT,
    NODE_TYPE_ENDELEMENT,
    NODE_TYPE_TEXT,
    NODE_TYPE_COMMENT,
    NODE_TYPE_DONE,
  };

	htmlParse(const char *html);
	virtual ~htmlParse();

  void setTextIndex(long idx);
  bool moveNext();

  String skipToElement();
  String skipInTag(const char *chars);
  String skipMaxInTag(const char *chars, int maxChars);
  String skipToInTag(const char *chars);

  String skipToBlanksOrEqualsInTag();
  String skipToBlanksInTag();
  String skipToBlanksOrSlashInTag();
  String skipEqualsInTag();

  String readValueInTag();
  String skipToStringInTag(const char *needle);

  bool isValidTagIdentifier(const char *needle);

  bool readTag();

  void clearAttributes();

  void skipEndOfTag();
  bool skipBlanksInTag();

  bool parse();

  int getNumAttrs();
  nodeAttr *getAttr(int num);

  String getNodeName() { return iNodeName; }
  String getNodeValue() { return iNodeValue; }
  int getNodeType() { return iNodeType; }

private:
  String htmlbuf;
/*
  const char *BOE_ARRAY;
  const char *B_ARRAY;
  const char *BOS_ARRAY;
*/
  char iCurrentChar[1];
  String iNodeName;
  String iNodeValue;
  int iNodeType;
  long iHtmlTextIndex;
  long iHtmlTextLength;
  PtrList<nodeAttr> iNodeAttributes;
};

/*
  How to use

int main() {
  htmlParse *psr = new htmlParse(html);
  while(psr->parse()) {
    if(psr->getNodeType() == htmlParse::NODE_TYPE_ELEMENT) printf("%s\n", psr->getNodeName());
  }
}

*/

#endif // !_HTMLPARSE_H__
