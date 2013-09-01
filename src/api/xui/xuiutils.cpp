#include <precomp.h>
#include <bfc/string/stringdict.h>
#include <wnd/wnds/statictextwnd.h>
#include <wnd/basewnd.h>
#include <parse/pathparse.h>
#include <api/xui/widgets/xuiobject.h> 
#include <api/wdf/wdf.h>
#include "dom/domnode.h"
#include "xuiutils.h"

int parseBoolAttribute(const char *value, int allowinherit) {
  if (!value) return 0;
  if (STRCASEEQLSAFE(value, "true")) return 1;
  if (STRCASEEQLSAFE(value, "false")) return 0;
  if (STRCASEEQLSAFE(value, "yes")) return 1;
  if (STRCASEEQLSAFE(value, "no")) return 0;
  if (allowinherit) {
    if (STRCASEEQLSAFE(value, "inherit")) return -1;
  }
  return (atoi(value) != 0) ? 1 : 0;
}

BEGIN_STRINGDICTIONARY(_COLOR_TABLE);
SDI("maroon",                  0x000080);
SDI("red",                     0x0000FF);
SDI("orange",                  0x00A5FF);
SDI("yellow",                  0x00FFFF);
SDI("olive",                   0x008080);
SDI("purple",                  0x800080);
SDI("fushia",                  0xFF00FF);
SDI("white",                   0xFFFFFF);
SDI("lime",                    0x00FF00);
SDI("green",                   0x008000);
SDI("navy",                    0x800000);
SDI("blue",                    0xFF0000);
SDI("aqua",                    0xFFFF00);
SDI("teal",                    0x808000);
SDI("black",                   0x000000);
SDI("silver",                  0xC0C0C0);
SDI("gray",                    0x808080);
SDI("sys-activeborder",        GetSysColor(COLOR_ACTIVEBORDER));
SDI("sys-activecaption",       GetSysColor(COLOR_ACTIVECAPTION));
SDI("sys-appworkspace",        GetSysColor(COLOR_APPWORKSPACE));
SDI("sys-background",          GetSysColor(COLOR_BACKGROUND));
SDI("sys-buttonface",          GetSysColor(COLOR_BTNFACE));
SDI("sys-buttonhighlight",     GetSysColor(COLOR_BTNHIGHLIGHT));
SDI("sys-buttonshadow",        GetSysColor(COLOR_BTNSHADOW));
SDI("sys-buttontext",          GetSysColor(COLOR_BTNTEXT));
SDI("sys-captiontext",         GetSysColor(COLOR_CAPTIONTEXT));
SDI("sys-graytext",            GetSysColor(COLOR_GRAYTEXT));
SDI("sys-highlight",           GetSysColor(COLOR_HIGHLIGHT));
SDI("sys-highlighttext",       GetSysColor(COLOR_HIGHLIGHTTEXT));
SDI("sys-inactiveborder",      GetSysColor(COLOR_INACTIVEBORDER));
SDI("sys-inactivecaption",     GetSysColor(COLOR_INACTIVECAPTION));
SDI("sys-inactivecaptiontext", GetSysColor(COLOR_INACTIVECAPTIONTEXT));
SDI("sys-infobackground",      GetSysColor(COLOR_INFOBK));
SDI("sys-infotext",            GetSysColor(COLOR_INFOTEXT));
SDI("sys-menu",                GetSysColor(COLOR_MENU));
SDI("sys-menutext",            GetSysColor(COLOR_MENUTEXT));
SDI("sys-scrollbar",           GetSysColor(COLOR_SCROLLBAR));
SDI("sys-3ddarkshadow",        GetSysColor(COLOR_3DDKSHADOW));
SDI("sys-3dface",              GetSysColor(COLOR_3DFACE));
SDI("sys-3dhighlight",         GetSysColor(COLOR_3DHIGHLIGHT));
SDI("sys-3dlightshadow",       GetSysColor(COLOR_3DLIGHT));
SDI("sys-3dshadow",            GetSysColor(COLOR_3DSHADOW));
SDI("sys-window",              GetSysColor(COLOR_WINDOW));
SDI("sys-windowframe",         GetSysColor(COLOR_WINDOWFRAME));
SDI("sys-windowtext",          GetSysColor(COLOR_WINDOWTEXT));
END_STRINGDICTIONARY(_COLOR_TABLE, colorTable);

#define ARGB(a,r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)|(((DWORD)(BYTE)(a))<<24)))

int parseColorAttribute(const char *color, COLORREF *colorValue) {
  if (STRCASEEQL(color, "none")) {
    *colorValue = 0;
    return 1;
  }
  int notfound=0;
  COLORREF c = colorTable.getId(color, &notfound);
  if (!notfound) {
    *colorValue = (c | 0xFF000000);
    return 1;
  }
  if (STRCHR(color, ',')) {
    int a=0,r=0,g=0,b=0;
    if (SSCANF(color, "%a,%d,%d,%d", &a, &r, &g, &b) != 4) {
      if (SSCANF(color, "%d,%d,%d", &r, &g, &b) != 3) {
        return 0;
      }
      *colorValue = ARGB(0xFF, r, g, b);
      return 1;
    }
    *colorValue = ARGB(a, r, g, b);
    return 1;
  }
  if (*color == '#') {
    int a=0,r=0,g=0,b=0;
    if (SSCANF(color, "#%02x%02x%02x%02x", &a, &r, &g, &b) != 4) {
      if (SSCANF(color, "#%02x%02x%02x", &r, &g, &b) != 3) {
        return 0;
      }
      *colorValue = ARGB(0xFF, r, g, b);
      return 1;
    }
    *colorValue = ARGB(a, r, g, b);
    return 1;
  } 
  //TODO: try rgb(%d,%d,%d), rgba(%d,%d,%d,%d)
  return 0;
}

void parseColorAndShadowAttribute(const char *colorattr, COLORREF *color, COLORREF *shadow, int *gotcolor, int *gotshadow) {
  *gotcolor = *gotshadow = 0;
  String s = colorattr;
  char *p = STRCHR(s, ',');
  if (p) *p++ = 0;
  int r = parseColorAttribute(s, color);
  if (r) *gotcolor = 1;
  if (p) {
    r = parseColorAttribute(p, shadow);
    if (r) *gotshadow = 1;
  }
}

BEGIN_STRINGDICTIONARY(_FONTSTYLE_TABLE);
SDI("normal",          FONTSTYLE_NORMAL);
SDI("bold",            FONTSTYLE_BOLD);
SDI("italic",          FONTSTYLE_ITALIC);
SDI("underline",       FONTSTYLE_UNDERLINE);
END_STRINGDICTIONARY(_FONTSTYLE_TABLE, fontStyleTable);

int parseFontStyleAttribute(const char *style) {
  int flags = FONTSTYLE_NORMAL; // 0
  PathParser pp(style, "|");
  for (int i=0;i<pp.getNumStrings();i++) {
    int r = fontStyleTable.getId(pp.enumString(i));
    if (r != -1) flags |= r;
  }
  return flags;
}

void parseCSVIntAttribute(const char *values, int *ret, int nvals, int defaultvalue) {
  int i;
  PathParser pp(values, ",");
  for (i=0;i<nvals;i++)
    ret[i] = defaultvalue;
  for (i=0;i<pp.getNumStrings();i++) {
    const char *v = pp.enumString(i);
    while (v && *v) {
      if (*v == ' ') v++;
      else break;
    }
    if (!v || *v == 0) continue;
    if (*v == '*' && v[1] == 0) ret[i] = defaultvalue;
    else ret[i] = atoi(v);
  }
}

BEGIN_STRINGDICTIONARY(_BGSTYLE_TABLE);
SDI("none",          XUIObject::XUI_BGSTYLE_NONE);
SDI("flat",          XUIObject::XUI_BGSTYLE_FLAT);
SDI("sunken",        XUIObject::XUI_BGSTYLE_SUNKEN);
SDI("raised",        XUIObject::XUI_BGSTYLE_RAISED);
END_STRINGDICTIONARY(_BGSTYLE_TABLE, bgStyleTable);

int parseBGStyleAttribute(const char *value) {
  int r = bgStyleTable.getId(value);
  if (r == -1) r = XUIObject::XUI_BGSTYLE_NONE;
  return r;
}

String unrelativizedPath;
const char *unrelativizePath(const char *file) {
  if (STRCHR(file, DIRCHAR)) return file;
  unrelativizedPath = WASABI_API_APP->path_getAppPath();
  if (!Std::isDirChar(unrelativizedPath.lastChar())) unrelativizedPath += DIRCHARSTR;
  unrelativizedPath += file;
  return unrelativizedPath;
}

const char *getImageFile(DefinitionsFramework *manager, const char *image) {
  DOMNode *node = manager->getTopLevelNode("define-image", image);
  if (node) {
    DOMNode *imageAttrib = node->getAttributes()->getNamedItem("file");
    if (imageAttrib) {
      image = unrelativizePath(imageAttrib->getNodeValue());
    }
  }
  return image;
}