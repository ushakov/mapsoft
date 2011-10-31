#include <2d/rainbow.h>
#include <srtm/map_tracer.h>
#include "pnm.h"

inline int dms(int d, int m, int s) {return d*3600+m*60+s;}

  // координаты углов
  const int lat1 = dms(53,50,00)/3; // данные задаем в секундах, деленных на три
  const int lat2 = dms(54,10,00)/3;
  const int lon1 = dms(95,00,00)/3;
  const int lon2 = dms(95,30,00)/3;

main(){

  map_tracer m(lat1, lon1, lat2, lon2);
  m.set_dirs(1000,1000);
  m.set_areas();

  print_pnm_head(m.w, m.h);
  simple_rainbow R(500,3000);

  iPoint p = iPoint(lon1+100, lat1+100);

  for (int lat=m.lat2-1; lat>=m.lat1; lat--){
    for (int lon=m.lon1; lon<m.lon2; lon++){

      short h  = m.geth(lat,lon);
      int c = 0;
      if (h > srtm_min) c = R.get(h);

      if (m.pt(lat,lon)->marea > 0.5) c=0x7f0000;
      if (m.pt(lat,lon)->rarea > 0.5) c=0x7f;

      print_pnm_col(c);
    }
  }

}

