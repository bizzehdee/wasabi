#include <precomp.h>
#include <bfc/textfeed.h>
#include <bfc/pair.h>

int TextFeed::registerFeed(const char *feedid, const char *initial_text, const char *description) {
  if (feeds.getItem(String(feedid))) return FALSE;
  Pair<String, String> pair(initial_text, description);
  feeds.addItem(String(feedid), pair);

  dependent_sendEvent(svc_textFeed::depend_getClassGuid(), Event_TEXTCHANGE, (int)feedid, (void*)initial_text, STRLEN(initial_text)+1);
  return TRUE;
}

int TextFeed::sendFeed(const char *feedid, const char *text) {
  Pair <String, String> ft("","");
  if (!feeds.getItem(String(feedid), &ft)) {
//CUT    ASSERTALWAYS("hey, you're trying to send a feed you didn't register. stop it.");
    DebugString("TextFeed::sendFeed(), feedid '%s' not registered", feedid);
    return FALSE;
  }
  String id(feedid);
  feeds.getItem(id, &ft);
  ft.a = String(text);
  feeds.setItem(String(feedid), ft);
  dependent_sendEvent(svc_textFeed::depend_getClassGuid(), Event_TEXTCHANGE, (int)feedid, (void*)text, STRLEN(text)+1);
  return TRUE;
}

const char *TextFeed::getFeedText(const char *name) {
  const Pair<String, String> *ft = feeds.getItemRef(String(name));
  if (ft == NULL) return NULL;
  return ft->a.getValue();
}

const char *TextFeed::getFeedDescription(const char *name) {
  const Pair<String, String> *ft = feeds.getItemRef(String(name));
  if (ft == NULL) return NULL;
  return ft->b.getValue();
}

int TextFeed::hasFeed(const char *name) {
  return feeds.getItem(name);
}

void TextFeed::dependent_onRegViewer(DependentViewer *viewer, int add) {
  if (add) {
    for (int i = 0; i < feeds.getNumItems(); i++) {
      String a = feeds.enumIndexByPos(i, String(""));
      Pair<String, String> sp("","");
      String b = feeds.enumItemByPos(i, sp).a;
      dependent_sendEvent(svc_textFeed::depend_getClassGuid(), Event_TEXTCHANGE, (int)a.getValue(), (void*)b.getValue(), b.len()+1, viewer);//send to this viewer only
    }
  }

  if (add) onRegClient();
  else onDeregClient();
}

void *TextFeed::dependent_getInterface(const GUID *classguid) {
  HANDLEGETINTERFACE(svc_textFeed);
  return NULL;
}
