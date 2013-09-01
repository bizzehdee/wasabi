#include <precomp.h>
#include "wacloader.h"
#include <bfc/file/readdir.h>

WacLoader::WacLoader(const char *_subdir, const char *pattern) {
  String sd = _subdir;
  if (Std::isDirChar(sd.firstChar())) sd = _subdir+1;
  if (Std::isDirChar(sd.lastChar())) sd.trunc(sd.numCharacters()-1);

	char m_dir[MAX_PATH];
	Std::getCurDir(m_dir, MAX_PATH);
	m_wacdir = m_dir;
  if (!Std::isDirChar(m_wacdir.lastChar())) m_wacdir.cat(DIRCHARSTR);
	m_wacdir.cat(sd);
  m_wacdir.cat(DIRCHARSTR);

	ReadDir m_read(m_wacdir, pattern);
	while(m_read.next()) {
		String m_full = m_read.getPath();
		m_full += m_read.getFilename();
		m_wacs.addItem(new waContainer(m_full));
	}
  foreach(m_wacs)
    m_wacs.getfor()->callCreate();
  endfor;
}

WacLoader::~WacLoader() {
  foreach(m_wacs)
    m_wacs.getfor()->callDestroy();
  endfor
	m_wacs.deleteAll();
}
