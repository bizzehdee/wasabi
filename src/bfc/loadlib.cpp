#include <precomp.h>

#include "loadlib.h"

#if !defined(WIN32) && !defined(LINUX)
#error port me
#endif

Library::Library(const char *filename) : Named(filename) {
  lib = NULL;
}

Library::~Library() {
  unload();
}

int Library::load(const char *newname) {
  if (lib != NULL && newname == NULL) return 1;
  unload();
  if (newname != NULL) setName(newname);

  const char *n = getName();
  ASSERT(n != NULL);
#ifdef WIN32
#if UTF8
  int utf16 = Std::encodingSupportedByOS(SvcStrCnv::UTF16);
  if(utf16) {
    EncodedStr name16enc;
    int retcode = name16enc.convertFromUTF8(SvcStrCnv::UTF16, n);
    wchar_t *name16 = static_cast<wchar_t *>(name16enc.getEncodedBuffer());
    if(name16 != NULL) {
      //__try {
        lib = LoadLibraryW(name16);
      /*} __except(EXCEPTION_EXECUTE_HANDLER) {
        // stupid DLL
        lib = NULL;
        OutputDebugStringW(L"exception while loading dll");
        OutputDebugStringW(name16);
        OutputDebugStringW(L"\n");
      }*/
    }
  } else {
    EncodedStr nameOSenc;
    int retcode = nameOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, n);
    char *nameOs = static_cast<char *>(nameOSenc.getEncodedBuffer());
    if(nameOs != NULL) {
      //__try {
        lib = LoadLibraryA(nameOs);
      /*} __except(EXCEPTION_EXECUTE_HANDLER) {
        // stupid DLL
        lib = NULL;
        OutputDebugStringA("exception while loading dll");
        OutputDebugStringA(nameOs);
        OutputDebugStringA("\n");
      }*/
    }
  }
#else
  __try {
    lib = LoadLibrary(n);
  } __except(EXCEPTION_EXECUTE_HANDLER) {
    // stupid DLL
    lib = NULL;
OutputDebugString("exception while loading dll");
OutputDebugString(newname);
OutputDebugString("\n");
  }
#endif
#elif defined(LINUX)
  // Not using string to try to not use common/wasabi in Studio.exe
  char *conv = strdup( getName() );
  int len = strlen( conv );
  if ( ! strcasecmp( conv + len - 4, ".dll" ) ) {
    strcpy( conv + len - 4, ".so" );
  }

  lib = dlopen(conv, RTLD_LAZY);

  free( conv );
#else
#error port me!
#endif
  if (lib == NULL) return 0;
  return 1;
}

void Library::unload() {
  if (lib != NULL) {
#ifdef WIN32
    FreeLibrary(lib);
#elif defined(LINUX)
    dlclose(lib);
#else
#error port me!
#endif
  }
  lib = NULL;
}

void *Library::getProcAddress(const char *procname) {
  ASSERT(procname != NULL);
#if defined(WIN32)
  return GetProcAddress(lib, procname);
#elif defined(LINUX)
  return dlsym(lib,procname);
#else
#error port me!
#endif
}
