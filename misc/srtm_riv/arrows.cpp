#include <stdexcept>
#include <iostream>
#include <cmath>

#include "srtm.h"
#include "color.h"
#include "map.h"

// рисование стрелочек

int arr3[] =
{1,0,1, 1,0,0, 1,0,1, 1,1,1, 1,1,1, 1,1,1, 1,0,1, 0,0,1,
 0,1,0, 1,1,0, 1,1,0, 1,1,0, 0,1,0, 0,1,1, 0,1,1, 0,1,1,
 1,1,1, 1,1,1, 1,0,1, 1,0,0, 1,0,1, 0,0,1, 1,0,1, 1,1,1};

int arr4[] =
{1,1,0,1, 1,0,1,0, 1,1,0,1, 1,0,0,1, 1,1,0,0, 1,1,0,1, 1,0,0,1, 1,1,0,0,
 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,1,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,1,
 1,0,0,1, 1,1,0,1, 1,1,0,0, 1,0,0,1, 1,1,0,1, 1,0,1,0, 1,1,0,1, 1,1,0,0,
 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1};

inline int dms(int d, int m, int s) {return d*3600+m*60+s;}
inline int dms(int d, int m) {return d*3600+m*60;}
inline int dms(int d) {return d*3600;}
                                    // координаты углов
const int lat1 = dms(55,00,00)/3; // данные задаем в секундах, деленных на три
const int lat2 = dms(55,10,00)/3;
const int lon1 = dms(111,00,00)/3;
const int lon2 = dms(111,10,00)/3;



main(){ 
try {

  map m("/d3/SRTMv2/", lat1, lon1, lat2, lon2);
//  map m("/d2/SRTM/hgt/", lat1, lon1, lat2, lon2);
  m.set_dirs(100,100);

//  m.mtrace(point(lon1+160,lat1+120),1000);
//  m.mtrace(point(lon1+80,lat1+110),1000);

  const int arr_size=4;
  std::cout << "P6\n" << m.w*arr_size << " " << m.h*arr_size << "\n255\n";

  for (int lat=m.lat2-1; lat>=m.lat1; lat--){
    for (int row=0; row<arr_size; row++){
      for (int lon=m.lon1; lon<m.lon2; lon++){

        short h  = m.geth(lat,lon);
        color c = color(0,0,0);

        if (h > srtm_min){
          c = rainbow(h, 500, 3000, 5, 0);
        }

        int dir = m.pt(lat,lon)->rdir;
        for (int col=0; col<arr_size; col++){

//        if ((lat-lat1==143)&&(lon-lon1==117)) c = color(255,0,0);

          if ((dir>-1)&&(dir<8)&&
              arr4[arr_size*8*row + dir*arr_size + col]==0)
            std::cout << (char)0 << (char)0 << (char)0;
          else std::cout << c.r << c.g << c.b;
      
        }
      }
    }
  }

} catch(std::domain_error e){ std::cerr << e.what(); }
}

