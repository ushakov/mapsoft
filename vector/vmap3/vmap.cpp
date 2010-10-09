#include <list>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "lib2d/line_utils.h"
#include "lib2d/line_rectcrop.h"
#include "libgeo_io/geofig.h"
#include "libgeo/geo_convs.h"
#include "libgeo/geo_nom.h"
#include "../libzn/zn.h"
#include "vmap.h"

namespace vmap {

using namespace std;

const int label_type = 0x1000FF;
const int border_type = 0x1000FE;

const double default_rscale = 50000;
const string default_style = "default";

/***************************************/

object_class
object::get_class() const{
  if (type & zn::area_mask) return POLYGON;
  if (type & zn::line_mask) return POLYLINE;
  return POI;
}

dRect
world::range() const{
  if (this->size()<1) return dRect(0,0,0,0);
  world::const_iterator i=this->begin();
  dRect ret=i->range();
  while ((++i) != this->end())
    ret = rect_bounding_box(ret, i->range());
  return ret;
}

void
world::add(const world & W){
  mp_id  = W.mp_id;
  name   = W.name;
  style  = W.style;
  rscale = W.rscale;
  brd    = W.brd;
  insert(end(), W.begin(), W.end());
  lbuf.insert(lbuf.end(), W.lbuf.begin(), W.lbuf.end());
}

} // namespace
