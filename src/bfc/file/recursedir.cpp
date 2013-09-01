#include <precomp.h>
#include "recursedir.h"

RecurseDir::RecurseDir(const char *_path, const char *_match) :
  path(_path), match(_match)
{
  if (match.isempty()) match = Std::matchAllFiles();

  curdir = new ReadDir(path, match);
}

RecurseDir::~RecurseDir() {
  dirstack.deleteAll();
}

int RecurseDir::next() {
  for (;;) {
    if (curdir == NULL) {	// pop one off the stack
      curdir = dirstack.getLast();
      if (curdir == NULL) return 0;	// done
      dirstack.removeLastItem();
    }
    int r = curdir->next();
    if (r <= 0) {
      delete curdir; curdir = NULL;
      continue;	// get another one
    }

    // ok, we have a file to look at
    if (curdir->isDir()) {	// descend into it
      String newpath = curdir->getPath();
      newpath.cat(DIRCHARSTR);
      newpath.cat(curdir->getFilename());

      dirstack.addItem(curdir);	// push the old one
      curdir = new ReadDir(newpath, match);	// start new one

      continue;
    }

    return r;
  }
}

const char *RecurseDir::getPath() {
  if (curdir == NULL) return NULL;
  return curdir->getPath();
}

const char *RecurseDir::getFilename() {
  if (curdir == NULL) return NULL;
  return curdir->getFilename();
}

const char *RecurseDir::getOriginalPath() {
  return path;
}
