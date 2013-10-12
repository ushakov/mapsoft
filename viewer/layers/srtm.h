#ifndef VIEWER_LAYER_SRTM_H
#define VIEWER_LAYER_SRTM_H

#include "../workplane.h"

class LayerSRTMs{
public:
  Workplane gobj;
  GObjGeo * get_gobj() {return &gobj;}
};

#endif
