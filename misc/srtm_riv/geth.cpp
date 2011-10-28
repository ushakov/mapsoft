#include <stdexcept>
#include <iostream>
#include <cmath>
#include <cstdlib>

#include <utils/srtm3.h>

// получение координат точек

main(){ 
try {

  srtm3 s("/d3/SRTMv2/", 4, interp_mode_on);
  // директория с hgt-файлами

  double lat,lon;

  while ((std::cin >> lat >> lon)){

    lat *= 1200;
    lon *= 1200;

    int x = int(lon);
    int y = int(lat);

    int h;

    if ((lat == y )&&(lon == x )){
      h = s.geth(x,y);
      if (h==srtm_nofile){
        std::cerr << "can't find file " << y/1200 << ' ' << x/1200 << '\n';
        exit(0);
      }
      else if (h==srtm_undef) std::cout << "undef\n";
      else  std::cout << h << '\n';
      continue;
    }

    int i,j;

    double Srh = 0;
    double Sr  = 0;

    const double ptsize_rad = 1.0/180.0/srtm_width*M_PI;
    // квадраты размеров одной точки, м:
    const double dx2 = pow(6380000.0 * ptsize_rad * cos(ptsize_rad*y), 2);
    const double dy2 = pow(6380000.0 * ptsize_rad, 2);


    for (j=0;j<2;j++){
      for (i=0;i<2;i++){
        h = s.geth(x+i,y+j);
        if (h==srtm_nofile){
          std::cerr << "can't find file " << (y+j)/1200 << ' ' << (x+i)/1200 << '\n';
          exit(0);
        }
        if (h>srtm_min){
          double rm2 = 1.0/( dy2*pow(lat-(double)(y+j), 2) + dx2*pow(lon-(double)(x+i), 2));
          Sr += rm2;
          Srh+= h * rm2;
        }
      }
    }
    if (Sr==0) std::cout << "undef\n";
    else std::cout << Srh/Sr << '\n';
  }

} catch(std::domain_error e){ std::cerr << e.what(); }
}

