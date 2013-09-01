#include <precomp.h>
//<?#include "<class data="implementationheader"/>"
#include "api_filereaderI.h"
//?>

api_filereader *fileApi = NULL;

api_filereaderI::api_filereaderI() {

}

api_filereaderI::~api_filereaderI() {

}

int api_filereaderI::fileCanOpen(const char *filename, int mode) {
	svc_fileReader *m_svc_rdr = (svc_fileReader *)&FileReaderEnum(filename);
  if(m_svc_rdr->open(filename, mode)) {
    m_svc_rdr->close();
    return 1;
  }
	return 0;
}

void *api_filereaderI::fileOpen(const char *filename, int mode) {
  svc_fileReader *m_svc_rdr = (svc_fileReader *)&FileReaderEnum(filename);
  if(m_svc_rdr->open(filename, mode)) {
    return (void *)m_svc_rdr;
  }
	return NULL;
}

void api_filereaderI::fileClose(void *file) {
  if(file != NULL) {
	  svc_fileReader *m_svc_rdr = (svc_fileReader *)file;
    m_svc_rdr->close();
  }
}

int api_filereaderI::fileRead(void *file, char *buf, int len) {
  if(file != NULL) {
    svc_fileReader *m_svc_rdr = (svc_fileReader *)file;
    return m_svc_rdr->read(buf, len);
  }
	return 0;
}

int api_filereaderI::fileWrite(void *file, char *buf, int len) {
  if(file != NULL) {
    svc_fileReader *m_svc_rdr = (svc_fileReader *)file;
    return m_svc_rdr->write(buf, len);
  }
	return 0;
}

int api_filereaderI::fileCanSeek(void *file) {
  if(file != NULL) {
    svc_fileReader *m_svc_rdr = (svc_fileReader *)file;
    return m_svc_rdr->canSeek();
  }
	return 0;
}

int api_filereaderI::fileSeek(void *file, int start, int dist) {
  if(file != NULL) {
    svc_fileReader *m_svc_rdr = (svc_fileReader *)file;
    return m_svc_rdr->seek(start+dist);
  }
	return 0;
}

int api_filereaderI::fileExists(const char *filename) {
  if(filename != NULL) {
    svc_fileReader *m_svc_rdr = (svc_fileReader *)&FileReaderEnum(filename);

    return m_svc_rdr->exists(filename);
  }
	return 0;
}

int api_filereaderI::fileIsDirectory(const char *filename) {
	return Std::isDirectory(filename);
}
