#ifndef VIEWER_LAYER_MAPS_H
#define VIEWER_LAYER_MAPS_H

#include "gred/gobj.h"
#include "maps_panel.h"
#include "../workplane.h"

class LayerMAPs{
public:
  Workplane gobj; // tmp - use workplane to combine tracks
  MapLL    panel; // Right panel (and data storage), see layerlist.h
  GObj * get_gobj() {return &gobj;}
  void clear(){
    gobj.clear();
    panel.clear();
  }
};

#endif