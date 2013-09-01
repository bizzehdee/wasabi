#ifndef __CONFIG_API_H
#define __CONFIG_API_H

#include <api/config/api_configi.h>

class ConfigApi : public api_configI {
  public:
    ConfigApi() {}
    virtual ~ConfigApi() {}

    virtual void config_registerCfgItem(CfgItem *cfgitem);
    virtual void config_deregisterCfgItem(CfgItem *cfgitem);
    virtual int config_getNumCfgItems();
    virtual CfgItem *config_enumCfgItem(int n);
    virtual CfgItem *config_getCfgItemByGuid(GUID g);
    virtual void setIntPrivate(const char *name, int val);
    virtual int getIntPrivate(const char *name, int def_val);
    virtual void setIntArrayPrivate(const char *name, const int *val, int nval);
    virtual int getIntArrayPrivate(const char *name, int *val, int nval);
    virtual void setStringPrivate(const char *name, const char *str);
    virtual int getStringPrivate(const char *name, char *buf, int buf_len, const char *default_str);
    virtual int getStringPrivateLen(const char *name);
    virtual void setIntPublic(const char *name, int val);
    virtual int getIntPublic(const char *name, int def_val);
    virtual void setStringPublic(const char *name, const char *str);
    virtual int getStringPublic(const char *name, char *buf, int buf_len, const char *default_str);

  protected:
};


#endif
