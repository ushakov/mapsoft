#ifndef VIEWER_LAYER_WPTS_H
#define VIEWER_LAYER_WPTS_H

#include "gred/gobj.h"
#include "../layerlist.h"
#include "../workplane.h"


class LayerWPTs{
public:
  Workplane gobj; // tmp - use workplane to combine tracks
  WptLL    panel; // Right panel (and data storage), see layerlist.h
  GObj * get_gobj() {return &gobj;}
};

#endif
