#include "ocad_geo.h"

#include "2d/line_utils.h"
#include "geo/geo_convs.h"
#include <sstream>

using namespace std;

namespace ocad{

#define OCAD_SCALE_PAR 1039

g_map
get_ref(const ocad_file & O){

  dLine pts = rect2line(O.range());

  vector<ocad_string>::const_iterator s;
  for (s=O.strings.begin(); s!=O.strings.end(); s++){
    if ((s->type == OCAD_SCALE_PAR) && (s->get_str('r') == "1")){
      try{
        double rscale  = s->get<double>('m');
        double grid    = s->get<double>('g');
        double grid_r  = s->get<double>('d');
        dPoint  p0(s->get<double>('x'),
                   s->get<double>('y'));
        double a = s->get<double>('a');
        int zone    = s->get<int>('i'); // grid and zone - "2037"

        Options opts;
        opts.put<int>("lon0", (zone%1000-30)*6-3);
        convs::pt2wgs cnv(Datum("pulkovo"), Proj("tmerc"), opts);

        dLine pts0(pts);
        lrotate(pts, -a * M_PI/180);
        pts*=rscale / 100000; // 1point = 0.01mm
        pts+=p0;
        cnv.line_frw_p2p(pts);
        g_map ret;
        for (int i = 0; i < pts.size(); i++){
          ret.push_back(g_refpoint(pts[i], pts0[i]));
        }
        ret.map_proj=Proj("tmerc");
        ret.border=pts0;
        return ret;
      }
      catch(boost::bad_lexical_cast x){
        std::cerr << "can't get reference: " << x.what() << "\n";
        return g_map();
      }
    }
  }
  return g_map();
}

void
set_ref(ocad_file & O, double rscale, const dPoint & p0){

  // remove old value;
  vector<ocad_string>::iterator si = O.strings.begin();
  while (si!=O.strings.end()){
    if (si->type == OCAD_SCALE_PAR) si = O.strings.erase(si);
    else si++;
  }

  Options opts;
  opts.put<int>("lon0", convs::lon2lon0(p0.x));
  convs::pt2wgs cnv(Datum("pulkovo"), Proj("tmerc"), opts);
  dPoint pc(p0);
  cnv.bck(pc);

  // add new string
  int grid=1000;
  ostringstream str;
  str << "\tm" << int(rscale)
      << "\tg" << grid   // grid, 0.01mm units
      << "\tr" << "1"    // geo reference is on
      << "\tx" << int(pc.x)
      << "\ty" << int(pc.y)
      << "\ta" << "0"      // angle, deg
      << "\td" << (grid*rscale)/100000 // grid, 1m units
      << "\ti" << convs::lon2pref(p0.x) + 2030;
                  // I don't know that is 2030...

  ocad_string s;
  s.type = OCAD_SCALE_PAR;
  s.data = str.str();
  O.strings.push_back(s);
}


} // namespace
