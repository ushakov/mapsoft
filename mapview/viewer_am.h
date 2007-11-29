#ifndef VIEWER_AM_H
#define VIEWER_AM_H

// ActionManager для DataList

#include "mapview.h"

class Mapview;

class ViewerAM{
  boost::shared_ptr<Viewer> viewer;
  Mapview * mapview;

  public:
  ViewerAM(boost::shared_ptr<Viewer> viewer_, Mapview * mapview_):
    viewer(viewer_), mapview(mapview_){
  }
};

#endif
