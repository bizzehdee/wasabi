#ifndef __GROUPTGBUTTON_H
#define __GROUPTGBUTTON_H

#include "groupclickwnd.h"
//#include "wnd/wnds/guiobjwnd.h"

#define GROUPTOGGLEBUTTON_PARENT ClickWnd

#define STATUS_OFF 0
#define STATUS_ON  1


/**
  Class 

  @short 
  @author Nullsoft
  @ver 1.0
  @see 
  @cat 
*/
class COMEXP GroupToggleButton : public GROUPTOGGLEBUTTON_PARENT {

  public:

    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    GroupToggleButton();
    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    virtual ~GroupToggleButton();

    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    virtual int onInit();
    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    virtual int childNotify(RootWnd *child, int msg, int param1=0, int param2=0);

    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    virtual void setGroups(const char *on, const char *off);

    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    virtual void toggle();
    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    virtual int wantFullClick();

    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    virtual void grouptoggle_onLeftPush();
    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    virtual void grouptoggle_onRightPush();

    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    virtual void setStatus(int s);
    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    virtual int getStatus() { return status; }
    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    virtual int wantAutoToggle() { return 1; }

    virtual GroupClickWnd *enumGroups(int n);
    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    virtual int getNumGroups();

  private:

    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    void initGroups();
    
    GroupClickWnd *on;
    GroupClickWnd *off;

    String on_id, off_id;

    int status;
};

#endif
