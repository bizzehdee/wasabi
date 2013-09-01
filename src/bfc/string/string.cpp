#include <precomp.h>
#include <bfc/std.h>
#include "string.h"
#include <bfc/guid.h>

#define VSPRINTF ::vsprintf	//FUCKO move to std_string.h

#if !(defined(NDEBUG) && defined(WASABI_NO_RELEASEMODE_DEBUGSTRINGS))
String _DebugString::outputdebugfilename;
#endif

String::String(const char *initial_val) :
  val(NULL)
{
  setValue(initial_val);
}

String::String(const String &s) :
  val(NULL)
{
  setValue(s.getValue());
}

String::String(const String *s) :
  val(NULL)
{
  if (s == NULL) setValue(NULL);
  else if (s != this) setValue(s->getValue());
}

String::~String() {
  FREE(val);
}

const char *String::getValueSafe(const char *def_val) const {
  if (val == NULL)
    return def_val;
  else
    return val;
}

int String::getChar(int pos, int bounds_check) const {
  if (pos < 0 || val == NULL) return -1;
  if (bounds_check && pos >= len()) return -1;
  return val[pos];
}

int String::setChar(int pos, int value) {
  if (pos < 0 || val == NULL) return -1;
  return val[pos] = value;
}

const char *String::setValue(const char *newval) {
  if (newval != val)  {
    if (newval == NULL) {
      FREE(val);
      val = NULL;
    } else {
      int len = STRLEN(newval);
      if (val != NULL)
#ifdef STRING_REALLOC_OPTIMS 
      {
        int oldlen = STRLEN(val);
        // if smaller but greater than half previous size, don't realloc
        if (len > oldlen || len < oldlen/2)
          val = (char *)REALLOC(val, len+1);
      }
#else
        val = (char *)REALLOC(val, len+1);
#endif
      else
        val = (char *)MALLOC(len+1);
      //CUT ASSERT(newval != NULL); // WaI> already "asserted" above.
      MEMMOVE(val, newval, len+1); 
    }
  }
  return getValue();
}

int String::len() const {
  return (val == NULL) ? 0 : STRLEN(val);
}

int String::isempty() const {
  return (!val || !*val);
}

bool String::istrue() const {
  return !!ATOI(v());
}

void String::toupper() {
  if (!isempty()) for (char *p = val; *p; p++) *p = TOUPPER(*p);
}

void String::tolower() {
  if (!isempty()) for (char *p = val; *p; p++) *p = TOLOWER(*p);
}

int String::isequal(const char *otherval) const {
  return !STRCMPSAFE(getValue(), otherval);
}

int String::iscaseequal(const char *otherval) const {
  return !STRICMPSAFE(getValue(), otherval);
}

int String::islessthan(const char *otherval) const {
  return STRCMPSAFE(getValue(), otherval) < 0;
}

int String::wordcount() const {
  if (isempty()) return 0;

  String copy(this);
  for (char *p = copy.ncv(); *p; p++) {
    int issp = ISSPACE(*p);
    int isal = ISALPHA(*p);
    if (issp || isal) continue;
    STRCPY(p, p+1);
    p--;
  }

  copy.trim();

  int ret = 0;

  int lastwasspace = 0;
  int l = copy.len();
  for (int i = 0; i < l; i++) {
    int v = copy.v()[i];
    ASSERT(v != 0);

    int issp = ISSPACE(v);
    int isal = ISALPHA(v);
    if (!issp && !isal) continue;	// crap

    if (issp) {
      if (lastwasspace) continue;
      ret++;
      lastwasspace = 1;
    } else {	// is alpha
      if (lastwasspace) ret ++;
      lastwasspace = 0;
    }
  }

  ret /= 2;	// 2 edges = 1 word
  ret ++;	// first word

//DebugString("wordcount: %d for '%s'", ret, vs());

  return ret;
}

void String::changeChar(int from, int to) {
  if (val == NULL) return;
  int length = len();
  for (int i = 0; i < length; i++)
    if (val[i] == from) val[i] = to;
}

void String::truncateOnChar(int which, int fromright) {
  if (fromright) {
    if (val == NULL) return;
    int length = len();
    for (int i = length-1; i >= 0; i--) {
      if (val[i] == which) {
        val[i] = '\0';
        return;
      }
    }
  } else {
    changeChar(which, '\0');
  }
}

int String::firstChar() const {
  if (isempty()) return -1;
  return val[0];
}

int String::lastChar() const {
  if (isempty()) return -1;
  return val[len()-1];
}

const char *String::printf(const char *format,  ...) {
  va_list args;
  va_start (args, format);
  vsprintf(format,args);
  va_end(args);
  return getValue();
}

const char *String::cat(const char *value) {
  if (value == NULL || *value == 0) return getValue();
  if (val == NULL) return setValue(value);
#if 0
//this is insane
  return setValue(StringPrintf("%s%s", getValue(), value));
#else
  return catn(value, STRLEN(value));
#endif
}

const char *String::catn(const char *value, int len) {
  if (len == 0) return val;
  if (value == NULL || *value == 0) return getValue();
  if (val == NULL) return ncpy(value,len);
  int ol=STRLEN(val);
  val=(char *)REALLOC(val, ol+len+1);
  val[ol+len]=0;
  STRNCPY(val+ol,value,len);
  return val;
}

const char *String::prepend(const char *value) {
  if (value == NULL || *value == 0) return getValue();
  if (val == NULL) return setValue(value);
  return setValue(StringPrintf("%s%s", value, getValue()));
}

// replaces string with n chars of val or length of val, whichever is less.
const char *String::ncpy(const char *newstr, int numchars) {
  val = (char *)REALLOC(val, numchars+1);
  val[numchars] = 0;
  STRNCPY(val,newstr,numchars);
  return getValue();
}

void String::strncpyTo(char *dest, int maxlen) const {
  ASSERT(dest != NULL);
  ASSERT(maxlen >= 0);
  if (maxlen == 0) return;
  else if (maxlen == 1) {
    *dest = '\0';
    return;
  }
  const char *src = val;
  if (src == NULL) src="";
  int copylen = MIN(STRLEN(src), maxlen);
  if (maxlen == copylen) copylen--;	// make room for 0-termination
  MEMMOVE(dest, src, copylen);
  dest[copylen] = 0;
}


// -----------------------------------------
// Character based find-n-splice methods --
// "l" and "r" prefixes specify to begin at
// front or back of string:

#ifdef FAST_METHODS
#undef FAST_METHODS
#endif//FAST_METHODS
#ifdef SAFE_METHODS
#undef SAFE_METHODS
#endif//SAFE_METHODS
#ifdef USE
#undef USE
#endif//USE

#define FAST_METHODS 1
#define SAFE_METHODS 0

#define USE FAST_METHODS

// Returns index of first found, -1 if not found.
int String::lFindChar(char findval) const {
  int length = len();
  for (int i = 0; i < length; i++) {
    if (val[i] == findval) {
      return i;
    }
  }
  return -1;
}

// Same as above, save the "findval" is a string where it searches
// for any of the characters in the string.
int String::lFindChar(const char *findval) const {
  int length = len();
  int numchars = STRLEN(findval);
  for (int i = 0; i < length; i++) {
    for (int j = 0; j < numchars; j++) {
      if (val[i] == findval[j]) {
        return i;
      }
    }
  }
  return -1;
}

int String::rFindChar(char findval) const {
  int length = len();
  for (int i = length-1; i > 0; i--) {
    if (val[i] == findval) {
      return i;
    }
  }
  return -1;
}

// Same as above, save the "findval" is a string where it searches
// for any of the characters in the string.
int String::rFindChar(const char *findval) const {
  int length = len();
  int numchars = STRLEN(findval);
  for (int i = length-1; i > 0; i--) {
    for (int j = 0; j < numchars; j++) {
      if (val[i] == findval[j]) {
        return i;
      }
    }
  }
  return -1;
}

// Splits string at findval.  Characters passed by search, including the 
// found character, are MOVED to the returned string.  If there is no char
// to be found, the entire string is returnef and the called instance is
// left empty.  (Makes looped splits very easy).
String String::lSplit(int idxval) {
  if (val == NULL) return String();
  if (idxval == -1) { // Not Found
    // Copy our contents to return on the stack
    String retval(val);
    // And zero the string.
    if (val) {
      val[0] = 0;
    }
    return retval;
  } else {
    String retval;
    // Copy into retval the number of characters to the found char index.
    retval.ncpy(val,idxval+1);
#if USE == FAST_METHODS
    int len = STRLEN(val+idxval+1);
    MEMMOVE(val,val+idxval+1,len+1);
#elif USE == SAFE_METHODS
    // Copy from the found index downwards to save for this object
    String temp(val + idxval + 1);
    // And then copy into ourselves the tempspace.
    *this = temp;
#endif
    return retval;
  }
  // this will never be hit.  many compilers are too stupid to realize this.
  return String();
}

String String::lSplitChar(char findval) {
  if (val == NULL) return String();
  // The index of the found character
  int idxval = lFindChar(findval);
  return lSplit(idxval);
}

String String::lSplitChar(const char *findval) {
  if (val == NULL) return String();
  // The index of the found character
  int idxval = lFindChar(findval);
  return lSplit(idxval);
}

String String::rSplit(int idxval) {
  if (val == NULL) return String();
  if (idxval == -1) { // Not Found
    // Copy our contents to return on the stack
    String retval(val);
    // And zero the string.
    val[0] = 0;
    return retval;
  } else {
    // Copy from the found index downwards to the retval
    String retval(val + idxval);
    // Terminate the found char index
    val[idxval] = 0;
    // That was easier, wasn't it?
    return retval;
  }
  // this will never be hit.  many compilers are too stupid to realize this.
  return String();
}

String String::rSplitChar(char findval) {
  if (val == NULL) return String();
  // The index of the found character
  int idxval = rFindChar(findval);
  return rSplit(idxval);
}

String String::rSplitChar(const char *findval) {
  if (val == NULL) return String();
  // The index of the found character
  int idxval = rFindChar(findval);
  return rSplit(idxval);
}

// Same as split, except the find char is cut completely.
String String::lSliceChar(char findval) {
  if (val == NULL) return String();
//CUT  // Auto-scope reference allows us to avoid a copy.
//CUT  String & retval = lSplitChar(findval);
//BU gcc doesn't agree with you and neither do I :/
  String retval = lSplitChar(findval);
  
  // We need to strip the findval char, which is the end char.
  int end = retval.len();
  if (end) {
    if (retval.val[end-1] == findval) {
      retval.val[end-1] = 0;
    }
  }
  return retval;
}

// Same as split, except the find char is cut completely.
String String::lSliceChar(const char *findval) {
  if (val == NULL) return String();
//CUT  // Auto-scope reference allows us to avoid a copy.
//CUT  String & retval = lSplitChar(findval);
//BU gcc doesn't agree with you and neither do I :/
  String retval = lSplitChar(findval);
  // We need to strip the findval char, which is the end char.
  int end = retval.len();
  int num = STRLEN(findval);
  if (end) {
    for (int i = 0; i < num; i++) {
      if (retval.val[end-1] == findval[i]) {
        retval.val[end-1] = 0;
      }
    }
  }
  return retval;
}

String String::rSliceChar(char findval) {
  if (val == NULL) return String();
//CUT  // Auto-scope reference allows us to avoid a copy.
//CUT  String & retval = rSplitChar(findval);
//BU gcc doesn't agree with you and neither do I :/
  String retval = rSplitChar(findval);
  // We need to strip the findval char, which is the first char.
  // (But we still check for empty string:)
  int end = retval.len();
  if (end) {
    if (retval.val[0] == findval) {
#if USE == FAST_METHODS
      int len = STRLEN(retval.val+1);
      MEMMOVE(retval.val,retval.val+1,len+1);
#elif USE == SAFE_METHODS
      String temp(retval.val + 1);
      retval = temp;
#endif
      return retval;
    }
  }
  return retval;
}

String String::rSliceChar(const char *findval) {
  if (val == NULL) return String();
//CUT  // Auto-scope reference allows us to avoid a copy.
//CUT  String & retval = rSplitChar(findval);
//BU gcc doesn't agree with you and neither do I :/
  String retval = rSplitChar(findval);
  // We need to strip the findval char, which is the first char.
  // (But we still check for empty string:)
  int end = retval.len();
  int num = STRLEN(findval);
  if (end) {
    for (int i = 0; i < num; i++) {
      if (retval.val[0] == findval[i]) {
#if USE == FAST_METHODS
      int len = STRLEN(retval.val+1);
      MEMMOVE(retval.val,retval.val+1,len+1);
#elif USE == SAFE_METHODS
        String temp(retval.val + 1);
        retval = temp;
#endif
        return retval;
      }
    }
  }
  return retval;
}

int String::replace(const char *find, const char *replace) {
  if (len() == 0 || find == NULL || replace == NULL) return 0;

  int find_count = 0;

  char *p, *p2;
  int rep_len = STRLEN( replace );
  int find_len = STRLEN( find );
  int size_diff = rep_len - find_len;

  if ( size_diff == 0 ) {
    p = val;
    while( (p = STRSTR( p, find  )) ) {
      STRNCPY( p, replace, rep_len );
      p += find_len;
      find_count++;
    }
  } else {

    char *new_buf, *in;

    p = val;
    while( (p = STRSTR( p, find )) != NULL ) {
      find_count++;
      p += find_len;
    }

    new_buf = (char *)MALLOC( len() + find_count * size_diff + 1 );

    p = val;
    in = new_buf;
    while( (p2 = STRSTR( p, find )) ) {
      STRNCPY( in, p, (int)(p2 - p) );
      in += p2 - p;
      STRNCPY( in, replace, (int)rep_len ); 
      in += rep_len;
      p = p2 + find_len;
    }
    STRCPY( in, p );
    new_buf[ len() + find_count * size_diff ] = 0;

    setValue( new_buf );
    FREE( new_buf );
  }
  return find_count;
}

int String::replaceNumericField(int value, int fieldchar) {
  if (val == NULL || *val == '\0') return 0;
  int nrep = 0;
  for (const char *p = val; *p; p++) {
    if (*p == fieldchar) nrep++;
    else if (nrep) break;
  }
  if (nrep == 0) return 0;	// no field found
  String rc;
  char fc[2] = { 0, 0 };
  fc[0] = fieldchar;
  for (int i = 0; i < nrep; i++) rc.cat(fc);
  StringPrintf fmt("%%0%0dd", nrep);
  StringPrintf replacement(fmt.getValue(), value);
  return replace(rc, replacement);
}

#undef USE
#undef SAFE_METHODS
#undef FAST_METHODS


int String::numCharacters() {
  if (isempty()) return 0;
  // count newsize characters over how many bytes?
  int count, bytes;
  for (bytes = 0, count = 0; val[bytes]; count++, bytes++) {
    // If we encounter a lead byte, skip over the trail bytes.
    switch (val[bytes] & 0xC0) {
      case 0x80: // trail bytes
        // THIS SHOULD NEVER EVER EVER EVER HAPPEN!
        // but we'll fall through anyhow, just in case someone
        // sends us non-UTF8.
      case 0xC0: // lead bytes
        do {
          bytes++;
          if (val[bytes] == 0) {
            // if people are giving us lame encodings, break here.
            break;
          }
        } while ((val[bytes+1] & 0xC0) == 0x80);
      break;
    }
  }
  return count;
}

void String::trunc(int newlen) {
  if (val == NULL) return;
  int oldlen = numCharacters();
  if (newlen < 0) newlen = MAX(oldlen + newlen, 0);
  if (newlen >= oldlen) return;

  // count newsize characters over how many bytes?
  int count, bytes;
  for (bytes = 0, count = 0; count < newlen; count++, bytes++) {
    // If we encounter a lead byte, skip over the trail bytes.
    switch (val[bytes] & 0xC0) {
      case 0x80: // trail bytes
        // THIS SHOULD NEVER EVER EVER EVER HAPPEN!
        // but we'll fall through anyhow, just in case someone
        // sends us non-UTF8.
      case 0xC0: // lead bytes
        do {
          bytes++;
        } while ((val[bytes+1] & 0xC0) == 0x80);
      break;
    }
  }

  val[bytes] = 0;
}

int String::trim(const char *whitespace, int left, int right) {
  if (val == NULL) return 0;
  int ret = 0;
  if (left) {	// trim left
    for (;;) {
      int restart = 0;
      const char *p;
      for (p = whitespace; *p; p++) {
        if (*p == val[0]) {
          STRCPY(val, val+1);
          restart = 1;
          ret = 1;
          break;
        }
      }
      if (!restart) break;
    }
  }//left
  if (right) {
    char *ptr = val;
    while (*ptr) ptr++;
    ptr--;
    if (ptr <= val) return ret;
    for (; ptr > val; ptr--) {
      const char *p;
      for (p = whitespace; *p; p++) {
        if (*p == *ptr) {
          *ptr = '\0';
          ret = 1;
          break;
        }
      }
      if (!*p) break;
    }
  }//right
  return ret;
}

int String::vsprintf(const char *format, va_list args) {
  if(!format) return 0;

  va_list saveargs;
  va_copy(saveargs,args);

  // roughly evaluate size of dest string
  const char *p=format;
  int length=0;
  while(*p) {
    if(*(p++)!='%') length++;
    else {
      for (;;) {
        const char f=*p++;
        if(f=='c') { length++; char c = va_arg(args, int); } //chars are pushed as ints for compatibility with C
        else if(f=='i') { length+=16; int i = va_arg(args, int); }
        else if(f=='u') { length+=16; unsigned int u = va_arg(args, unsigned int); }
        else if(f=='f') { length += 64; double f = va_arg(args, double); }
        else if(f=='e') { length += 64; double e = va_arg(args, double); }
        else if(f=='g') { length += 64; double g = va_arg(args, double); }
        else if(f=='d') { length+=64; int d = va_arg(args, int); }
        else if(f=='x') { length+=32; int x = va_arg(args, int); } // Hex with LC Alphas: 0x0009a64c
        else if(f=='X') { length+=32; int X = va_arg(args, int); } // Hex with UC Alphas: 0x0009A64C
        else if(f=='s') { // ::vsprintf can properly handle null.
          char *str = va_arg(args, char *); 
          if (str == NULL) {
            length+=STRLEN("(null)"); // Just to be explicit.
          } else {
            length+=STRLEN(str);
          }
        }
        else if(ISDIGIT(f)) continue;
        else if(f == '.') continue;
        else if(f=='%') length++;
        else ASSERTPR(0,"undefined format passed to stringprintf!");
        break;
      }
    }
  }
  if(val) {
    if(len() < length) 
      val=(char *)REALLOC(val,length+1);
  } else val=(char *)MALLOC(length+1);

  // now write the string in val
  int real_len = VSPRINTF(val,format,saveargs);
  va_end(saveargs);

  ASSERTPR(real_len <= length, "String.printf overflow");
  return real_len;
}

void String::purge() {
  FREE(val);
  val = NULL;
}

String String::substr(int from, int slen) {
  if (isempty()) return "";
  ASSERTPR(slen >= 0, "negative slen not implemented yet, sorry");
  const char *fromptr;
  int mylen = len();
  if (from < 0) {
    fromptr = val + mylen + from;	// from is negative
  } else {
    fromptr = val + from;
  }
  if (fromptr < val || fromptr > val+mylen) return "";	// out of bounds
  int newlen = MIN(STRLEN(fromptr), slen);
  String ret;
  ret.catn(fromptr, newlen);
  return ret;
}

StringPrintf::StringPrintf(const char *format, ...) {
  va_list args;
  va_start (args, format);
  vsprintf(format,args);
  va_end(args);
}

StringPrintf::StringPrintf(int value) {
  *this += value;
}

StringPrintf::StringPrintf(bool value) {
  *this += (int)value;
}

StringPrintf::StringPrintf(double value) {
  *this += StringPrintf("%f", value);
}

StringPrintf::StringPrintf(const GUID &g) {
  Guid glup(g);
  cat(glup.toChar().vs());
}

StringPrintf::StringPrintf(const RECT &r) {
  printf("%d,%d,%d,%d", r.left, r.top, r.right, r.bottom);
}

StringCat::StringCat(const char *val1, const char *val2) {
  constructVals(val1, val2);
}

StringCat::StringCat(const char *val1, const char *val2, const char *val3,
            const char *val4, const char *val5, const char *val6, const char *val7) {
  constructVals(val1, val2, val3, val4, val5, val6, val7);
}

void StringCat::constructVals(const char *val1, const char *val2, const char *val3,
            const char *val4, const char *val5, const char *val6, const char *val7) {
  if (val1 == NULL) val1="";
  if (val2 == NULL) val2="";
  if (val3 == NULL) val3="";
  if (val4 == NULL) val4="";
  if (val5 == NULL) val5="";
  if (val6 == NULL) val6="";
  if (val7 == NULL) val7="";

  int len1 = STRLEN(val1);
  int len2 = STRLEN(val2);
  int len3 = STRLEN(val3);
  int len4 = STRLEN(val4);
  int len5 = STRLEN(val5);
  int len6 = STRLEN(val6);
  int len7 = STRLEN(val7);

  int total_len = len1+len2+len3+len4+len5+len6+len7+1;

  val = (char*)MALLOC(total_len);
  char *pt = val;

  MEMCPY(pt, val1, len1);
  pt += len1;
  MEMCPY(pt, val2, len2);
  pt += len2;
  MEMCPY(pt, val3, len3);
  pt += len3;
  MEMCPY(pt, val4, len4);
  pt += len4;
  MEMCPY(pt, val5, len5);
  pt += len5;
  MEMCPY(pt, val6, len6);
  pt += len6;
  MEMCPY(pt, val7, len7);
  pt += len7;
  *pt = '\0';
  ASSERT(pt == val + total_len - 1);
}

StringInt::StringInt(int v) {
  setValue(StringPrintf("%d", v));
}

#include <time.h>

StringStrftime::StringStrftime(time_t when, const char *format, bool local) {
  if (when == 0) {
    when = time(NULL);
  }
  char buf[1024]="";
  strftime(buf, 1024, format, local ? localtime(&when):gmtime(&when));
  setValue(buf);
}

int debug_string_enabled=1;

_DebugString::_DebugString(const char *format, ...) {
  va_list args;
  va_start (args, format);
  vsprintf(format,args);
  va_end(args);

  debugPrint();
}

_DebugString::_DebugString(const String &s) : String(s) {
  debugPrint();
}

_DebugString::_DebugString(const String *s) : String(s) {
  debugPrint();
}

_DebugString::_DebugString(const RECT &r) {
  printf("%d,%d,%d,%d", r.left, r.top, r.right, r.bottom);
  debugPrint();
}

void _DebugString::debugPrint() {
  if (!debug_string_enabled) return;
  outputdebugstring(getValue());
}

void _DebugString::outputdebugstring(const char *text) {
  if (text == NULL || *text == '\0') return;
  OutputDebugString(text);
  const char *pt = text; 
  for (; *pt; pt++) { }
  if (pt[-1] != '\n') OutputDebugString("\n");

#if !(defined(NDEBUG) && defined(WASABI_NO_RELEASEMODE_DEBUGSTRINGS))
  if (!_DebugString::outputdebugfilename.isempty()) {
    StringCat filename(outputdebugfilename, ".", StringStrftime(0, "%Y-%m-%d"), ".txt");
    FILE *fp = fopen(filename.vs(), "at");
    if (fp != NULL) {
      fprintf(fp, "%s", text);
      const char *pt = text; 
      for (; *pt; pt++) { }
      if (pt[-1] != '\n') fprintf(fp, "\n");
      fclose(fp);
    }
  }
#endif
}

int StringComparator::compareItem(String *p1, String* p2) {
  return STRCMP(p1->getValue(), p2->getValue());
}

int StringComparator::compareAttrib(const char *attrib, String *item) {
  return STRCMP(attrib, item->getValue());
}

int StringComparator2::compareItem(String *p1, String* p2) {
  return STRCMP(p1->getValue(), p2->getValue());
}

int StringComparator2::compareAttrib(const char *attrib, String *item) {
  return STRCMP(((String *)attrib)->getValue(), item->getValue());
}
