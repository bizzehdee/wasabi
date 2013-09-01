#include "precomp.h"

#include "paramobj.h"

ParamObj::ParamObj(ParamObj *po) {
  if (po) copyParamsFrom(po);
}

void ParamObj::setParam(const char *tag, const char *buf) {
  ASSERT(tag != NULL);
  if (*tag == '\0') return;	// empty tag

  if (buf == NULL || *buf == '\0') {
    attrlist.delItem(ParamObjTag(tag));
//DebugString("del %s", tag);
  } else {
    attrlist.setItem(ParamObjTag(tag), String(buf));
//DebugString("set %s = %s", tag, buf);
  }

  onParamSet(tag, buf);
}

void ParamObj::setParam(const char *tag, int v) {
  setParam(tag, StringPrintf("%d", v));
}

String ParamObj::getParam(const char *tag, const char *default_val) const {
  if (this == NULL) return String("this == NULL");	// hehe
  return attrlist.getItemByValue(ParamObjTag(tag), String(default_val));
}

int ParamObj::getNumParams() const {
  return attrlist.getNumItems();
}

int ParamObj::enumParam(int n, String *tag, String *value) const {
  ParamObjTag t;
  int r = attrlist.enumPair(n, &t, value);
  tag->setValue(t.getVal());
  return r;
}

int ParamObj::paramEmpty(const char *tag) const {
  return getParam(tag).isempty();
}

void ParamObj::copyParamsFrom(ParamObj *po, int clear_previous) {
  if (clear_previous) {
    attrlist.deleteAll();
    onParamSet(NULL, NULL);
  }
  if (po == NULL) return;
  for (int i = 0; i < po->attrlist.getNumItems(); i++) {
    String idx(po->attrlist.enumIndexByPos(i, ParamObjTag()).getVal());
    String dat(po->attrlist.enumItemByPos(i, String((const char *)0)));
    if (!idx.isempty() && !dat.isempty()) setParam(idx, dat);
  }
}

void ParamObj::clearAll() {
  copyParamsFrom(NULL, TRUE);
}

//////////////////////

#include <bfc/string/stringtable.h>

static StringTable tagtable;

ParamObj::ParamObjTag::ParamObjTag(const char *v) {
  val = NULL;
  setVal(v);
}

ParamObj::ParamObjTag::ParamObjTag(const ParamObjTag &pot) {
  val = NULL;
  if (&pot != this) {
    setVal(pot.getVal());
  }
}
ParamObj::ParamObjTag::ParamObjTag(const ParamObjTag *pot) {
  val = NULL;
  if (pot != NULL && pot != this) {
    setVal(pot->getVal());
  }
}

ParamObj::ParamObjTag::~ParamObjTag() {
  tagtable.deallocStr(val);
}


void ParamObj::ParamObjTag::setVal(const char *v) {
  tagtable.deallocStr(val); val = NULL;
  val = tagtable.allocStr(v);	// NULL is ok
}
