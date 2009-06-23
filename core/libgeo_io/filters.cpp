#include "io.h"
#include "../libgeo/geo_convs.h"
#include "../libgeo/geo_nom.h"

#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>


namespace filters{

using namespace std;

void map_nom_brd(geo_data & world){
  vector<g_map>::iterator i;
  for (i = world.maps.begin(); i!=world.maps.end(); i++){

    dRect r = convs::nom_range(i->comm);
    if (r.empty()) return;
    double lon1 = r.x;
    double lat1 = r.y;
    double lon2 = lon1 + r.w;
    double lat2 = lat1 + r.h;

    convs::map2pt conv(*i, Datum("pulk42"), Proj("lonlat"));

    g_line brd;
    brd.push_back(g_point(lon1,lat2));
    brd.push_back(g_point(lon2,lat2));
    brd.push_back(g_point(lon2,lat1));
    brd.push_back(g_point(lon1,lat1));
    brd.push_back(g_point(lon1,lat2));
    i->border = conv.line_bck(brd);
  }
}


}//namespace
