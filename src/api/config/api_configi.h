#ifndef __API_CONFIG_IMPL_H
#define __API_CONFIG_IMPL_H

/*<?<autoheader/>*/
#include "api_config.h"
#include "api_configx.h"

class CfgItem;
class ConfigFile;
/*?>*/

#include <api/service/services.h>

class api_configI : public api_configX {
public:
  api_configI();
  virtual ~api_configI();

  static const char *getServiceName() { return "Config API"; }
  static GUID getServiceType() { return WaSvc::UNIQUE; }
  
  DISPATCH(10) virtual void config_registerCfgItem(CfgItem *cfgitem);
  DISPATCH(20) virtual void config_deregisterCfgItem(CfgItem *cfgitem);
  DISPATCH(30) virtual int config_getNumCfgItems();
  DISPATCH(40) virtual CfgItem *config_enumCfgItem(int n);
  DISPATCH(50) virtual CfgItem *config_getCfgItemByGuid(GUID g);
  DISPATCH(60) virtual void setIntPrivate(const char *name, int val);
  DISPATCH(70) virtual int getIntPrivate(const char *name, int def_val);
  DISPATCH(80) virtual void setIntArrayPrivate(const char *name, const int *val, int nval);
  DISPATCH(90) virtual int getIntArrayPrivate(const char *name, int *val, int nval);
  DISPATCH(100) virtual void setStringPrivate(const char *name, const char *str);
  DISPATCH(110) virtual int getStringPrivate(const char *name, char *buf, int buf_len, const char *default_str);
  DISPATCH(120) virtual int getStringPrivateLen(const char *name);
  DISPATCH(130) virtual void setIntPublic(const char *name, int val);
  DISPATCH(140) virtual int getIntPublic(const char *name, int def_val);
  DISPATCH(150) virtual void setStringPublic(const char *name, const char *str);
  DISPATCH(160) virtual int getStringPublic(const char *name, char *buf, int buf_len, const char *default_str);
private:
  ConfigFile *public_config;
};

/*[interface.footer.h]
// {470B890C-4747-4113-ABA5-A8F8F4F8AB7E}
static const GUID configApiServiceGuid = 
{ 0x470b890c, 0x4747, 0x4113, { 0xab, 0xa5, 0xa8, 0xf8, 0xf4, 0xf8, 0xab, 0x7e } };

extern api_config *configApi;
*/

#endif // __API_CONFIG_IMPL_H
