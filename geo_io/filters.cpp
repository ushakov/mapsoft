#include "io.h"
#include "../utils/mapsoft_convs.h"

#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>


namespace filters{

using namespace std;


// вспомогательная функция, которая по названию листа возвращает диапазон геодезических
// координат в СК pulkovo-42

Rect<double> nom_range(const std::string & key){
    using namespace boost::spirit;

    char a  = ' ';
    int  b  = 0;
    string c5 = "";
    string c2 = "";
    string c1 = "";
    string d = "";
    // поймем номер листа
    if (!parse(key.c_str(), 
      alpha_p[assign_a(a)] >> !(ch_p('-')||'_') >> 
      uint_p[assign_a(b)] >> 
        !( (ch_p('-')||'_') >>
        ((digit_p >> space_p)[assign_a(c5)] ||
         (digit_p >> digit_p >> space_p)[assign_a(c2)] ||
         (digit_p >> digit_p >> digit_p >> space_p)[assign_a(c1)] ||
         ((digit_p >> digit_p >> digit_p)[assign_a(c1)] >>
          !( (ch_p('-')||'_') >> (digit_p)[assign_a(d)] ) >> space_p)
        )) >> *anychar_p).full) {
      std::cerr << "map_nom_brd: can't parse " << key << "\n";
      return Rect<double>(0,0,0,0);
    }
    
    if      ((a>='A')&&(a <= 'T')) a-='A';
    else if ((a>='a')&&(a <= 't')) a-='a';
    else {
      std::cerr << "map_nom_brd: can't parse " << key << " (" << a << ")\n";
      return Rect<double>(0,0,0,0);
    }

    double lat1,lat2,lon1,lon2;

    lat1 = a*4; lat2=lat1+4;

    if ((b<1)||(b>=60)) {
      std::cerr << "map_nom_brd: can't parse " << key << " (" << b << ")\n";
      return Rect<double>(0,0,0,0);
    }

    lon1 = b*6 - 186; lon2=lon1+6;

    int col,row;

    int c1i=0, c2i=0, c5i=0, di=0;
    parse(c1.c_str(), uint_p[assign_a(c1i)]);
    parse(c2.c_str(), uint_p[assign_a(c2i)]);
    parse(c5.c_str(), uint_p[assign_a(c5i)]);
    parse(d.c_str(),  uint_p[assign_a(di)]);

    if ((di != 0)&&(c1i != 0)){  // 1:50 000
      col = ((c1i-1)%12)*2 + (di-1)%2;
      row = 23 - ((c1i-1)/12)*2 - (di-1)/2;
      lon1 += col*6.0/24; lon2=lon1+6.0/24;
      lat1 += row*4.0/24; lat2=lat1+4.0/24;
//      std::cerr << "1:50 000, col: " << col << ", row: "<< row << '\n';
    }
    else if (c1i != 0){  // 1:100 000
      col = (c1i-1)%12;
      row = 11 - (c1i-1)/12;
      lon1 += col*6.0/12; lon2=lon1+6.0/12;
      lat1 += row*4.0/12; lat2=lat1+4.0/12;
//      std::cerr << "1:100 000, col: " << col << ", row: "<< row << '\n';
    }
    else if (c2i != 0){  // 1:200 000
      col = (c2i-1)%6;
      row = 5 - (c2i-1)/6;
      lon1 += col*6.0/6; lon2=lon1+6.0/6;
      lat1 += row*4.0/6; lat2=lat1+4.0/6;
//      std::cerr << "1:200 000, col: " << col << ", row: "<< row << '\n';
    }
    else if (c5i != 0){  // 1:500 000
      col = (c5i-1)%2;
      row = 1 - (c5i-1)/2;
      lon1 += col*6.0/2; lon2=lon1+6.0/2;
      lat1 += row*4.0/2; lat2=lat1+4.0/2;
//      std::cerr << "1:500 000, col: " << col << ", row: "<< row << '\n';
    }
    std::cerr << "lat: " << lat1 << ".." << lat2 <<'\n';
    std::cerr << "lon: " << lon1 << ".." << lon2 <<'\n';

    return Rect<double>(g_point(lon1,lat1), g_point(lon2,lat2));
}

void map_nom_brd(geo_data & world){
  vector<g_map>::iterator i;
  for (i = world.maps.begin(); i!=world.maps.end(); i++){

    Rect<double> r = nom_range(i->comm);
    if (r.empty()) return;
    double lon1 = r.x;
    double lat1 = r.y;
    double lon2 = lon1 + r.w;
    double lat2 = lat1 + r.h;
    
    convs::map2pt conv(*i, Datum("pulk42"), Proj("lonlat"), Options());

    vector<g_point> brd;
    brd.push_back(g_point(lon1,lat2));
    brd.push_back(g_point(lon2,lat2));
    brd.push_back(g_point(lon2,lat1));
    brd.push_back(g_point(lon1,lat1));
    brd.push_back(g_point(lon1,lat2));
    i->border = conv.line_bck(brd);
  }
}


bool wpt_srt_com_test(const g_waypoint & p1, const g_waypoint & p2){
}

void wpt_srt_com(geo_data & world){
  vector<g_waypoint_list>::iterator i;
  for (i = world.wpts.begin(); i!=world.wpts.end(); i++){

//14-OCT-06 11:08:57

  }
}
}//namespace