#include <precomp.h>
#include "widgets/xuiobject.h"
#include "xuidocument.h"
#include "xuijsscript.h"

void XUIJSScript::appendCode(const char *text, const char *file, int line) {
  if (m_code.isempty()) {
    m_file = file;
    m_line = line;
  }

  m_code += text;
}

void XUIJSScript::onReady(XUIObject *topLevel, const char *objectName) {
  // Compile the script
  load(m_code, m_code.len(), m_file, m_line);

  // Install the top level XUI object as a global js object called 'window'
  if (topLevel) {
    jsval obj = OBJECT_TO_JSVAL(topLevel->js_getJSObject(getContext()));
    JS_SetProperty(getContext(), getGlobalObject(), objectName, &obj);
  }

  // And execute.
  execute();
}

void XUIJSScript::onReady(PtrList<JSObjectDef> &defs) {
  // Compile the script
  load(m_code, m_code.len(), m_file, m_line);

  // Install the top level XUI object as a global js object called 'window'
  foreach(defs)
    JSObjectDef *def = defs.getfor();
    jsval obj = OBJECT_TO_JSVAL(def->getObject()->js_getJSObject(getContext()));
    JS_SetProperty(getContext(), getGlobalObject(), def->getName(), &obj);
  endfor;

  // And execute.
  execute();
}

void XUIJSScript::setAttribute(const char *attrname, const char *attrvalue) {
  // load scripts provided as external files and specified via xml attribute
  if (STRCASEEQL(attrname, "file")) {
    String path = WASABI_API_APP->path_getAppPath();
    if (!Std::isDirChar(path.lastChar())) path += DIRCHARSTR;
    path += attrvalue;
    FILE *script = FOPEN(path, "rt");
    if (!script) return;
    FSEEK(script, 0, SEEK_END);
    int size = FTELL(script);
    MemBlock<char> mem;
    mem.setSize(size);
    FREAD(mem.getMemory(), size, 1, script);
    FCLOSE(script);
    appendCode(mem.getMemory(), path, 0);
  }
}

