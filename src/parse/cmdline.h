#ifndef _CMDLINE_H
#define _CMDLINE_H

#include <bfc/string/string.h>
#include <parse/pathparse.h>

// utility class for parsing command lines that look like so
// app.exe /blah /gleh /something:param /blah:param1,param2 nakeditem

/**
  Command line parsing. Parses DOS/Win32 style command lines.
  
  Example: app.exe /switch /anotherswitch /a_switch=with_a_param /another_switch=param1,param2 stuff.mp3
  
  @short DOS/Win32 style command line parsing.
  @author Nullsoft
  @ver 1.0
  @see PathParser
*/
class CmdLine : public PathParser {
public:
  /**
    Pre-processes the commandline and initially splits it
    on whitespaces.
    
    @param cmdline The commandline to parse.
  */
  CmdLine(const char *cmdline) : PathParser(cmdline, " ") { }

  /**
    Look for a perticular switch in the commandline. If found
    it returns it's position. If more than one is found, the first
    position is returned.
    
    @param switchname The switch to look for.
    @ret The position of the switch in the command line.
  */
  int haveSwitch(const char *switchname) {
    return (getSwitchPos(switchname) >= 0);
  }
  
  /**
    Verify if a perticular position in the command line is
    a switch.
    
    @param pos The position in the command line to verify.
    @ret 0, No switch at that position; 1, Switch present at that position;
  */
  int isSwitch(int pos) {
    const char *p = enumString(pos);
    if (p == NULL) return 0;
    return (*p == SWITCHCHAR);
  }
  
  /**
    Get the position of a switch in the commandline.
    
    @param switchname The switch to look for.
    @ret The position of the switch in the commandline.
  */
  int getSwitchPos(const char *switchname, int nth=0) {
    ASSERT(switchname != NULL);
    String str;
    str.printf("%c%s", SWITCHCHAR, switchname);
    for (int i = 0; i < getNumStrings(); i++) {
      PathParser pp(enumString(i), "=");
      if (STRCASEEQL(pp.enumString(0), str)) {
ASSERT(nth >= 0);
        if (nth-- == 0) return i;
      }
    }
    return -1;
  }
  
  /**
    Get a paramater from a switch in the commandline.
    Here is an example of a paramater:
      app.exe /switch=param1,param2
      
    If there's no paramater present at the requested position,
    null will be returned.
      
    @param switchname The switch for which you want the paramaters.
    @param parampos   The position of the paramater you want to read.
    @ret A string containing the parameter value.
  */
  String getSwitchParam(const char *switchname, int parampos=0) {
    int pos = getSwitchPos(switchname);
    if (pos < 0) return String((const char *)NULL);
    PathParser pp(enumString(pos), "=");
    const char *s = pp.enumString(1);
    if (s == NULL) return String((const char *)NULL);
    PathParser pp2(s, ",");
    return String(pp2.enumString(parampos));
  }

  String getNthSwitchParam(int n, const char *switchname, int parampos=0) {
    int pos = getSwitchPos(switchname, n);
    if (pos < 0) return String((const char *)NULL);
    PathParser pp(enumString(pos), "=");
    const char *s = pp.enumString(1);
    if (s == NULL) return String((const char *)NULL);
    PathParser pp2(s, ",");
    return String(pp2.enumString(parampos));
  }

protected:
  /**
    Pre-processes the commandline before splitting it.
    This is to ensure that if quotes are used, they are interpreted
    correctly.
    
    @param str Reference to the commandline to pre-process.
  */
  virtual void preProcess(String &str) {
    char *nstr = str.getNonConstVal();
    for (int in=0; *nstr; nstr++) {
      if (*nstr == '"') in ^= 1;
      else if (in && *nstr == ' ') *nstr = 1;
    }
  }
  
  /**
    Post-processes the split pieces of the commandline 
    to ensure that if quotes are used, they are 
    interpretted correctly.
    
    @param str The piece of the commandline to post-process.
  */
  virtual void postProcess(char *str) {
    for (; *str; str++) {
      if (*str == 1) *str = ' ';
      else if (*str == '"') {
        STRCPY(str, str+1);
        str--;
      }
    }
  }
};

#endif
