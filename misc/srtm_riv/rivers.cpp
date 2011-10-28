#include <stdexcept>
#include <iostream>
#include <cmath>

#include <utils/srtm3.h>
#include "color.h"
#include "map.h"

inline int dms(int d, int m, int s) {return d*3600+m*60+s;}
inline int dms(int d, int m) {return d*3600+m*60;}
inline int dms(int d) {return d*3600;}
                                    // координаты углов

  const int lat1 = dms(53,30,00)/3; // данные задаем в секундах, деленных на три
  const int lat2 = dms(54,30,00)/3;
  const int lon1 = dms(95,00,00)/3;
  const int lon2 = dms(97,00,00)/3;



main(){ 
try {

  map m("/d3/SRTMv2/", lat1, lon1, lat2, lon2);
  m.set_dirs(100000,100000);
  m.set_areas();
 

  std::cout << "P6\n" << m.w << " " << m.h << "\n255\n";

  point p = point(lon1+100, lat1+100);

  for (int lat=m.lat2-1; lat>=m.lat1; lat--){
    for (int lon=m.lon1; lon<m.lon2; lon++){

      short h  = m.geth(lat,lon);
      color c = color(0,0,0);

      if (h > srtm_min)
        c = rainbow(h, 500, 3000, 5, 0);

      if (m.pt(lat,lon)->marea > 0.5) c=color(0,0,0);
      if (m.pt(lat,lon)->rarea > 0.5) c=color(0,0,255);

      std::cout << c.r << c.g << c.b;
    }
  }

} catch(std::domain_error e){ std::cerr << e.what(); }
}

