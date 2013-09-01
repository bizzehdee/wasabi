#ifndef _STRING_H_WASABI
#define _STRING_H_WASABI

#ifdef __cplusplus

/**
   This is a very basic string class. It is not trying to be a be-all end-all
   string class. It's meant to be just enough functionality to avoid STRDUP().
   It is available to use in client apps, but no API in Wasabi depends on its
   use by client code, so you can ignore it if you like. ;)
   Also note that one of the design goals is to use same storage space as
   a char *, which means no virtual methods.
  @short Basic string class, to replace STRDUP/FREE
  @see StringPrintf
  @see DebugString
*/

#include <bfc/std_string.h>

class String {
public:
  String(const char *initial_val = NULL);
  String(const String &s);
  String(const String *s);
  ~String();

/**
  Returns the value of the string. It is a pointer to the internal storage
  used by the class, so you must not assume the pointer will not change,
  because it will.
  @see getValueSafe();
  @ret The value of the string.
*/
  const char *getValue() const { return val; }
  const char *v() const { return getValue(); } // for ease of typing
  const char *vs(const char *def_val="") const { return getValueSafe(def_val); } // for ease of typing
  operator const char *() const { return getValue(); }
/**
  Gets the value of the string, or a safe value if the value is NULL.
  @param def_val The value to return if the string's value is NULL. Defaults to "".
  @see getValue()
  @ret The value of the string, or a safe value if the value is NULL.
*/
  const char *getValueSafe(const char *def_val = "") const; // returns def_val if NULL

/**
  Returns the value of the character at a position in the string. If the
  position is invalid, returns -1. Note that unless bounds_check is set to
  TRUE, the pos will only be checked for negative values.
  @param pos The position of the character to return.
  @param bounds_check If TRUE, pos is checked against the length of the string.
  @see setChar()
  @ret The value of the character at the position in the string, or -1 if the position is invalid.
*/
  int getChar(int pos, int bounds_check = FALSE) const;
/**
  Sets the value of the character at a position in the string. Not multibyte UTF-8 safe yet. No lengthwise bounds checking yet.
  @param pos The position to set.
  @param value The value to set.
  @ret The value of the character set, or -1 if pos < 0.
*/
  int setChar(int pos, int value);

/**
  Sets the value of the string. The given value will be copied.
  Can accept NULL. Note that this may cause the
  pointer returned by getValue() et al to change.
  @param newval The new value of the string, nul-terminated.
  @ret The new value of the string.
  @see getValue()
*/
  const char *setValue(const char *newval);
/**
  Gets the value of the string in the form of a non-const char *. WARNING: you
  don't usually need to call this. If you want to modify the string,
  you can generally just use setChar(), or setValue().
  @ret The value of the string, casted to a non-const char *.
  @see getValue()
  @see setValue();
  @see setChar();
*/
  char *getNonConstVal() { return const_cast<char *>(getValue()); }
  char *ncv() { return getNonConstVal(); }
  const char *operator =(const char *newval) { return setValue(newval); }
  const char *operator +=(const char *addval) {
    return cat(addval);
  }
  const char *operator +=(char value);
  const char *operator +=(int value);
  const char *operator +=(GUID guid);

  // copy assignment operator
  String &operator =(const String &s) {
    if (this != &s)
      setValue(s);
    return *this;
  }

  // comparator operators
  inline int operator ==(const char *val) const {
    if(!val) return isempty();
    return isequal(val);
  }
  inline int operator <(const char *val) const {
    return islessthan(val);
  }
  inline int operator !=(const char *val) const {
    if(!val) return !isempty();
    return !isequal(val);
  }
  inline int operator >(const char *val) const {
    return (!islessthan(val)) && (!isequal(val));
  }
  inline String operator +(const char *val) {
    String retval = *this;
    return retval += val;
  }
  inline int operator ==(const String &val) const {
    return isequal(val);
  }
  inline int operator <(const String &val) const {
    return islessthan(val);
  }
  inline int operator !=(const String &val) const {
    return !isequal(val);
  }
  inline int operator >(const String &val) const {
    return (!islessthan(val)) && (!isequal(val));
  }
  inline String operator +(const String &val) {
    String retval = *this;
    return retval += val;
  }
  inline String operator +(const char val) {
    String retval = *this;
    return retval += val;
  }

/**
  Gets the length of the string's value. Note that a 0 length can result from
  both a value of NULL and a value of "".
  @ret The length of the string's value;
*/
  int len() const;
/**
  Returns TRUE if the string's value is either NULL or "".
  @ret TRUE if the string's value is either NULL or "", FALSE otherwise.
*/
  int isempty() const;
  int notempty() const { return !isempty(); }	// more readable sometimes

  bool istrue() const;
/**
  Converts entire string to uppercase. Not multibyte UTF-8 safe yet.
  @see tolower()
*/
  void toupper();
/**
  Converts entire string to lowercase. Not multibyte UTF-8 safe yet.
  @see tolower()
*/
  void tolower();
/**
  Checks string value equality against a nul-terminated char *.
  @param otherval The value to check against. If NULL, will be treated as "".
  @ret TRUE if the string matches exactly, FALSE otherwise.
  @see iscaseequal()
  @see islessthan()
*/
  int isequal(const char *otherval) const;  // basically !strcmp
/**
  Checks string value equality against a nul-terminated char *, case insensitively. I.e. "Blah" is case equal to "bLaH".
  @param otherval The value to check against. If NULL, will be treated as "".
  @ret TRUE if the string matches case-insensitively, FALSE otherwise.
  @see isequal()
  @see islessthan()
*/
  int iscaseequal(const char *otherval) const;  // basically !strcasecmp
  int islessthan(const char *otherval) const;  // basically strcmp < 0

  int wordcount() const;	// uses ISSPACE and ISALPHA

/**
  Changes all instances of a character to another character throughout the
  string. Not multibyte UTF-8 aware yet. Note you can use a 'to' of NULL,
  but this practice is not encouraged: try to use trunc() or truncateOnChar() instead.
  @param from The character value to modify.
  @param to The character value to replace with.
  @see trunc()
  @see truncateOnChar()
*/
  void changeChar(int from, int to);
/**
  Truncates the string value at the first given character value found. If fromright==TRUE, searches from the right, otherwise goes left-to-right. Not UTF-8 multibyte aware yet.
  Ex:
    String x("abcd");
    x.truncateOnChar('c');
    x now contains "ab"
  @see changeChar()
*/
  void truncateOnChar(int which, int fromright = FALSE);

  int firstChar() const; // -1 if empty
/**
  Gets the last character value (rightmost).
  @see getChar()
  @ret The rightmost character value, or -1 if string is empty.
*/
  int lastChar() const;  // -1 if empty

/**
  Executes a standard printf type call and sets the string's value to it.
  @ret The new value of the string.
  @param format The formatting string to use.
*/
  const char *printf(const char *format,  ...);
/**
  Concatenates the given value onto the end of the string. NULL will be
  treated as "".
  @param value The value to concatenate.
  @ret The new value of the string.
  @see catn()
  @see prepend()
*/
  const char *cat(const char *value);
/**
  Concatenates a certain number of characters from the given value onto the end of the string. NULL will be treated as "".
  @param value The value to concatenate.
  @param len How many characters of value to use.
  @ret The new value of the string.
  @see cat()
  @see prepend()
*/
  const char *catn(const char *value, int len);
/**
  Inserts the given string value at the beginning of the string. NULL will be
  treated as "".
  @param value The value to insert.
  @ret The new value of the string.
  @see cat()
  @see catn()
*/
  const char *prepend(const char *value);
  // replaces string with n chars of val or length of val, whichever is less.
  const char *ncpy(const char *newstr, int numchars);

/**
  Copies up to maxlen chars from the string into the destination. Differs from
  STRNCPY in that it makes sure the destination is always nul-terminated, so
  note that maxlen includes the terminating nul.
  @param dest The destination to copy to.
  @param maxlen How many bytes, at most, to copy.
*/
  void strncpyTo(char *dest, int maxlen) const;

  // -----------------------------------------
  // Character based find-n-splice methods --
  // "l" and "r" prefixes specify to begin at
  // front or back of string:

  // Returns index of first found, -1 if not found.
  int lFindChar(char findval) const;
  int lFindChar(const char *findval) const;  // a list of chars to search for
  int rFindChar(char findval) const;
  int rFindChar(const char *findval) const;  // a list of chars to search for

  // Splits string at findval.  Characters passed by search, including the
  // found character, are MOVED to the returned string.  If there is no char
  // to be found, the entire string is returned and the called instance is
  // left empty.  (Makes looped splits very easy).
  String lSplit(int idxval);
  String lSplitChar(char findval);
  String lSplitChar(const char *findval);
  String rSplit(int idxval);
  String rSplitChar(char findval);
  String rSplitChar(const char *findval);

  // Same as split, except the find char is cut completely.
  String lSliceChar(char findval);
  String lSliceChar(const char *findval);
  String rSliceChar(char findval);
  String rSliceChar(const char *findval);

  // these names are deprecated
  #define lSpliceChar lSliceChar
  #define rSpliceChar rSliceChar

/**
  Replaces all occurences of the value specified by 'find' with the value
  specified by 'replace'.
  @param find The value to find.
  @param replace The value to replace with.
  @ret The number of replacements that were executed.
*/
  int replace(const char *find, const char *replace);

/**
  Replaces fields of same character with 0-padded text representation of an int.
  Example: blah$$$$.png becomes blah0000.png
*/
  int replaceNumericField(int value, int fieldchar = '\x24');

  // UTF8-Aware "Character Based" Methods
/**
  Returns how many characters are in the string value. Same as len(), but multibyte UTF-8 aware.
  @see len()
  @ret Number of logical UTF-8 character in the string.
*/
  int numCharacters();

/**
  Truncates the length of the string to newlen. If newlen is negative, trims
  -newlen characters from the end. Multibyte UTF-8 aware.
  Note: do NOT do trunc(len()-1), as trunc is UTF8 aware, but len() is not, so 
  the call may actually not remove a character (possibly resulting in a lockup if you
  do that in a while loop). use trunc(numCharacters()-1) !
  @param newlen The new length of the string. If the string is shorter than this, nothing happens. If this value is negative, then the absolute value of newlen is how many characters to trim from the right. I.e. -1 means trim off one character from the end.
  @see truncateOnChar()
*/
  void trunc(int newlen);

  int trim(const char *whitespace=" \t\r\n", int left=TRUE, int right=TRUE);

/**
  Does a vsprintf. Used the same way as printf(), but with a va_list instead of "...".
  @param format The format string to use.
  @param args The argument list in va_list format.
  @ret The number of characters in the final string.
*/
  int vsprintf(const char *format, va_list args);

/**
  Ensures that the string drops any memory it might have allocated.
*/
  void purge();

// just like php's substr, except slen must be >= 0
  String substr(int from, int slen=0x7fffffff);

protected:
  char * val;
  enum { wastage_allowed = 128 };
};

/**
  String class with a printf-style constructor. Otherwise identical to String.
  Also takes some standard types, like int, double, and GUID.
  @see GUID
  @see String
*/
class StringPrintf : public String {
public:
  StringPrintf(const char *format=NULL, ...);
  StringPrintf(int value);
  StringPrintf(bool value);	// false="0", true="1"
  StringPrintf(double value);
  StringPrintf(const GUID &g);
  StringPrintf(const RECT &r);
};

class StringCat : public String {
public:
  StringCat(const char *val1, const char *val2);
  StringCat(const char *val1, const char *val2, const char *val3,
            const char *val4=NULL, const char *val5=NULL, const char *val6=NULL,
            const char *val7=NULL);
private:
  void constructVals(const char *val1, const char *val2, const char *val3=NULL,
            const char *val4=NULL, const char *val5=NULL, const char *val6=NULL,
            const char *val7=NULL);
};

class StringInt : public String {
public:
  StringInt(int v);
};

class StringStrftime : public String {
public:
  StringStrftime(time_t when=0, const char *format="%#c", bool local=true);
};

class StringToLower : public String {
public:
  StringToLower(const char *val=NULL) : String(val) {
    tolower();
  }
};
class StringToUpper : public String {
public:
  StringToUpper(const char *val=NULL) : String(val) {
    toupper();
  }
};

#if defined(NDEBUG) && defined(WASABI_NO_RELEASEMODE_DEBUGSTRINGS)
#define DebugString 0
#else
#define DebugString _DebugString
#endif

class _DebugString : public String {
public:
  _DebugString(const char *format=NULL, ...);
  _DebugString(const String &s);
  _DebugString(const String *s);
  _DebugString(const RECT &r);

  void debugPrint();
  static void outputdebugstring(const char *text);
#if !(defined(NDEBUG) && defined(WASABI_NO_RELEASEMODE_DEBUGSTRINGS))
  static String outputdebugfilename;
#endif
};

#define RecycleString String

// String operators using StringPrintf

inline const char *String::operator +=(char value)
{
  return cat(StringPrintf("%c",value));  // Uhm.  Shouldn't the string be given
                                         // the "Fast" methods and the Printf be
                                         // built off of that?
}

inline const char *String::operator +=(int value)
{
  return cat(StringPrintf("%i",value));
}

inline const char *String::operator +=(GUID guid) {
  return cat(StringPrintf(guid));
}

//
//  Global operator overrides to allow string to take over for
//  the use of standard operators with const char pointers as
//  left hand operands.
inline int operator ==(const char *v1, const String &v2) {
  return v2.isequal(v1);
}
inline int operator !=(const char *v1, const String &v2) {
  return !v2.isequal(v1);
}
inline int operator <(const char *v1, const String &v2) {
  return !v2.islessthan(v1);
}
inline int operator >(const char *v1, const String &v2) {
  return v2.islessthan(v1);
}


/**
  Compares two strings. Generally used with PtrListSorted<>
  @see PtrListSorted
*/
class StringComparator {
public:
  // comparator for sorting
  static int compareItem(String *p1, String* p2);
  // comparator for searching
  static int compareAttrib(const char *attrib, String *item);
};

// same as StringComparator, but searches with a String instead of
// a const char *, for use in Maps that have String as index.
class StringComparator2 {
public:
  // comparator for sorting
  static int compareItem(String *p1, String* p2);
  // comparator for searching
  static int compareAttrib(const char *attrib, String *item);
};

#endif // __cplusplus

#endif
