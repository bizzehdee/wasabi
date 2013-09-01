#ifndef _API_JSI_H__
#define _API_JSI_H__

#include <precomp.h>
#include <bfc/ptrlist.h>

/*<?<autoheader/>*/
#include "api_js.h"
#include "api_jsX.h"
/*?>*/

/*[interface.header.h]
#if defined(WASABI_PLATFORM_WIN32) && !defined(XP_WIN) 
#define XP_WIN
#else 
#if defined(WASABI_PLATFORM_LINUX) && !defined(XP_UNIX) 
#define XP_UNIX
#endif
#endif
#include <jsapi.h>
*/

/*<?<classdecl name="api_js" implname="api_jsI" factory="Interface" dispatchable="1"/>*/
class api_jsI : public api_jsX {
/*?>*/
public:
	NODISPATCH api_jsI();
	NODISPATCH virtual ~api_jsI();

  static const char *getServiceName() { return "JavaScript API"; }
  static GUID getServiceType() { return WaSvc::UNIQUE; }

  DISPATCH(100) JSContext *createNewContext();
  DISPATCH(110) JSContext *enumContext(int n);
  DISPATCH(120) int deleteContext(int n);
  DISPATCH(130) int deleteContext(JSContext *ctx);
  DISPATCH(170) JSContext *getMainContext();
  DISPATCH(200) void collectGarbage(JSContext *ctx);

  DISPATCH(140) JSObject *createNewObject(JSContext *cx, JSClass *cls, JSObject *parent=NULL);
  DISPATCH(150) JSObject *enumObject(int n);
  DISPATCH(180) JSObject *getGlobalObject();

  DISPATCH(160) void assignFunctions(JSContext *cx, JSObject *classobj, JSFunctionSpec *spec);

  DISPATCH(1000) JSScript *compileScript(JSContext *cx, JSObject *obj, const char *filename);
  DISPATCH(1001) JSScript *compileScript(JSContext *cx, JSObject *obj, const char *bytes, long len, const char *filename=0);
  DISPATCH(1010) JSObject *createScriptObject(JSContext *cx, JSScript *script);
  DISPATCH(1020) void createNamedRoot(JSContext *cx, JSObject *scriptobject, const char *name);
  DISPATCH(1030) void deleteNamedRoot(JSContext *cx, JSObject *scriptobject);

  DISPATCH(1500) bool executeScript(JSContext *cx, JSScript *script, jsval *rval);
  DISPATCH(1510) bool executeFunction(JSContext *cx, JSObject *script, const char *name, int argn, jsval *args, jsval *rval);
  DISPATCH(1520) bool evalScript(JSContext *cx, JSObject *obj, const char *bytes, long len, jsval *retval, const char *filename=0);

  DISPATCH(2000) int jsvalToInt(JSContext *cx, jsval *v);
  DISPATCH(2010) double jsvalToDouble(JSContext *cx, jsval *v);
  DISPATCH(2020) const char *jsvalToString(JSContext *cx, jsval *v);
  DISPATCH(2030) bool jsvalToBool(JSContext *cx, jsval *v);
  DISPATCH(2040) void jsvalToObject(JSContext *cx, jsval *v, JSObject **obj);

  DISPATCH(2500) jsval intToJSval(JSContext *cx, int v);
  DISPATCH(2510) jsval doubleToJSval(JSContext *cx, double v);
  DISPATCH(2520) jsval stringToJSval(JSContext *cx, const char *v);
  DISPATCH(2530) jsval boolToJSval(JSContext *cx, bool v);
  DISPATCH(2540) jsval objectToJSval(JSContext *cx, JSObject *obj);

private:
  JSRuntime *rt;
  PtrList<JSContext> ctxs;
  JSObject *globalObj;
  PtrList<JSObject> objs;
};

/*[interface.footer.h]
// {FF7DBA7F-480E-4e75-8A84-AD0F71D01E24}
static const GUID JSApiServiceGuid = 
{ 0xff7dba7f, 0x480e, 0x4e75, { 0x8a, 0x84, 0xad, 0xf, 0x71, 0xd0, 0x1e, 0x24 } };

extern api_js *jsApi;
*/

#endif // !_API_JSI_H__
