#include <2d/rainbow.h>
#include "map.h"
#include "pnm.h"

// построение хребтовки-речевки

inline int dms(int d, int m, int s) {return d*3600+m*60+s;}

                                    // координаты углов
const int lat1 = dms(55,00,00)/3; // данные задаем в секундах, деленных на три
const int lat2 = dms(56,00,00)/3;
const int lon1 = dms(111,00,00)/3;
const int lon2 = dms(112,00,00)/3;

main(){
  map m(lat1, lon1, lat2, lon2);

  print_pnm_head(m.w, m.h);

  m.trace(iPoint(lon1+1100, lat1+200), false, 1000);
  m.trace(iPoint(lon1+600, lat1+200), true, 1000);

  simple_rainbow R(500,3000);

  for (int lat=m.lat2-1; lat>=m.lat1; lat--){
    for (int lon=m.lon1; lon<m.lon2; lon++){

      short h  = m.geth(lat,lon);
      int c = 0xffffff;

      if (h > srtm_min)  c = R.get(h);

      if (m.pt(lat,lon)->rdir != -1) c= 0x7f;
      if (m.pt(lat,lon)->mdir != -1) c= 0x7f0000;

      print_pnm_col(c);
    }
  }
}

