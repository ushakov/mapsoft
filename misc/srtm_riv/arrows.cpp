#include <2d/rainbow.h>
#include "map.h"
#include "pnm.h"

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

                                    // координаты углов
const int lat1 = dms(55,00,00)/3; // данные задаем в секундах, деленных на три
const int lat2 = dms(55,10,00)/3;
const int lon1 = dms(111,00,00)/3;
const int lon2 = dms(111,10,00)/3;

main(){

  map m(lat1, lon1, lat2, lon2);
  m.set_dirs(100,100);

  const int arr_size=4;
  print_pnm_head(m.w*arr_size, m.h*arr_size);
  simple_rainbow R(500,3000);

  for (int lat=m.lat2-1; lat>=m.lat1; lat--){
    for (int row=0; row<arr_size; row++){
      for (int lon=m.lon1; lon<m.lon2; lon++){

        short h  = m.geth(lat,lon);
        int  c = 0;

        if (h > srtm_min)  c = R.get(h);

        int dir = m.pt(lat,lon)->rdir;
        for (int col=0; col<arr_size; col++){

          if ((dir>-1)&&(dir<8)&&
              arr4[arr_size*8*row + dir*arr_size + col]==0)
            print_pnm_col(0);
          else print_pnm_col(c);
        }
      }
    }
  }
}

