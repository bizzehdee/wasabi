#include <precomp.h>
#include "xmlparamsi.h"

XmlReaderParamsI::~XmlReaderParamsI() {
  parms_list.deleteAll();
}

const char *XmlReaderParamsI::getItemName(int i) {
  if(i>=getNbItems()) return "";
  return parms_list[i]->parm;
}

const char *XmlReaderParamsI::getItemValue(int i) {
  if(i>=getNbItems()) return "";
  return parms_list[i]->value;
}

const char *XmlReaderParamsI::getItemValue(const char *name) {
  for(int i=0;i<getNbItems();i++)
    if(STRCASEEQL(parms_list[i]->parm, name))
      return parms_list[i]->value;
  return NULL;
}

const char *XmlReaderParamsI::enumItemValues(const char *name, int nb) {
  int f=0;
  for(int i=0;i<getNbItems();i++)
    if(STRCASEEQL(parms_list[i]->parm, name))
      if(f==nb)
        return parms_list[i]->value;
      else f++;
  return NULL;
}

int XmlReaderParamsI::getItemValueInt(const char *name, int def) {
  for(int i=0;i<getNbItems();i++)
    if(STRCASEEQL(parms_list[i]->parm, name))
      return atoi(parms_list[i]->value);
  return def;
}

int XmlReaderParamsI::getNbItems() {
  return parms_list.getNumItems();
}

void XmlReaderParamsI::addItem(const char *parm, const char *value) {
  parms_struct *p= new parms_struct;
  p->parm = parm;
  p->value = value;
  parms_list.addItem(p);
}

void XmlReaderParamsI::removeItem(const char *parm) {
  for (int i=0;i<parms_list.getNumItems();i++) {
    parms_struct *s = parms_list.enumItem(i);
    if (STRCASEEQLSAFE(parm, s->parm)) {
      delete s;
      parms_list.removeByPos(i);
      i--;
    }
  }
}

void XmlReaderParamsI::replaceItem(const char *parm, const char *value) {
  if (!value) {
    removeItem(parm);
    return;
  }

  String s = value; // if we were passed our current value's pointer ... 

  const char *curval = getItemValue(parm);
  if (s.isequal(curval) && curval) return; // (hey, if we're replacing with same value, forget about it, but only if we did have that value, because isequal will return true if curval is NULL and we pass it ("") )

  removeItem(parm); // ... then this call would make the value param invalid ...

  addItem(parm, s); // ... so we're sending a saved buffer instead
}

int XmlReaderParamsI::findItem(const char *parm) {
  for(int i=0;i<getNbItems();i++)
    if(STRCASEEQL(parms_list[i]->parm, parm))
      return i;
  return -1;
}

