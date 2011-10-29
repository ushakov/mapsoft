#include <2d/rainbow.h>
#include "map.h"
#include "pnm.h"
#include "font.h"

inline int dms(int d, int m, int s) {return d*3600+m*60+s;}

// координаты углов
  const int lat1 = dms(51,00,00)/3; // данные задаем в секундах, деленных на три
  const int lat2 = dms(51,10,00)/3;
  const int lon1 = dms(103,00,00)/3;
  const int lon2 = dms(103,10,00)/3;


main(){

  map m(lat1, lon1, lat2, lon2);

  font f(FONT_4x6D);
  std::set<iPoint> S = f.print(lat1+20, lon1+20, "123 456.789.0asd");

  iPoint p = iPoint(lat1+100, lon1+100);

  simple_rainbow R(500,3000);
  print_pnm_head(m.w, m.h);

  for (int lat=m.lat2-1; lat>=m.lat1; lat--){
    for (int lon=m.lon1; lon<m.lon2; lon++){

      short h  = m.geth(lat,lon);
      int c = 0;
      if (h > srtm_min) c = R.get(h);

      if (S.count(iPoint(lat,lon))) c=0xFF0000;
      print_pnm_col(c);
    }
  }

}

