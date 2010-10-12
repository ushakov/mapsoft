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
// input angle conversions:
//                           ll p1,p2 -> ll angle   : mp labels
//              fig p1,p2 -> ll p1,p2 -> ll angle   : fig object labels
// fig angle -> fig p1,p2 -> ll p1,p2 -> ll angle   : fig text labels
double
ang_pll2a(const dPoint & p1, const dPoint & p2, int dir){
  dPoint v = (dir == 2)? p1-p2 : p2-p1;
  if (dir == 2) v=-v;
  return atan2(v.y, v.x);
}
double
ang_pfig2a(const dPoint & p1, const dPoint & p2, int dir, Conv & cnv){
  dPoint p1l(p1), p2l(p2);
  cnv.frw(p1l); cnv.frw(p2l);
  return ang_pll2a(p1l, p2l, dir);
}
double
ang_afig2a(double afig, int dir, Conv & cnv, dPoint fig_pos){
  double llen = label_len * fig::cm2fig;
  dPoint dp = llen * dPoint(cos(afig), -sin(afig));
  return ang_pfig2a(fig_pos, fig_pos+dp, dir, cnv);
}
/***************************************/
// output angle conversions
// ll angle -> ll dp                                : mp labels
// ll angle -> ll dp -> fig dp                      : fig object labels
// ll angle -> ll dp -> fig dp -> fig a             : fig text labels
dPoint
ang_a2pll(double a, int dir, dPoint pos, const double rscale){
  dPoint v(cos(a), sin(a));
  v*=pdist(v_m2deg(rscale / 100.0 * label_len * v, pos.y));
  v*=(dir == 2)? -1.0:1.0;
  return pos+v;
}
dPoint
ang_a2pfig(double a, int dir, Conv & cnv, dPoint fig_pos, const double rscale){
  dPoint pos(fig_pos); cnv.frw(pos);
  dPoint ret = ang_a2pll(a, dir, pos, rscale);
  cnv.bck(ret);
  return ret;
}
double
ang_a2afig(double a, Conv & cnv, dPoint fig_pos, const double rscale){
  dPoint v = ang_a2pfig(a, 0, cnv, fig_pos, rscale) - fig_pos;
  return atan2(-v.y, v.x);
}

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
