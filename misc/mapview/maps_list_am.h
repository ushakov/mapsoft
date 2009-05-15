#ifndef MAPS_LIST_AM_H
#define MAPS_LIST_AM_H

// ActionManager для DataList

#include "mapview.h"

class MAPSListAM{
  boost::shared_ptr<MAPSList> maps_list;
  boost::shared_ptr<Mapview>  mapview;

  public:
  MAPSListAM(boost::shared_ptr<MAPSList> maps_list_, 
             boost::shared_ptr<Mapview>  mapview_):
    maps_list(maps_list_), mapview(mapview_){
  }
};

#endif
