#include <stdexcept>
#include <iostream>
#include <cmath>

#include "srtm.h"
#include "color.h"

//#include "point.h"

// вывод красивой картинки в PNM-файл

// координаты углов:
inline int dms(int d, int m, int s) {return d*3600+m*60+s;}
inline int dms(int d, int m) {return d*3600+m*60;}
inline int dms(int d) {return d*3600;}
inline void swap(int x, int y){ int z=x; x=y; y=z;}

  const int lat1 = dms(53,30,00)/3; // данные задаем в секундах, деленных на три
  const int lat2 = dms(54,30,00)/3;
  const int lon1 = dms(95,00,00)/3;
  const int lon2 = dms(97,00,00)/3;



main(){ 
try {

  srtm s("/d2/SRTM/hgt/", 10, mode_interp); 
//  srtm s("/home/sla/TUR/SRTM/data/", 10, mode_interp);
    // директория с hgt-файлами, кэш из 10 файлов, интерполяция

  if (lat2<lat1) swap(lat2,lat1);
  if (lon2<lon1) swap(lon2,lon1);

  std::cout << "P6\n" << lon2-lon1 << " " << lat2-lat1 << "\n255\n";

  for (int lat=lat2-1; lat>=lat1; lat--){
    for (int lon=lon1; lon<lon2; lon++){

      short h  = s.geth(lat,lon);
      short hx = s.geth(lat+1,lon);
      short hy = s.geth(lat,lon+1);

      if (h >srtm_min_interp) h -=srtm_zer_interp;
      if (hx>srtm_min_interp) hx-=srtm_zer_interp;
      if (hy>srtm_min_interp) hy-=srtm_zer_interp;

      double a=180;

      if ((h > srtm_min) && (hx > srtm_min) && (hy > srtm_min)){

        const double ptsize_rad = 1.0/180.0/srtm_width*M_PI;
        // квадраты размеров одной точки, (м/угл.сек)^2:
        const double dx2 = pow(6380000.0 * ptsize_rad * cos(ptsize_rad*lat), 2);
        const double dy2 = pow(6380000.0 * ptsize_rad, 2);
        const double  U = sqrt(1.0*pow(h-hx ,2)/dx2 + 1.0*pow(h-hy ,2)/dy2);

        a = atan(U)*180.0/M_PI;
      }

      color c = color(0,0,0);

//      if (h > srtm_min){
//        c = rainbow(h, 500, 3000, 5, 0);
//      } 

//      if ((lon%1200==0)||(lat%1200==0)) a=45;
//      else if ((lon%30==0)||(lat%30==0)) a+=15;

      if ((h <= srtm_min) || (a>90)) c = color(255,255,255);

      else if ((h > srtm_min) && (a<45)){
        c = rainbow(h, 500, 3000, 5, 0) * (1-a/45.0);
      } 


//      if ((h > srtm_min) && (a<45)){
//        if (a < 45) c = color(128,128,0);
//        if (a < 30) c = color(182,182,0);
//        if (a < 15) c = color(255,255,255);
//      } 

      std::cout << c.r << c.g << c.b;
    }
  }
} catch(std::domain_error e){ std::cerr << e.what(); }
}

