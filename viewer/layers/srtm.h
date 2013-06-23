#ifndef VIEWER_LAYER_SRTM_H
#define VIEWER_LAYER_SRTM_H

#include "gred/gobj.h"
#include "../workplane.h"


class LayerSRTMs{
public:
  Workplane gobj;
  GObj * get_gobj() {return &gobj;}
};

#endif
