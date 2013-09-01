#include "precomp.h"
#include "skinclr.h"

#include "api_skin.h"

int *SkinColor::skin_iterator=0;

SkinColor::SkinColor(const char *_name, const char *colorgroup) : FilteredColor(0, (colorgroup == NULL || !*colorgroup) ? "Text" : colorgroup) {
  latest_iteration = -1;
  skin_iterator = NULL;
  setElementName(_name);
}

DWORD SkinColor::v() {
  if (name == NULL) return 0;
#ifdef WASABI_COMPILE_SKIN
  // fetch iterator pointer if necessary
  if (skin_iterator == NULL) {
    skin_iterator = WASABI_API_SKIN->skin_getIterator();
    ASSERT(skin_iterator != NULL);
  }

  // see if we're current
  if (*skin_iterator != latest_iteration) {
    val = NULL;	// pointer now invalid, must re-get
    latest_iteration = *skin_iterator;	// and then we'll be current
  }

  // new pointer please
  if (val == NULL) {
    setColor(WASABI_API_SKIN->skin_getColorElement((char *)name));
    val = getColorRef();
  }
#endif
  if (val == NULL) return 0xFFFFFFFF;

  return *val;
}

void SkinColor::setElementName(const char *_name) {
  name = _name;
  val = NULL;
  latest_iteration = 0;
}

