#ifndef _STD_FILE_H
#define _STD_FILE_H

#define WA_MAX_PATH (8*1024)

#ifndef EXTC
#define EXTC
#endif

#ifndef _NOSTUDIO
// EXTC is used here as some .c files will use these functions
EXTC FILE * FOPEN(const char *filename,const char *mode);
#ifdef WASABI_PLATFORM_POSIX
EXTC FILE *FOPEN_NOCASE(const char *filename,const char *mode);
#endif
EXTC int FCLOSE(FILE *stream);
EXTC int FSEEK(FILE *stream, long offset, int origin);
EXTC long FTELL(FILE *stream);
EXTC size_t FREAD(void *buffer, size_t size, size_t count, FILE *stream);
EXTC size_t FWRITE(const void *buffer, size_t size, size_t count, FILE *stream);
EXTC char *FGETS( char *string, int n, FILE *stream );
EXTC int FPRINTF( FILE *stream, const char *format , ...);
EXTC int FGETSIZE(FILE *stream);
EXTC int TMPNAM(char *buf, const char *prefix=NULL);	// returns 0 on error
//BU killed TMPNAM2
EXTC int FEXISTS(const char *filename); // return 1 if true, 0 if not, -1 if unknown
EXTC int UNLINK(const char *filename); // return 1 on success, 0 on error
EXTC int ACCESS(const char *filename, int mode); // returns 0 if mode is set
EXTC int RENAME(const char *from, const char *to);
#ifdef __cplusplus
// returns 1 on success, 0 on error, -1 if undoable deletes aren't supported
EXTC int FDELETE(const char *filename, int permanently=TRUE);
#else
EXTC int FDELETE(const char *filename, int permanently);
#endif

// 1 on success, 0 on fail
// can't move directories between volumes on win32
EXTC int MOVEFILE(const char *filename, const char *destfilename);
// 1 on success, 0 on fail
EXTC int COPYFILE(const char *filename, const char *destfilename);

#ifdef __cplusplus
namespace StdFile {
#endif
  int resolveShortcut(const char *filename, char *destfilename, int maxbuf);
  int isDirectory(const char *path);
#ifdef __cplusplus
};
#endif

#endif //_nostudio

#endif
