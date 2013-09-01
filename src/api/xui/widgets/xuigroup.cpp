#include <precomp.h>
#include <bfc/string/stringdict.h>
#include "../xuiinstantiator.h"
#include "api/wdf/wdf.h"
#include "xuigroup.h"

_DECLARE_XUISERVICE(XUIGroupSvc);

enum {
  GROUPXUIOBJECT_ATTRIBUTE_GID,
};

XUIGroup::XUIGroup() {
  m_content = NULL;
  m_inOnInit = 0;
}

XUIGroup::~XUIGroup() {
  ASSERT(m_content == NULL);
}

void XUIGroup::onDestroy() {
  if (m_content) removeChild(m_content);
  XUIGroup_PARENT::onDestroy();
  destroyContent();
}

BEGIN_STRINGDICTIONARY(_GROUPXUIOBJECT_ATTRIBUTES);
SDI("gid",       GROUPXUIOBJECT_ATTRIBUTE_GID);
END_STRINGDICTIONARY(_GROUPXUIOBJECT_ATTRIBUTES, groupXuiObjectAttributes);

void XUIGroup::onAttributeChanged(const char *attributeName, const char *attributeValue) {
  XUIGroup_PARENT::onAttributeChanged(attributeName, attributeValue);
  switch (groupXuiObjectAttributes.getId(attributeName)) {
    case GROUPXUIOBJECT_ATTRIBUTE_GID:
      if (m_wnd->isPostOnInit())
        setContent(attributeValue);
      break;
  }
}

void XUIGroup::onInit() {
  m_inOnInit = 1;
  XUIGroup_PARENT::onInit();
  setContent(getAttribute("gid"));
  m_inOnInit = 0;
}

void XUIGroup::setContent(const char *id) {
  if (m_content) {
    removeChild(m_content);
    destroyContent();
  }

  if (!id || !*id) return;

  // get the group definition, starting from this object, and going up the hierarchy. 
  DOMNode *groupdef = getLocalGroupDefinition(id);

  // if we hit the top without finding a match, try the framework.
  DefinitionsFramework *framework = getFramework();
  if (!groupdef) {
    groupdef = framework->getTopLevelNode("groupdef", id);
  }

  // if the group was still not found, abort loading content
  if (!groupdef) return;

  // instantiate the group
  XUIInstantiator instantiator(framework);

  m_content = instantiator.instantiateXUINode(groupdef, "group", false, NULL);

  // and init it if necessary
  if (m_content) {
    addChild(m_content);
    if (m_inOnInit) {
      initChild(m_content);
    }
  }
}

void XUIGroup::destroyContent() {
  if (m_content) {
    svc_xuiObject *svc = m_content->getOwnerService();
    if (svc == NULL) {
      DebugString("Owner service is NULL for XUIObject %s", getTagName());
    } else {
      svc->destroy(m_content);
    }
    m_content = NULL;
  }
}
