#ifndef TRKS_LIST_AM_H
#define TRKS_LIST_AM_H

// ActionManager для DataList

#include "mapview.h"

class TRKSListAM{
  boost::shared_ptr<TRKSList> trks_list;
  boost::shared_ptr<Mapview>  mapview;

  public:
  TRKSListAM(boost::shared_ptr<TRKSList> trks_list_, 
             boost::shared_ptr<Mapview> mapview_):
    trks_list(trks_list_), mapview(mapview_){
  }
};

#endif
