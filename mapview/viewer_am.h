#ifndef VIEWER_AM_H
#define VIEWER_AM_H

// ActionManager для DataList

class Mapview;

class ViewerAM{
  boost::shared_ptr<Viewer>  viewer;
  boost::shared_ptr<Mapview> mapview;

  public:
  ViewerAM(boost::shared_ptr<Viewer> viewer_, 
           boost::shared_ptr<Mapview> mapview_):
    viewer(viewer_), mapview(mapview_){
  }
};

#endif
