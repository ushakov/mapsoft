#include <list>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "2d/line_utils.h"
#include "2d/line_rectcrop.h"
#include "geo_io/geofig.h"
#include "geo/geo_convs.h"
#include "geo/geo_nom.h"
#include "vmap/zn.h"
#include "vmap.h"

namespace vmap {

using namespace std;

/***************************************/

void
set_source(Options & o, const string & source){
  if (source != "") o.put<string>("Source", source);
  else o.erase("Source");
}

dPoint
v_m2deg(const dPoint & v, const double lat){
  dPoint ret(v/6380000.0 * 180/M_PI);
  ret.x /= cos(M_PI/180.0 * lat);
  return ret;
}
dPoint
v_deg2m(const dPoint & v, const double lat){
  dPoint ret(v * 6380000 * M_PI/180);
  ret.x *= cos(M_PI/180.0 * lat);
  return ret;
}

double
dist_pt(const dPoint & v, double lat){
  return pdist(v_deg2m(v,lat));
}
double
dist_pt_pt(const dPoint & p1, const dPoint & p2){ // approximate distance in m
  return  pdist(v_deg2m(p2-p1,(p2.y+p1.y)/2.0));
}
double
dist_pt_l(const dPoint & p, const dMultiLine & l, dPoint & n){
  double ret = 1e99;
  for (dMultiLine::const_iterator i=l.begin(); i!=l.end(); i++){
    for (dLine::const_iterator j=i->begin(); j!=i->end(); j++){
      double d = dist_pt_pt(p, *j);
      if (ret > d){ ret=d; n=*j; }
    }
  }
  return ret;
}

} // namespace
