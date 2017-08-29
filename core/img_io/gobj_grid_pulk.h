#ifndef GOBJ_GRID_PULK_H
#define GOBJ_GRID_PULK_H

#include "gobj_geo.h"
#include "geo/geo_data.h"
#include "cache/cache.h"
#include "utils/cairo_wrapper.h"


/// Pulkovo-1941 grid

class GObjGridPulk : public GObjGeo{
public:
  GObjGridPulk (const Options & o = Options()): convs(8) { set_opt(o); }
  int draw(iImage & image, const iPoint & origin);
  Cache<int, convs::map2pt> convs; // vs lon0

  void set_ref(const g_map & ref_){convs.clear(); GObjGeo::set_ref(ref_);}
};


#endif
