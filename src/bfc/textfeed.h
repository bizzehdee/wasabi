#ifndef _TEXTFEED_H
#define _TEXTFEED_H

#include <bfc/common.h>
#include <bfc/map.h>
#include <bfc/depend.h>
#include <api/service/svcs/svc_textfeed.h>

/**
  This is the standard helper class for implementing a textfeed.
  Be sure to check out class EzFeed in ezfeed.h, which combines this
  class with a service factory.
*/
class COMEXP TextFeed : public svc_textFeedI, public DependentI {
public:
/**
  Call this to register your feeds by id. Make the ids unique!
  @see sendFeed()
  @ret TRUE if succeeded, FALSE on error (i.e. nonunique id)
*/
  int registerFeed(const char *feedid, const char *initial_text="", const char *description="");

/**
  Call this to send text into a feed.
  @see registerFeed()
  @ret TRUE if succeeded, FALSE on error (i.e. feedid not registered)
*/
  int sendFeed(const char *feedid, const char *text);

/**
  Gives the most recently sent text on a feed.
*/
  virtual const char *getFeedText(const char *feedid);

/**
  Gives a description for the feed (used by accessibility).
*/
  virtual const char *getFeedDescription(const char *feedid);

protected:
  virtual Dependent *getDependencyPtr() { return this; }
  virtual void dependent_onRegViewer(DependentViewer *viewer, int add);
  virtual void *dependent_getInterface(const GUID *classguid);

/**
  Called when someone subscribes to this feed.
*/
  virtual void onRegClient() { }
  virtual void onDeregClient() { }

  virtual int hasFeed(const char *name);

private:
  Map<String, Pair<String, String> > feeds;
};

#endif
