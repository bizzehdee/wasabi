#include <precomp.h>

#include "std_file.h"
#include <bfc/file/readdir.h>

#ifdef WIN32
#include <shellapi.h>	// for ShellExecute
#endif

#ifndef TMPNAME_PREFIX
#define TMPNAME_PREFIX "WT"
#endif

#if UTF8
#ifdef WANT_UTF8_WARNINGS
#pragma CHAT("mig", "all", "UTF8 is enabled in std_file.cpp -- Things might be screwy till it's all debugged?")
#endif
# include <bfc/string/encodedstr.h>
#endif

#ifndef _NOSTUDIO

#include <parse/pathparse.h>

//for no filereaders, #define WASABI_STD_FILE_WANT_FILEREADERS 0
#define WASABI_STD_FILE_WANT_FILEREADERS 0
#ifndef WASABI_STD_FILE_WANT_FILEREADERS
#define WASABI_STD_FILE_WANT_FILEREADERS defined(WASABI_COMPILE_SVC)
#endif

#if WASABI_STD_FILE_WANT_FILEREADERS
#ifndef WASABI_COMPILE_SVC
#error filereaders require service API (add WASABI_COMPILE_SVC to your wasabicfg.h)
#endif

#include <api/service/svcs/svc_fileread.h>
static PtrList<svc_fileReader> fileReaders;

#endif // want filereaders

#ifdef WASABI_PLATFORM_POSIX
FILE *FOPEN_NOCASE( const char *file, const char *mode ) {
  FILE *ret;

  PathParser pp(file);
  String proper;
  for (int i=0;i<pp.getNumStrings();i++) {
    if (i > 0) proper += DIRCHARSTR;
    proper += pp.enumString(i);
  }
  const char *filename = proper.getValue();

  ret = fopen( filename, mode );

  if ( ! ret ) {
    // Try to mix the case up...
    String infile;
    String curdir;
    if ( filename[0] == DIRCHAR ) {
      infile = filename + 1;
      curdir = "/";
    } else {
      infile = filename;
      curdir = ".";
    }
    char *dir = strtok( (char *)infile.getValue(), DIRCHARSTR );
    int found = 1;

    while ( found && dir && curdir != "" ) {
      found = 0;

      char *next = strtok( NULL, DIRCHARSTR );
      ReadDir rd( curdir );

      while( rd.next() ) {
        if ( ! STRICMP( rd.getFilename(), dir ) ) {
          if ( next && ! rd.isDir() ) {
            curdir = "";
          } else {
            curdir += DIRCHARSTR;
            curdir += rd.getFilename();
          }

          found = 1;
          break;
        }
      }

      dir = next;
    }

    if ( found )
      ret = fopen( curdir, mode );
  }

  return ret;
}
#endif

FILE *FOPEN(const char *file,const char *mode)
{
PathParser pp(file);
String proper;
for (int i=0;i<pp.getNumStrings();i++) {
  if (i > 0) proper += DIRCHARSTR;
  proper += pp.enumString(i);
}
const char *filename = proper.getValue();
#if !WASABI_STD_FILE_WANT_FILEREADERS
  return fopen(filename, mode);
#else
  FILE *ret = NULL;

#if UTF8
  if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
    // CODE FOR NT COMPATIBLE OS'S
    EncodedStr fname16enc;
    int retcode = fname16enc.convertFromUTF8(SvcStrCnv::UTF16, String(filename));
    wchar_t *fname16 = static_cast<wchar_t *>(fname16enc.getEncodedBuffer());
    EncodedStr mode16enc;
    mode16enc.convertFromUTF8(SvcStrCnv::UTF16, String(mode));
    wchar_t *mode16 = static_cast<wchar_t *>(mode16enc.getEncodedBuffer());
    if (fname16 != NULL) {
      ret = _wfopen(fname16,mode16);
    } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
      // If we fail conversion, the service might not be available yet.
      // so we'll try opening the file and pray that it's ascii7.
      ret=fopen(filename,mode);
    }
  } else {
    // CODE FOR 9x COMPATIBLE OS'S
    EncodedStr fnameOSenc;
    int retcode = fnameOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(filename));
    char *fnameOS = static_cast<char *>(fnameOSenc.getEncodedBuffer());
    if (fnameOS != NULL) {
      ret=fopen(fnameOS,mode);
    } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
      // If we fail conversion, conversion might not be available yet.
      // so we'll try opening the file and pray that it's ascii7.
      ret=fopen(filename,mode);
    }
  }
#else
  ret=fopen(filename,mode);
#endif

  if (ret) return ret;

  // File not found... try to open it with the file readers

  if (WASABI_API_SVC) {
    int m = 0;
    if(STRCHR(mode, 'r')) m |= SvcFileReader::READ;
    if(STRCHR(mode, 'w')) m |= SvcFileReader::WRITE;
    if(STRCHR(mode, 'a')) m |= SvcFileReader::APPEND;
    if(STRCHR(mode, '+')) m |= SvcFileReader::PLUS;
    if(STRCHR(mode, 'b')) m |= SvcFileReader::BINARY;
    if(STRCHR(mode, 't')) m |= SvcFileReader::TEXT;

    FileReaderEnum fre(filename, m, TRUE);
    svc_fileReader *fr;
    while( (fr=fre.getNext()) ) {
      if(fr->open(filename, m) == 1) {
        fileReaders.addItem(fr);
        return reinterpret_cast<FILE *>(fr);
      }
      fre.release(fr);
    }
  }

	// File still not found ...
#ifdef _DEBUG
	char tmp[512];
	wsprintf(tmp,"Warning: can't open %s\n",filename);
	DebugString(tmp);
#endif
  return 0;
#endif
}

int FCLOSE(FILE *stream) {
#if WASABI_STD_FILE_WANT_FILEREADERS
  if (fileReaders.searchItem(reinterpret_cast<svc_fileReader *>(stream)) != -1) {
    fileReaders.removeItem(reinterpret_cast<svc_fileReader *>(stream));
    reinterpret_cast<svc_fileReader *>(stream)->close();
    return 0;
  } 
#endif
  return fclose(stream);
}

int FSEEK(FILE *stream, long offset, int origin) {
#if WASABI_STD_FILE_WANT_FILEREADERS
  svc_fileReader *fr = reinterpret_cast<svc_fileReader *>(stream);
  if (fileReaders.searchItem(fr) != -1) {
    if(origin == SEEK_END) {
      int len = fr->getLength();
      if(len < 0) return -1;
      offset = len - offset;
      if(offset < 0) return -1; // but, do allow setting past EOF, let filreader decide.
    }
    else if(origin == SEEK_CUR) {
      int pos = fr->getPos();
      if(pos < 0 ) return -1;
      offset = pos + offset;
      if(offset < 0) return -1;
    }
    else
      ASSERTPR( origin == SEEK_SET, "Bad origin" );
    return fr->seek(offset);
  }
#endif
  return fseek(stream,offset,origin);
}

long FTELL(FILE *stream) {
#if WASABI_STD_FILE_WANT_FILEREADERS
  if (fileReaders.searchItem(reinterpret_cast<svc_fileReader *>(stream))!=-1)
    return reinterpret_cast<svc_fileReader *>(stream)->getPos();
#endif
  return ftell(stream);
}

size_t FREAD(void *buffer, size_t size, size_t count, FILE *stream) {
#if WASABI_STD_FILE_WANT_FILEREADERS
  if (fileReaders.searchItem(reinterpret_cast<svc_fileReader *>(stream))!=-1)
    return reinterpret_cast<svc_fileReader *>(stream)->read((char *)buffer,size*count);
#endif
  return fread(buffer,size,count,stream);
}

size_t FWRITE(const void *buffer, size_t size, size_t count, FILE *stream) {
#if WASABI_STD_FILE_WANT_FILEREADERS
  if (fileReaders.searchItem(reinterpret_cast<svc_fileReader *>(stream)) != -1)
    return reinterpret_cast<svc_fileReader *>(stream)->write((char *)buffer,size*count);
#endif
  return fwrite(buffer,size,count,stream);
}

int FGETSIZE(FILE *stream) {
#if WASABI_STD_FILE_WANT_FILEREADERS
  if (fileReaders.searchItem(reinterpret_cast<svc_fileReader *>(stream))!=-1)
    return reinterpret_cast<svc_fileReader *>(stream)->getLength();
#endif
  int p = ftell(stream);
  fseek(stream, 0, SEEK_END);
  int s = ftell(stream);
  fseek(stream, p, SEEK_SET);
  return s;
}

char *FGETS(char *string, int n, FILE *stream) {
#if WASABI_STD_FILE_WANT_FILEREADERS
  if (fileReaders.searchItem(reinterpret_cast<svc_fileReader *>(stream)) != -1) {
    char c;
    char *p=string;
    for(int i=0;i<(n-1);i++) {
      if(!reinterpret_cast<svc_fileReader *>(stream)->read(&c,1)) {
        if(!i) return NULL;
        break;
      }
      if(c==0x0d) continue;
      if(c==0x0a) break;
      *p++=c;
    }
    *p=0;
    return string;
  }
#endif
  return fgets(string,n,stream);
}

int FPRINTF(FILE *stream, const char *format , ...) {
  int ret;
  va_list args;
  va_start (args, format);	
#if WASABI_STD_FILE_WANT_FILEREADERS
  if (fileReaders.searchItem(reinterpret_cast<svc_fileReader *>(stream)) != -1) {
    String p;
    ret = p.vsprintf(format, args);
    FWRITE(p.v(), p.len(), 1, stream);
  } else
#endif
    ret = vfprintf(stream, format, args); //real stdio
  va_end (args);
  return ret;
}

int TMPNAM(char *str, const char *prefix) {
  int tmpnam_failed=0;
  if (prefix == NULL || *prefix == '\0') prefix = TMPNAME_PREFIX;

#ifdef WIN32
  String tempPath8;
  static char tempName8[WA_MAX_PATH];

#if UTF8 && 0 // commented out because it doesn't work and leaks tmpfiles
  int failed = 0;
  if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
    // CODE FOR NT COMPATIBLE OS'S
    wchar_t tempPath16[WA_MAX_PATH];
    wchar_t tempName16[WA_MAX_PATH];
    GetTempPathW(WA_MAX_PATH, tempPath16);
    if (tempPath16[WSTRLEN(tempPath16)-1] != '\\') {
      WSTRCAT(tempPath16, L"\\");
    }
    EncodedStr tmpPrefix;
    tmpPrefix.convertFromUTF8(SvcStrCnv::UTF16, String(prefix));

    //GetTempFileNameW(tempPath16, L"WT", 0, tempName16);
    if (!GetTempFileNameW(tempPath16, (wchar_t*)tmpPrefix.getEncodedBuffer(), 0, tempName16)) {
      tmpnam_failed = 1;
    } else {
      // Convert
      String tempN8;
      EncodedStr tempPath16enc(SvcStrCnv::UTF16, tempPath16, WSTRLEN(tempPath16) + 1, 0); // you must give the size PLUS the null term if any.
      tempPath16enc.convertToUTF8(tempPath8);
      EncodedStr tempName16enc(SvcStrCnv::UTF16, tempName16, WSTRLEN(tempName16) + 1, 0); // you must give the size PLUS the null term if any.
      int retcode = tempName16enc.convertToUTF8(tempN8);
      if (retcode > 0) {
        // Copy to the static retbuf.
        STRNCPY(tempName8, tempN8, WA_MAX_PATH);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        failed = 1;
//BUG: need to unlink the tmpfile that was created!
DebugString("ERROR: tmpfile leak");
      }
    }
  } else {
    // CODE FOR 9x COMPATIBLE OS'S
    char tempPathOS[WA_MAX_PATH];
    char tempNameOS[WA_MAX_PATH];
    GetTempPath(WA_MAX_PATH, tempPathOS);
    if (tempPathOS[STRLEN(tempPathOS)-1] != '\\') {
      STRCAT(tempPathOS, "\\");
    }
    if (!GetTempFileName(tempPathOS, prefix, 0, tempNameOS)) {
      tmpnam_failed = 1;
    } else {
      // Convert
      String tempN8;
      EncodedStr tempPathOSenc(SvcStrCnv::OSNATIVE, tempPathOS, STRLEN(tempPathOS) + 1, 0); // you must give the size PLUS the null term if any.
      tempPathOSenc.convertToUTF8(tempPath8);
      EncodedStr tempNameOSenc(SvcStrCnv::OSNATIVE, tempNameOS, STRLEN(tempNameOS) + 1, 0); // you must give the size PLUS the null term if any.
      int retcode = tempNameOSenc.convertToUTF8(tempN8);
      if (retcode > 0) {
        // Copy to the static retbuf.
        STRNCPY(tempName8, tempN8, WA_MAX_PATH);
      } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
        failed = 1;
//BUG: need to unlink the tmpfile that was created!
DebugString("ERROR: tmpfile leak");
      }
    }
  }
  // Drop back and punt.
  if (failed && !tmpnam_failed) {
    char tempPath[WA_MAX_PATH];
    static char tempName[WA_MAX_PATH];
    GetTempPath(WA_MAX_PATH, tempPath);
    if (tempPath[STRLEN(tempPath)-1] != '\\')
      STRCAT(tempPath, "\\");
    tmpnam_failed = !GetTempFileName(tempPath,prefix,0,tempName);
    STRNCPY(tempName8, tempName, WA_MAX_PATH);
  }
#else // The code that was there before.
  char tempPath[WA_MAX_PATH];
  static char tempName[WA_MAX_PATH];
  GetTempPath(WA_MAX_PATH, tempPath);
  if (tempPath[STRLEN(tempPath)-1] != '\\')
    STRCAT(tempPath, "\\");
  tmpnam_failed = !GetTempFileName(tempPath,prefix,0,tempName);
#endif

  if (str) {
#if UTF8 && 0 // commented out because it doesn't work and leaks tmpfiles
    STRCPY(str, tempName8);
#else
    STRCPY(str, tempName);
#endif
  }
#elif defined(WASABI_PLATFORM_POSIX)
  StringPrintf temp("%sXXXXXX", prefix);
  MEMCPY(str,temp.getValue(),temp.len()+1);
  mkstemp(str); //BUG: should check for failure here
//CUT  return (const char *)str;
#else
#error port me!
#endif
  return !tmpnam_failed;
}

int FEXISTS(const char *filename) {
  if (filename == NULL || *filename == '\0') return 0;
#if WASABI_STD_FILE_WANT_FILEREADERS
  FileReaderEnum fre(filename, SvcFileReader::READ, TRUE);
  svc_fileReader *fr = NULL;
  int r = 0;
  while( (fr=fre.getNext(FALSE/*no global lock*/)) && r <= 0) {
    r = fr->exists(filename);
    fre.getLastFactory()->releaseInterface(fr);
  }
  if(r > 0) return r;
#endif
  return (ACCESS(filename, 0) == 0);
}

int UNLINK(const char *filename) {
  if (filename == NULL || *filename == '\0') return TRUE; // sure, why not
#if defined(WASABI_PLATFORM_WIN32)
  return _unlink(filename);
#elif defined(WASABI_PLATFORM_POSIX)
  return unlink(filename); // this has been undefed at the top of this file
#else
#error port me!
#endif
}

int ACCESS(const char *filename, int mode) {
#ifdef WIN32
  return _access(filename, mode);
#else
  return access(filename, mode); // this has been undefed at the top of this file
#endif
}

int RENAME(const char *from, const char *to) {
  return !rename(from, to);
}

int FDELETE(const char *filename, int permanently) {
#if WASABI_STD_FILE_WANT_FILEREADERS
  FileReaderEnum fre(filename, SvcFileReader::READ, TRUE);
  svc_fileReader *fr = NULL;
  int r = 0;
  while( (fr=fre.getNext(FALSE)) && r <= 0 ) {
    if (permanently)
      r = fr->remove(filename);
    else
      r = fr->removeUndoable(filename);
    fre.getLastFactory()->releaseInterface(fr);
  }
  if(r > 0) return r;
#endif
  return UNLINK(filename);
}

int MOVEFILE(const char *filename, const char *destfilename) {
#if WASABI_STD_FILE_WANT_FILEREADERS
  FileReaderEnum fre(filename, SvcFileReader::READ, TRUE);
  svc_fileReader *fr = NULL;
  int r = 0;
  while( (fr=fre.getNext(FALSE)) && r <= 0 ) {
    r = fr->move(filename, destfilename);
    fre.getLastFactory()->releaseInterface(fr);
  }
  if(r > 0) return r;
#endif
#if defined(WASABI_PLATFORM_WIN32)
  return MoveFile(filename, destfilename);
#elif defined(WASABI_PLATFORM_POSIX)
  rename(filename, destfilename);
#else
#error port me!
#endif
}

int COPYFILE(const char *filename, const char *destfilename) {
#if WASABI_STD_FILE_WANT_FILEREADERS
  FileReaderEnum fre(filename, SvcFileReader::READ, TRUE);
  svc_fileReader *fr = NULL;
  int r = 0;
  while( (fr=fre.getNext(FALSE)) && r <= 0 ) {
    r = fr->copy(filename, destfilename);
    fre.getLastFactory()->releaseInterface(fr);
  }
  if(r > 0) return r;
#endif
#ifdef WIN32
//TODO: move the following code into the file readers
//#pragma CHAT("bas", "rOn", "feel free to move this into the readers")
  SHFILEOPSTRUCT s;
  ZERO(s);
  s.hwnd = NULL;

  s.wFunc = FO_COPY;

  char from[WA_MAX_PATH];
  STRNCPY(from, filename, WA_MAX_PATH-1); //leave space for extra null
  from[STRLEN(from)+1] = 0;	// extra null
  s.pFrom = from;

  char to[WA_MAX_PATH];
  STRNCPY(to, destfilename, WA_MAX_PATH-1); //leave space for extra null
  to[STRLEN(to)+1] = 0;	// extra null
  s.pTo = to;

  s.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI;

  int r = SHFileOperation(&s);
  
  if (s.fAnyOperationsAborted) return 0;
  return !r;
#else
  FILE *fin = fopen( filename, "rb" );
  FILE *fout = fopen( destfilename, "wb" );

  if ( !fin || ! fout )
    return 0;

  int bytes;
  char buf[32768];
  while( bytes = FREAD( buf, 1, 32768, fin ) ) {
    fwrite( buf, 1, bytes, fout );
  }

  fclose( fin );
  fclose( fout );

  return 1;
#endif
}

#ifdef WIN32
#include <shlobj.h>
#include <shellapi.h>

static HRESULT ResolveShortCut(LPCSTR pszShortcutFile, LPSTR pszPath, int maxbuf)
{
  HRESULT hres;
  IShellLink* psl;
  char szGotPath[MAX_PATH];
  WIN32_FIND_DATA wfd;

  *pszPath = 0;   // assume failure

  hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                            IID_IShellLink, (void **) &psl);
  if (SUCCEEDED(hres))
  {
    IPersistFile* ppf;

    hres = psl->QueryInterface(IID_IPersistFile, (void **) &ppf); // OLE 2!  Yay! --YO
    if (SUCCEEDED(hres))
    {
      WORD wsz[MAX_PATH];
      MultiByteToWideChar(CP_ACP, 0, pszShortcutFile, -1, (LPWSTR)wsz, MAX_PATH);

     hres = ppf->Load((LPCOLESTR)wsz, STGM_READ);
     if (SUCCEEDED(hres))
     {
       hres = psl->Resolve(HWND_DESKTOP, SLR_ANY_MATCH);
       if (SUCCEEDED(hres)) {
          strcpy(szGotPath, pszShortcutFile);
          hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA *)&wfd,
                 SLGP_SHORTPATH );
	        strncpy(pszPath,szGotPath, maxbuf);
          if (maxbuf) pszPath[maxbuf] = 0;
        }
      }
      ppf->Release();
    }
    psl->Release();
  }
  return SUCCEEDED(hres);
}
#endif

// ommitting a maxbuf param was just asking for trouble...
int StdFile::resolveShortcut(const char *filename, char *destfilename, int maxbuf) {
#ifdef WASABI_PLATFORM_WIN32
  return ResolveShortCut(filename, destfilename, maxbuf);
#elif defined(WASABI_PLATFORM_POSIX)
  //TODO: we may also want to resolve freedesktop.org's .Desktop files.
  readlink(filename, destfilename, maxbuf);
#else
#error port me!
#endif
}

int StdFile::isDirectory(const char *path) {
  ReadDir testForDir(path, "*.", 0/*don't skip dots*/); // using ReadDir to test for dir?
  if (testForDir.next()) {
    // the first file you get if you don't skip dots is your own self if you're a dir.
    return testForDir.isDir();
  }
  return FALSE;
}

#endif // ndef _NOSTUDIO
