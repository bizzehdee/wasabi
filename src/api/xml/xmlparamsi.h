#ifndef __XMLPARAMSI_H
#define __XMLPARAMSI_H

#include <bfc/dispatch.h>
#include <bfc/string/string.h>
#include <bfc/ptrlist.h>

/*<?<autoheader/>*/
#include <api/xml/xmlparams.h>
#include <api/xml/xmlparamsx.h>
/*?>*/

class XmlReaderParamsI : public XmlReaderParamsX {
public:
  XmlReaderParamsI() {}
  virtual ~XmlReaderParamsI();

  DISPATCH(100) const char *getItemName(int i);
  DISPATCH(200) const char *getItemValue(int i);
  DISPATCH(201) const char *getItemValue(const char *name);
  DISPATCH(202) const char *enumItemValues(const char *name, int nb);
  DISPATCH(300) int getItemValueInt(const char *name, int def = 0);
  DISPATCH(400) int getNbItems();

  DISPATCH(500) void addItem(const char *parm, const char *value);
  DISPATCH(600) void removeItem(const char *parm);
  DISPATCH(700) void replaceItem(const char *parm, const char *value);
  DISPATCH(800) int findItem(const char *parm);
private:
  struct parms_struct {
    String parm;
    String value;
  };
  PtrList<parms_struct> parms_list;
};


#endif
