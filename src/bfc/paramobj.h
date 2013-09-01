#ifndef _PARAMOBJ_H
#define _PARAMOBJ_H

#include <bfc/string/string.h>
#include <bfc/map.h>

// kinda like Named but with a Map of taggable string and a virtual onParamSet()

class ParamObj {
public:
  ParamObj(ParamObj *po=NULL);
  virtual ~ParamObj() { }

  void setParam(const char *tag, const char *buf);
  void setParam(const char *tag, int v);

  String getParam(const char *tag, const char *default_val="") const;

  int getNumParams() const;
  int enumParam(int n, String *tag, String *value) const;
  int paramEmpty(const char *tag) const;

  void copyParamsFrom(ParamObj *po, int clear_previous=FALSE);
  void clearAll();

  String operator[](const char *tag) { return getParam(tag); } // haha kinda wasteful to do String(String()) but eh works for now

protected:
  virtual void onParamSet(const char *tag, const char *buf) { (void)tag; (void)buf; }

  // this is a refcounted tag, so N objects with name=blah don't force
  // allocating storage of "name" N times
  class ParamObjTag {
    public:
      ParamObjTag(const char *v=NULL);
      ParamObjTag(const ParamObjTag &pot);
      ParamObjTag(const ParamObjTag *pot);
      ~ParamObjTag();
      ParamObjTag & operator=(const ParamObjTag &pot) {
        if (&pot != this) setVal(pot.getVal());
        return *this;
      }
      void setVal(const char *v);
      inline const char *getVal() const { return val; }
      inline int operator ==(const ParamObjTag &cmp) const {
        return (val == cmp.val);	// because they are in stringtable
      }
      inline int operator <(const ParamObjTag &cmp) const {
        return (STRCMPSAFE(getVal(), cmp.getVal()) < 0);
      }
      
    private:
      const char *val;
  };

  // this is mutable since Map<> isn't const-correct at this time --BU
  mutable Map<ParamObjTag, String> attrlist;
};

#endif
