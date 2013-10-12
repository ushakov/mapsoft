#ifndef VIEWER_LAYER_TRKS_H
#define VIEWER_LAYER_TRKS_H

#include "trks_panel.h"
#include "../workplane.h"

class LayerTRKs{
public:
  Workplane gobj; // tmp - use workplane to combine tracks
  TrkLL    panel; // Right panel (and data storage), see layerlist.h
  GObjGeo * get_gobj() {return &gobj;}
  void clear(){
    gobj.clear();
    panel.clear();
  }
};

#endif
