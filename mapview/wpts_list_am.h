#ifndef WPTS_LIST_AM_H
#define WPTS_LIST_AM_H

// ActionManager для DataList

#include "mapview.h"

class WPTSListAM{
  boost::shared_ptr<WPTSList> wpts_list;
  boost::shared_ptr<Mapview>  mapview;

  public:
  WPTSListAM(boost::shared_ptr<WPTSList> wpts_list_, 
             boost::shared_ptr<Mapview> mapview_):
    wpts_list(wpts_list_), mapview(mapview_){
  }
};

#endif
