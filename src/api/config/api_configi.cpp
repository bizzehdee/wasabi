#include <precomp.h>
//<?#include "<class data="implementationheader"/>"
#include "api_configi.h"
//?>

#include <parse/pathparse.h>
#include <api/config/config.h>
#include <api/config/cfglist.h>

static CfgList cfglist;

api_config *configApi = NULL;

api_configI::api_configI() {
  //CUT public_config = new ConfigFile(StringPrintf(INVALID_GUID), "public data");
  // BU making section name NULL means it will be stored as public data.
  // later on we should add a method to the api to create an api* for a
  // specific section
  public_config = new ConfigFile("public data", "public data");
  public_config->initialize();
}

api_configI::~api_configI() {
  delete public_config;
}

void api_configI::config_registerCfgItem(CfgItem *cfgitem) {
  cfglist.addItem(cfgitem);
}

void api_configI::config_deregisterCfgItem(CfgItem *cfgitem) {
  cfglist.delItem(cfgitem);
}

int api_configI::config_getNumCfgItems() {
  return cfglist.getNumItems();
}

CfgItem *api_configI::config_enumCfgItem(int n) {
  return cfglist.enumItem(n);
}

CfgItem *api_configI::config_getCfgItemByGuid(GUID g) {
  return cfglist.getByGuid(g);
}

// The private config functions are currently pointing at the public config item, this is because
// only the monolithic api gets instantiated once per component and thus can know its GUID, this
// version of the config api should eventually be instantiated once per component as well when
// we start making them use the modular apis
void api_configI::setIntPrivate(const char *name, int val) {
  public_config->setInt(name, val);
}

int api_configI::getIntPrivate(const char *name, int def_val) {
  int ret = public_config->getInt(name, def_val);
  return ret;
}

void api_configI::setIntArrayPrivate(const char *name, const int *val, int nval) {
  if (nval > 256) return;
  char buf[12*256]="";
  for (int i = 0; i < nval; i++) {
    STRCAT(buf, StringPrintf("%d", val[i]));
    if (i != nval-1) STRCAT(buf, ",");
  }
  public_config->setString(name, buf);
}

int api_configI::getIntArrayPrivate(const char *name, int *val, int nval) {
  char buf[12*256]="";
  public_config->getString(name, buf, sizeof(buf), "");
  PathParser pp(buf, ",");
  if (pp.getNumStrings() != nval) return 0;
  for (int i = 0; i < nval; i++) {
    *val = ATOI(pp.enumString(i));
    val ++;
  }
  return 1;
}

void api_configI::setStringPrivate(const char *name, const char *str) {
  public_config->setString(name, str);
}

int api_configI::getStringPrivate(const char *name, char *buf, int buf_len, const char *default_str) {
  int ret = public_config->getString(name, buf, buf_len, default_str);
  return ret;
}

int api_configI::getStringPrivateLen(const char *name) {
  return public_config->getStringLen(name);
}

void api_configI::setIntPublic(const char *name, int val) {
  public_config->setInt(name, val);
}

int api_configI::getIntPublic(const char *name, int def_val) {
  return public_config->getInt(name, def_val);
}

void api_configI::setStringPublic(const char *name, const char *str) {
  public_config->setString(name, str);
}

int api_configI::getStringPublic(const char *name, char *buf, int buf_len, const char *default_str) {
  return public_config->getString(name, buf, buf_len, default_str);
}
