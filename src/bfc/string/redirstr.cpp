#include "precomp.h"

#include "redirstr.h"

#include <api/service/svcs/svc_redir.h>

// this defines the most redirects we'll put up with.
#define MAX_REDIRECT 256

RedirString::RedirString(const char *val, const char *domain) {
  if (val == NULL) return;
  String fn = val;
  for (int done = 0, c = 0; !done && c < MAX_REDIRECT; done = 1, c++) {
    RedirectEnum re(fn, domain);
    svc_redirect *svc;
    while ((svc = re.getNext(FALSE)) != NULL) {
      char buf[WA_MAX_PATH]="";
      if (svc->redirect(fn, domain, buf, sizeof(buf))) {
        fn = buf;
        done = 0;
      }
      re.getLastFactory()->releaseInterface(svc); svc = NULL;
    }
  }
  setValue(fn);
}
