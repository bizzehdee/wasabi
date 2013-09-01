#ifndef __SCRIPTVAR_H
#define __SCRIPTVAR_H

#ifdef __cplusplus
 class ScriptObject;
#endif

typedef struct {
  int type;    // basic type, see above
  union { // union of 4 bytes of different types 
    int idata;    // Integer
    float fdata;  // Float
    double ddata; // Double
#ifdef __cplusplus
    ScriptObject *odata;  // Object
#else
    void *odata;
#endif
    const char *sdata;  // String
  } data;
} scriptVar;

#endif
