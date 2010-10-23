#ifndef OCAD_GEO_H
#define OCAD_GEO_H

#define USE_LIB2D
#include "2d/line_utils.h"

#include "geo/geo_data.h"
#include "geo/geo_convs.h"
#include "ocad_file.h"

namespace ocad{


g_map get_ref(const ocad_file & O){

  dLine pts = rect2line(O.range());

  std::vector<ocad_string>::const_iterator s;
  for (s=O.strings.begin(); s!=O.strings.end(); s++){
    if ((s->type == 1039) && (s->get_field('r') == "1")){
      double rscale  = s->get<double>('m');
      double grid    = s->get<double>('g');
      double grid_r  = s->get<double>('d');
      dPoint  p0(s->get<double>('x'),
                 s->get<double>('y'));
      double a = s->get<double>('a');
      int zone    = s->get<int>('i'); // grid and zone - "2037"

      Options opts;
      opts.put<int>("lon0", zone%1000);
      convs::pt2pt cnv(Datum("pulkovo"), Proj("tmerc"), opts,
                       Datum("wgs84"), Proj("lonlat"), opts);


      dLine pts0(pts);
      lrotate(pts, -a * M_PI/180);
      pts+=p0;
      cnv.line_frw_p2p(pts);

      g_map ret;
      for (int i = 0; i < pts.size(); i++){
        ret.push_back(g_refpoint(pts[i], pts0[i]));
      }
      ret.map_proj=Proj("tmerc");
      ret.border=pts;
      return ret;
    }
  }
  return g_map();
}

/*
void set_ref(const ocad_file & O, const g_ref & ref){
  double rscale  = ;
  double grid    = ;
  double grid_r  = ;
  double x       = ;
  double y       = ;
  double a       = ;
  double zone    = ;
}
*/


} // namespace
#endif
