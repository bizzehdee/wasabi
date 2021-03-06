/*!*/ // ----------------------------------------------------------------------------
/*!*/ // Generated by InterfaceFactory [Sun May 30 10:47:30 2010]
/*!*/ // 
/*!*/ // File        : api_filereader.h
/*!*/ // Class       : api_filereader
/*!*/ // class layer : Dispatchable Interface
/*!*/ // ----------------------------------------------------------------------------
/*!*/ 
#ifndef __API_FILEREADER_H
#define __API_FILEREADER_H
/*!*/ 
#include "bfc/dispatch.h"
/*!*/ 
/*!*/ 
/*!*/ 
/*!*/ 
/*!*/ namespace FileReader {
/*!*/  enum {
/*!*/  READ=1,
/*!*/  WRITE=2,
/*!*/  APPEND=4,
/*!*/  PLUS=8,
/*!*/  BINARY=16,
/*!*/  TEXT=32,
/*!*/  };
/*!*/ };
/*!*/ 
/*!*/ // ----------------------------------------------------------------------------
/*!*/ 
/*!*/ class COMEXP api_filereader: public Dispatchable {
/*!*/   protected:
/*!*/     api_filereader() {}
/*!*/     ~api_filereader() {}
/*!*/   public:
/*!*/     int fileCanOpen(const char *filename, int mode);
/*!*/     void *fileOpen(const char *filename, int mode);
/*!*/     void fileClose(void *file);
/*!*/     int fileRead(void *file, char *buf, int len);
/*!*/     int fileWrite(void *file, char *buf, int len);
/*!*/     int fileCanSeek(void *file);
/*!*/     int fileSeek(void *file, int start, int dist);
/*!*/     int fileExists(const char *filename);
/*!*/     int fileIsDirectory(const char *filename);
/*!*/   
/*!*/   protected:
/*!*/     enum {
/*!*/       API_FILEREADER_FILECANOPEN = 10,
/*!*/       API_FILEREADER_FILEOPEN = 20,
/*!*/       API_FILEREADER_FILECLOSE = 30,
/*!*/       API_FILEREADER_FILEREAD = 40,
/*!*/       API_FILEREADER_FILEWRITE = 50,
/*!*/       API_FILEREADER_FILECANSEEK = 60,
/*!*/       API_FILEREADER_FILESEEK = 70,
/*!*/       API_FILEREADER_FILEEXISTS = 80,
/*!*/       API_FILEREADER_FILEISDIRECTORY = 90,
/*!*/     };
/*!*/ };
/*!*/ 
/*!*/ // ----------------------------------------------------------------------------
/*!*/ 
/*!*/ inline int api_filereader::fileCanOpen(const char *filename, int mode) {
/*!*/   int __retval = _call(API_FILEREADER_FILECANOPEN, (int)0, filename, mode);
/*!*/   return __retval;
/*!*/ }
/*!*/ 
/*!*/ inline void *api_filereader::fileOpen(const char *filename, int mode) {
/*!*/   void *__retval = _call(API_FILEREADER_FILEOPEN, (void *)NULL, filename, mode);
/*!*/   return __retval;
/*!*/ }
/*!*/ 
/*!*/ inline void api_filereader::fileClose(void *file) {
/*!*/   _voidcall(API_FILEREADER_FILECLOSE, file);
/*!*/ }
/*!*/ 
/*!*/ inline int api_filereader::fileRead(void *file, char *buf, int len) {
/*!*/   int __retval = _call(API_FILEREADER_FILEREAD, (int)0, file, buf, len);
/*!*/   return __retval;
/*!*/ }
/*!*/ 
/*!*/ inline int api_filereader::fileWrite(void *file, char *buf, int len) {
/*!*/   int __retval = _call(API_FILEREADER_FILEWRITE, (int)0, file, buf, len);
/*!*/   return __retval;
/*!*/ }
/*!*/ 
/*!*/ inline int api_filereader::fileCanSeek(void *file) {
/*!*/   int __retval = _call(API_FILEREADER_FILECANSEEK, (int)0, file);
/*!*/   return __retval;
/*!*/ }
/*!*/ 
/*!*/ inline int api_filereader::fileSeek(void *file, int start, int dist) {
/*!*/   int __retval = _call(API_FILEREADER_FILESEEK, (int)0, file, start, dist);
/*!*/   return __retval;
/*!*/ }
/*!*/ 
/*!*/ inline int api_filereader::fileExists(const char *filename) {
/*!*/   int __retval = _call(API_FILEREADER_FILEEXISTS, (int)0, filename);
/*!*/   return __retval;
/*!*/ }
/*!*/ 
/*!*/ inline int api_filereader::fileIsDirectory(const char *filename) {
/*!*/   int __retval = _call(API_FILEREADER_FILEISDIRECTORY, (int)0, filename);
/*!*/   return __retval;
/*!*/ }
/*!*/ 
/*!*/ // ----------------------------------------------------------------------------
/*!*/ 
/*!*/ 
/*!*/ // {9879C23B-0DD1-4494-AFA2-DAF9CC1E4F60}
/*!*/ static const GUID fileApiServiceGuid= 
/*!*/ { 0x9879c23b, 0xdd1, 0x4494, { 0xaf, 0xa2, 0xda, 0xf9, 0xcc, 0x1e, 0x4f, 0x60 } };
/*!*/ 
/*!*/ extern api_filereader *fileApi;
/*!*/ 
#endif // __API_FILEREADER_H
