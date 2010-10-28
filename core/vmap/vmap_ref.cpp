#include "vmap.h"
#include "geo/geo_data.h"
#include "2d/line_utils.h"

using namespace std;
namespace vmap{

g_map get_tmerc_rec(const world & W, double u_per_cm, bool yswap){

  g_map ref;

  // create ref from brd or from map range
  dLine refs=W.brd;
  if (refs.size()<3) refs=rect2line(W.range());

  // add last = first if needed
  if ((refs.size()>0) && (refs[0]!=refs[refs.size()-1])) refs.push_back(refs[0]);

  // reduce refs to 5 points, remove last one
  refs=generalize(refs, -1, 5);
  if (refs.size()>4) refs.resize(4);

  Options PrO;
  PrO.put<double>("lon0", convs::lon2lon0(refs.center().x));
  convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), PrO,
                   Datum("wgs84"), Proj("tmerc"), PrO);

  dLine refs_r(refs);
  cnv.line_frw_p2p(refs_r);
  refs_r *= 100.0/W.rscale * u_per_cm; // to out units
  refs_r -= refs_r.range().TLC();
  double h = refs_r.range().h;

  if (yswap){
    for (int i=0;i<refs_r.size();i++)
      refs_r[i].y = h - refs_r[i].y;
  }

  for (int i=0;i<refs.size();i++){
    ref.push_back(g_refpoint(refs[i], refs_r[i]));
  }

  ref.border = rect2line(refs_r.range());
  ref.map_proj=Proj("tmerc");
  ref.comm=W.name;

  // set ref.border to W.brd in pixels if it is not empty;
  if (W.brd.size()>2){
    convs::map2pt cnv1(ref, Datum("wgs84"), Proj("lonlat"), Options());
    ref.border = generalize(cnv1.line_bck(W.brd), 1, -1); // 1 unit accuracy
  }

  return ref;
}

} // namespace

