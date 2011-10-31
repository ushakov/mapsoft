#include <2d/rainbow.h>
#include <srtm/srtm3.h>
#include "pnm.h"

// вывод красивой картинки в PNM-файл

// координаты углов:
inline int dms(int d, int m, int s) {return d*3600+m*60+s;}
inline int dms(double d) {return d*3600;}
inline void swap(int x, int y){ int z=x; x=y; y=z;}

const int lat1 = dms(53,30,00)/3; // данные задаем в секундах, деленных на три
const int lat2 = dms(54,30,00)/3;
const int lon1 = dms(95,00,00)/3;
const int lon2 = dms(97,00,00)/3;

int shade(int c, double k){
  unsigned char r=(c>>16)&0xff, g=(c>>8)&0xff,  b=c&0xff;
  r*=k; g*=k; b*=k;
  return (r << 16) + (g << 8) + b;
}

main(){

  srtm3 s;
  // директория с hgt-файлами, кэш из 10 файлов, интерполяция

  if (lat2<lat1) swap(lat2,lat1);
  if (lon2<lon1) swap(lon2,lon1);

  print_pnm_head(lon2-lon1, lat2-lat1);
  simple_rainbow R(500,3000);

  for (int lat=lat2-1; lat>=lat1; lat--){
    for (int lon=lon1; lon<lon2; lon++){

      short h  = s.geth(lon, lat, true);
      short hx = s.geth(lon, lat+1, true);
      short hy = s.geth(lon+1, lat, true);

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

      int c = 0;

      if ((h <= srtm_min) || (a>90)) c = 0xffffff;

      else if ((h > srtm_min) && (a<45)){
        c = shade(R.get(h), (1-a/45.0));
      }

      print_pnm_col(c);
    }
  }
}

