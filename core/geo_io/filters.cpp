#include "io.h"
#include "geo/geo_convs.h"
#include "geo/geo_nom.h"

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>


namespace filters{

using namespace std;

void map_nom_brd(geo_data & world){
  vector<g_map_list>::iterator ml_i;
  vector<g_map>::iterator i;
  for (ml_i = world.maps.begin(); ml_i!=world.maps.end(); ml_i++){
    for (i = ml_i->begin(); i!=ml_i->end(); i++){

      dRect r = convs::nom_to_range(i->comm);
      if (r.empty()) return;
      double lon1 = r.x;
      double lat1 = r.y;
      double lon2 = lon1 + r.w;
      double lat2 = lat1 + r.h;

      convs::map2pt conv(*i, Datum("pulk42"), Proj("lonlat"));

      dLine brd;
      brd.push_back(dPoint(lon1,lat2));
      brd.push_back(dPoint(lon2,lat2));
      brd.push_back(dPoint(lon2,lat1));
      brd.push_back(dPoint(lon1,lat1));
      brd.push_back(dPoint(lon1,lat2));
      i->border = conv.line_bck(brd);
    }
  }
}


void
generalize(g_track * line, double e, int np){
  // какие точки мы хотим исключить:
  std::vector<bool> skip(line->size(), false);

  Options o;
  o.put("lon0", convs::lon2lon0((*line)[0].x));
  convs::pt2pt cnv(Datum("wgs84"), Proj("tmerc"), o,
                       Datum("wgs84"), Proj("lonlat"), Options());

  dLine gk_line=(dLine)(*line);
  cnv.line_bck_p2p(gk_line);

  np-=2; // end points are not counted
  while (1){
    // для каждой точки найдем расстояние от нее до
    // прямой, соединяющей две соседние (не пропущенные) точки.
    // найдем минимум этой величины
    double min=-1;
    int mini; // index of point with minimal deviation
    int n=0;
    for (int i=1; i<int(gk_line.size())-1; i++){
      if (skip[i]) continue;
      n++; // count point we doesn't plan to skip
      int ip, in; // previous and next indexes
      // skip[0] and skip[line.size()-1] are always false
      for (ip=i-1; ip>=0; ip--)            if (!skip[ip]) break;
      for (in=i+1; in<gk_line.size()-1; in++) if (!skip[in]) break;
      Point<double> p1 = gk_line[ip];
      Point<double> p2 = gk_line[i];
      Point<double> p3 = gk_line[in];
      double ll = pdist(p3-p1);
      dPoint v = (p3-p1)/ll;
      double prj = pscal(v, p2-p1);
      double dp;
      if      (prj<=0)  dp = pdist(p2,p1);
      else if (prj>=ll) dp = pdist(p2,p3);
      else              dp = pdist(p2-p1-v*prj);
      if ((min<0) || (min>dp)) {min = dp; mini=i;}
    }
    if (n<=2) break;
    // если этот минимум меньше e или точек в линии больше np - выкинем точку
    if ( ((e>0) && (min<e)) ||
         ((np>0) && (n>np))) skip[mini]=true;
    else break;
  }
  g_track::iterator i = line->begin();
  int j=0;
  while (i != line->end()){
    if (skip[j]) i = line->erase(i);
    else i++;
    j++;
  }
}

}//namespace
