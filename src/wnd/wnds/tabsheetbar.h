#ifndef __TABSHEETBAR_H
#define __TABSHEETBAR_H

//#include "wnd/wnds/guiobjwnd.h"
#include "wnd/clickwnd.h"

class GroupTabButton;

#define TABSHEETBAR_PARENT ClickWnd //GuiObjectWnd


/**
  Class 

  @short 
  @author Nullsoft
  @ver 1.0
  @see 
  @cat 
*/
class TabSheetBar : public TABSHEETBAR_PARENT {

  public:

    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    TabSheetBar();
    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    virtual ~TabSheetBar();

    
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
    virtual int onResize();
    
    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    virtual void addChild(GroupTabButton *child);
    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    virtual int getHeight();

    
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
    void setMargin(int m) { margin = m; if (isInited()) onResize(); }
    
    /**
      Method
    
      @see 
      @ret 
      @param 
    */
    void setSpacing(int s) { spacing = s; if (isInited()) onResize(); }

  private:

    int maxheightsofar;
    PtrList<GroupTabButton> btns;
    int margin, spacing;
    BaseWnd *bottombar;
};

#endif
