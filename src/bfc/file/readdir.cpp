#include <precomp.h>
#include "readdir.h"

#if !defined(WIN32) && !defined(LINUX)
#error port me
#endif

#if UTF8
#ifdef WANT_UTF8_WARNINGS
#pragma CHAT("mig", "all", "UTF8 is enabled in readdir.cpp -- Things might be screwy till it's all debugged?")
#endif
# include <bfc/string/encodedstr.h>
#endif

//PORT
ReadDir::ReadDir(const char *_path, const char *_match, BOOL _skipdots) :
  skipdots(_skipdots), first(1), path(_path), match(_match) {
#ifdef WIN32
  files = INVALID_HANDLE_VALUE;
#if UTF8
  utf16 = Std::encodingSupportedByOS(SvcStrCnv::UTF16);
#endif
  if (match.isempty()) match = MATCHALLFILES;
  ZERO(data);
#endif
#ifdef LINUX
  if (!Std::isDirChar(path.lastChar())) path.cat(DIRCHARSTR);
  d = NULL;
#endif
}

ReadDir::~ReadDir() {
//PORT
#ifdef WIN32
  if (files != INVALID_HANDLE_VALUE) FindClose(files);
#endif
#ifdef LINUX
  if (d != NULL ) closedir( d );
#endif
}

int ReadDir::next() {
//PORT
#ifdef WIN32
  for (;;) {
    if (first) {
      StringPrintf fullpath("%s%s%s", path.getValue(), path.isempty() ? "" : DIRCHARSTR, match.getValue());
#if UTF8
      if (utf16) {
        // CODE FOR NT COMPATIBLE OS'S
        EncodedStr path16enc;
        int retcode = path16enc.convertFromUTF8(SvcStrCnv::UTF16, fullpath);
        WCHAR *path16 = static_cast<WCHAR *>(path16enc.getEncodedBuffer());
        if (path16 != NULL) {
          files = FindFirstFileW(path16, &dataW);
        } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
          files = FindFirstFile(fullpath, &data);
          // yea... and we shouldn't be in the 16 side of the family, either.
          utf16 = 0;
        }
      } else {
        // CODE FOR 9x COMPATIBLE OS'S
        EncodedStr pathOSenc;
        int retcode = pathOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, fullpath);
        char *pathOS = static_cast<char *>(pathOSenc.getEncodedBuffer());
        if (pathOS != NULL) {
          files = FindFirstFileA(pathOS, (LPWIN32_FIND_DATAA)&data);
        } else if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
          files = FindFirstFileA(fullpath, (LPWIN32_FIND_DATAA)&data);
        }
      }
#else
      files = FindFirstFile(fullpath, &data);
#endif
    }
    if (files == INVALID_HANDLE_VALUE) return 0;
    if (first) {
      first = 0;
      if (skipdots && (isDotDir() || isDotDotDir())) continue;
      return 1;
    }
#if UTF8
    if (utf16) {
      // CODE FOR NT COMPATIBLE OS'S
      if (!FindNextFileW(files, &dataW)) return 0;
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      if (!FindNextFile(files, &data)) return 0;
    }
#else
    if (!FindNextFile(files, &data)) return 0;
#endif
    if (skipdots && (isDotDir() || isDotDotDir())) continue;
    return 1;
  }
#endif//WIN32
#ifdef LINUX
  int a;

  if (!path.isempty() && !Std::isDirChar(path.lastChar())) path += DIRCHARSTR;
  if ( first || d == NULL ) {
    if ( !(d = opendir( path )) ) return 0;
    first = 0;
  }

  while (1) {
    de = readdir( d );
    if ( ! de ) {
      closedir ( d );
      d = NULL;
      return 0;
    }

    String full;
    full.printf( "%s%s", (const char *)path, de->d_name );

    if ( stat( full, &st ) == -1 )
      continue;

    if (skipdots && (isDotDir() || isDotDotDir())) continue;

    if (!Std::match(match, de->d_name)) continue;

    return 1;
  }

#endif
}

const char *ReadDir::getFilename() {
  if (first) if (!next()) return NULL;
//PORT
#ifdef WIN32
#if UTF8
  if (utf16) {
    // CODE FOR NT COMPATIBLE OS'S
    EncodedStr fileenc(SvcStrCnv::UTF16, dataW.cFileName, (WSTRLEN(dataW.cFileName)+1)*2, 0/*no delete!!*/);
    int retcode = fileenc.convertToUTF8(filename8);
    if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
      return (char *)data.cFileName;
    }
  } else {
    // CODE FOR 9x COMPATIBLE OS'S
    EncodedStr fileenc(SvcStrCnv::OSNATIVE, data.cFileName, (STRLEN(((char *)data.cFileName))+1), 0/*no delete!!*/);
    int retcode = fileenc.convertToUTF8(filename8);
    if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
      return (char *)data.cFileName;
    }
  }
  return filename8;
#else
  return data.cFileName;
#endif
#endif//WIN32
#ifdef LINUX
  return de->d_name;
#endif
}

int ReadDir::isDir() {
//PORT
#ifdef WIN32
  if (files == INVALID_HANDLE_VALUE) return 0;
#if UTF8
  if (utf16) {
    return !!(dataW.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
  } else {
    return !!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
  }
#else
  return !!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
#endif
#endif
#ifdef LINUX
  if ( d == NULL ) return 0;
  return S_ISDIR( st.st_mode );
#endif
}

int ReadDir::isReadonly() {
//PORT
#ifdef WIN32
  if (files == INVALID_HANDLE_VALUE) return 0;
#if UTF8
  if (utf16) {
    return !!(dataW.dwFileAttributes & FILE_ATTRIBUTE_READONLY);
  } else {
    return !!(data.dwFileAttributes & FILE_ATTRIBUTE_READONLY);
  }
#else
  return !!(data.dwFileAttributes & FILE_ATTRIBUTE_READONLY);
#endif
#endif//WIN32
#ifdef LINUX
  if ( d == NULL ) return 0;
  return !( st.st_mode & 0222 ); // Not exactly a read-only check, but close
#endif
}

int ReadDir::isSystemFile() {
#ifdef WIN32
  if (files == INVALID_HANDLE_VALUE) return 0;
#if UTF8
  if (utf16) {
    return !!(dataW.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM);
  } else {
    return !!(data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM);
  }
#else
  return !!(data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM);
#endif
#endif//WIN32
}

int ReadDir::isEncrypted() {
#ifdef WIN32
  if (files == INVALID_HANDLE_VALUE) return 0;
#if UTF8
  if (utf16) {
    return !!(dataW.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED);
  } else {
    return !!(data.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED);
  }
#else
  return !!(data.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED);
#endif
#endif//WIN32
}

int ReadDir::isCompressed() {
#ifdef WIN32
  if (files == INVALID_HANDLE_VALUE) return 0;
#if UTF8
  if (utf16) {
    return !!(dataW.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED);
  } else {
    return !!(data.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED);
  }
#else
  return !!(data.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED);
#endif
#endif//WIN32
}

int ReadDir::isTempFile() {
#ifdef WIN32
  if (files == INVALID_HANDLE_VALUE) return 0;
#if UTF8
  if (utf16) {
    return !!(dataW.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY);
  } else {
    return !!(data.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY);
  }
#else
  return !!(data.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY);
#endif
#endif//WIN32
}

int ReadDir::isNormalFile() {
#ifdef WIN32
  if (files == INVALID_HANDLE_VALUE) return 0;
#if UTF8
  if (utf16) {
    return !!(dataW.dwFileAttributes & FILE_ATTRIBUTE_NORMAL);
  } else {
    return !!(data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL);
  }
#else
  return !!(data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL);
#endif
#endif//WIN32
}

__int64 ReadDir::getFileSize() {
#ifdef WIN32
  if (files == INVALID_HANDLE_VALUE) return 0;
#if UTF8
  if (utf16) {
    return (dataW.nFileSizeHigh * MAXDWORD) + dataW.nFileSizeLow;
  } else {
    return (data.nFileSizeHigh * MAXDWORD) + data.nFileSizeLow;
  }
#else
  return (data.nFileSizeHigh * MAXDWORD) + data.nFileSizeLow;
#endif
#endif//WIN32
}

int ReadDir::isDotDir() {
//PORT
#ifdef WIN32
  if (files == INVALID_HANDLE_VALUE) return 0;
#if UTF8
  if (utf16) {
    return !!!WSTRCMP7(dataW.cFileName, DOTDIR);
  } else {
    return !!!STRCMP((const char *)data.cFileName, DOTDIR);
  }
#else
  return !STRCMP(data.cFileName, DOTDIR);
#endif
#endif//WIN32
#ifdef LINUX
  if ( d == NULL ) return 0;
  return !STRCMP(de->d_name, DOTDIR );
#endif
}

int ReadDir::isDotDotDir() {
//PORT
#ifdef WIN32
  if (files == INVALID_HANDLE_VALUE) return 0;
#if UTF8
  if (utf16) {
    return !WSTRCMP7(dataW.cFileName, DOTDOTDIR);
  } else {
    return !STRCMP((const char *)data.cFileName, DOTDOTDIR);
  }
#else
  return !STRCMP(data.cFileName, DOTDOTDIR);
#endif
#endif
#ifdef LINUX
  if ( d == NULL ) return 0;
  return !STRCMP(de->d_name, DOTDOTDIR );
#endif
}
