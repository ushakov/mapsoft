#ifndef VIEWER_LAYER_WPTS_H
#define VIEWER_LAYER_WPTS_H

#include "wpts_panel.h"
#include "../workplane.h"


class LayerWPTs{
public:
  Workplane gobj; // tmp - use workplane to combine tracks
  WptLL    panel; // Right panel (and data storage), see layerlist.h
  GObjGeo * get_gobj() {return &gobj;}
  void clear(){
    gobj.clear();
    panel.clear();
  }
};

#endif
