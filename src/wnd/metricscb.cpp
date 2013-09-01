#include "precomp.h"

#include "metricscb.h"

int MetricsCallbackI::mcb_setMetric(int metricid, int param1, int param2) {
  switch (metricid) {
    case Metric::TEXTDELTA:
      return metricscb_setTextDelta(param1);
  }
  return 0;
}

#define CBCLASS MetricsCallbackI
START_DISPATCH;
  CB(SETMETRIC, mcb_setMetric);
END_DISPATCH;
#undef CBCLASS
