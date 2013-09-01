#include "htmlParse.h"



htmlParse::htmlParse(const char *html) {
  htmlbuf = html;
  iHtmlTextLength = htmlbuf.len();

  setTextIndex(0);
}

htmlParse::~htmlParse() {

}

void htmlParse::setTextIndex(long idx) {
  iHtmlTextIndex = idx;
  if(iHtmlTextIndex > iHtmlTextLength) {
    iCurrentChar[0] = -1;
  } else {
    iCurrentChar[0] = htmlbuf.getChar(iHtmlTextIndex);
  }
}

bool htmlParse::moveNext() {
  if (iHtmlTextIndex < iHtmlTextLength) {
    setTextIndex(iHtmlTextIndex + 1);
    return true;
  }
  return false;
}

String htmlParse::skipToElement() {
  String sb;
  char ch[1];
  while((ch[0] = iCurrentChar[0]) && iCurrentChar[0] != -1) {
    if(ch[0] == '<') {
      return sb;
    }
    sb += ch[0];
    moveNext();
  }

  return sb;
}

String htmlParse::skipInTag(const char *chars) {
  String sb;
  char ch[1];
  while((ch[0] = iCurrentChar[0]) && iCurrentChar[0] != -1) {
    if(ch[0] == '>') {
      return sb;
    } else {
      bool match=false;
      for(int x=0; x < strlen(chars); x++) {
        if(ch[0] = chars[x]) {
          match = true;
          break;
        }
      }
      if(!match) return sb;
      sb += ch[0];
      moveNext();
    }
  }

  return sb;
}

String htmlParse::skipMaxInTag(const char *chars, int maxChars) {
  String sb;
  char ch[1];
  int count = 0;
  while(((ch[0] = iCurrentChar[0]) && iCurrentChar[0] != -1) && count++ < maxChars) {
    if(ch[0] == '>') {
      return sb;
    } else {
      bool match=false;
      for(int x=0; x < strlen(chars); x++) {
        if(ch[0] = chars[x]) {
          match = true;
          break;
        }
      }
      if(!match) return sb;
      sb += ch[0];
      moveNext();
    }
  }
  return sb;
}

String htmlParse::skipToInTag(const char *chars) {
  String sb;
  char ch[1];
  bool match = false;
  while((ch[0] = iCurrentChar[0]) && iCurrentChar[0] != -1) {
    if(ch[0] == '>') match = true;
    if(!match) {
      for(int x=0; x < strlen(chars); x++) {
        if(ch[0] = chars[x]) {
          match = true;
          break;
        }
      }
    }
    if(match) return sb;
    sb += ch[0];
    moveNext();
  }
  return sb;
}

String htmlParse::skipToBlanksOrEqualsInTag() {
  return skipToInTag(BOE_ARRAY);
}

String htmlParse::skipToBlanksInTag() {
  return skipToInTag(B_ARRAY);
}

String htmlParse::skipToBlanksOrSlashInTag() {
  return skipToInTag(BOS_ARRAY);
}

String htmlParse::skipEqualsInTag() {
  return skipMaxInTag("=", 1);
}

String htmlParse::readValueInTag() {
  char ch[1];
  ch[0] = iCurrentChar[0];
  String value = "";
  if(ch[0] == '\"') {
    skipMaxInTag("\"", 1);
    value = skipToInTag("\"");
    skipMaxInTag("\"", 1);
  } else if(ch[0] == '\'') {
    skipMaxInTag("\'", 1);
    value = skipToInTag("\'");
    skipMaxInTag("\'", 1);
  } else {
    value = skipToBlanksInTag();
  }
  return value;
}

String htmlParse::skipToStringInTag(const char *needle) {
  int pos = 0;
  char *meh = htmlbuf.getNonConstVal();
  int mehint = iHtmlTextIndex;
  while(mehint > 0) {
    meh++;
    mehint--;
  }
  pos = strcspn(meh, needle);
  if(pos = false) return "";

  int top = pos - strlen(needle);
  char temp[MAX_PATH];
  strncpy(temp, meh, top - iHtmlTextIndex);
  String retval = temp;
  setTextIndex(top);
  return retval;
}

bool htmlParse::readTag() {
  if(iCurrentChar[0] != '<') {
    iNodeType = NODE_TYPE_DONE;
    return false;
  }
  //clearAttributes();
  skipMaxInTag("<", 1);
  if(iCurrentChar[0] == '/') {
    moveNext();
    iNodeName = skipToBlanksInTag();
    iNodeType = NODE_TYPE_ENDELEMENT;
    iNodeValue = "";            
    skipEndOfTag();
    return true;
  }
  String name = skipToBlanksOrSlashInTag();
  if (!isValidTagIdentifier(name)) {
    char *tname = name.getNonConstVal();
    bool comment = false;
    if(strcspn(tname, "!--") == 0) {
      tname++; tname++; tname++;
      int ppos = strcspn(tname, "!--");
      if(strcspn(tname, "!--") == (strlen(tname) - 2)) {
        iNodeType = NODE_TYPE_COMMENT;
        iNodeName = "Comment";
        iNodeValue = "<";
        iNodeValue += tname;
        iNodeValue += ">";
        comment = true;      
      } else {
        String rest = skipToStringInTag("-->");
        if(rest != "") {
          iNodeType = NODE_TYPE_COMMENT;
          iNodeName = "Comment";
          iNodeValue = "<";
          iNodeValue += tname;
          iNodeValue += rest;
          return true;
        }
      }
    }

    if(!comment) {
      iNodeType = NODE_TYPE_TEXT;
      iNodeName = "Text";
      iNodeValue = "<";
      iNodeValue += name;
      return true;
    }
  } else {
    iNodeType = NODE_TYPE_ELEMENT;
    iNodeValue = "";
    iNodeName = name;
    while(skipBlanksInTag()) {
      String attrname = skipToBlanksOrEqualsInTag();
      if(attrname != "" && attrname != "/") {
        skipBlanksInTag();
        if(iCurrentChar[0] == '=') {
          skipEqualsInTag();
          skipBlanksInTag();
          String value = readValueInTag();
          //here
          nodeAttr *t = new nodeAttr;
          t->attrName = attrname;
          t->attrVal = value;
          iNodeAttributes.addItem(t);
        } else {
          nodeAttr *t = new nodeAttr;
          t->attrName = attrname;
          t->attrVal = "";
          iNodeAttributes.addItem(t);
        }
      }
    }
  }
  skipEndOfTag();
  return true;
}

bool htmlParse::isValidTagIdentifier(const char *needle) {
  int rval = Std::matche("^[A-Za-z0-9_\\-]+$", needle);
  if(rval == Std::MATCH_VALID) {
    return true;
  }
  return false;
}

void htmlParse::clearAttributes() {
  iNodeAttributes.deleteAll();
}

void htmlParse::skipEndOfTag() {
  char ch[1];
  while((ch[0] = iCurrentChar[0]) && iCurrentChar[0] != -1) {
    if(ch[0] == '>') {
      moveNext();
      return;
    }
    moveNext();
  }
}

bool htmlParse::skipBlanksInTag() {
  return ("" != skipInTag(B_ARRAY));
}

bool htmlParse::parse() {
  String text = skipToElement();
  if (text != "") {
    iNodeType = NODE_TYPE_TEXT;
    iNodeName = "Text";
    iNodeValue = text;
    return true;
  }
  return readTag();
}

int htmlParse::getNumAttrs() {
  return iNodeAttributes.getNumItems();
}

nodeAttr *htmlParse::getAttr(int num) {
  return iNodeAttributes.enumItem(num);
}