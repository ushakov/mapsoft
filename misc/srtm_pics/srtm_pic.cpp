#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>

#include "utils/srtm3.h"
#include "geo/geo_data.h"
#include "geo_io/io_oe.h"
#include "jeeps/gpsmath.h"
#include "jeeps/gpsdatum.h"

#define GETH geth16

// построение растровых горизонталей и выделение крутых уклонов цветами
// (СК Пулково, координаты Г-К)

// todo -- найти древнюю функцию rainbow для вычисления плавных переходов цветов
// todo -- вынести в параметры настройки на Сибирь/Подмосковья (разные диапазоны уклонов)
// todo -- стандартные параметры --geom --lon0 ...

const char* srtm_dir = "/d/MAPS/SRTMv2/"; 
const int datum = 92; //из ../jeeps/gpsdatum.h

using namespace std;
int main(int argc, char *argv[]) {
try{
    if (argc!=10) {
	std::cerr << "usage: " << argv[0] << " scale dpi lon0 X1 X2 Y1 Y2 step sstep\n";
	exit(0);
    }
    // Зададим масштаб нужной нам карты, ее границы (в км Г-К), разрешение, осевой меридиан:
    double scale = atof(argv[1]);
    double dpi   = atof(argv[2]);
    double lon0  = atof(argv[3]);
    double X1    = atof(argv[4]);
    double X2    = atof(argv[5]);
    double Y1    = atof(argv[6]);
    double Y2    = atof(argv[7]);
    int step   = atoi(argv[8]);
    int sstep  = atoi(argv[9]);

    const int grid_step = 1000;

// определим размер картинки
    if ((X1>X2)||(Y1>Y2)) throw std::domain_error("Range error!\n"); 

    double k = scale/2.54e-2*dpi;

    int W = (int)((X2-X1)*k);
    int H = (int)((Y2-Y1)*k);
    std::cerr << W << "x" << H << "\n";

    srtm3 s(srtm_dir, 10, interp_mode_off); 


    std::cout << "P6\n" << W << " " << H << "\n255\n";

    int h, hl, hm, ho=srtm_undef, hoo=srtm_undef;

    unsigned char c1,c2,c3;
    bool dd=false;

    g_map m;
    m.comm = "made by mapsoft_srtm_hor";
    m.file = "out.pnm";
    m.map_proj = Proj("tmerc");

    double a = GPS_Ellipse[GPS_Datum[datum].ellipse].a;
    double f = 1/GPS_Ellipse[GPS_Datum[datum].ellipse].invf;
    double b = a*(1-f);

    for (int j = H; j > 0; j--){
      std::cerr << j << " ";
      ho = srtm_undef;
      for (int i = 0; i < W; i++){

	double X  = (double)i/k + X1;
	double Y  = (double)j/k + Y1;
	double lat,lon, latl,lonl, latm,lonm;

	GPS_Math_EN_To_LatLon(X,     Y,     &lat,  &lon,  0, 500000, 0, lon0, 1, a, b);
	GPS_Math_EN_To_LatLon(X,     Y+1/k, &latl, &lonl, 0, 500000, 0, lon0, 1, a, b);
	GPS_Math_EN_To_LatLon(X+1/k, Y,     &latm, &lonm, 0, 500000, 0, lon0, 1, a, b);

	double lat_wgs,lon_wgs, lat_wgs_l, lon_wgs_l, lat_wgs_m, lon_wgs_m, z;
	GPS_Math_Known_Datum_To_WGS84_M(lat,  lon,  0, &lat_wgs,   &lon_wgs,   &z, datum);
	GPS_Math_Known_Datum_To_WGS84_M(latl, lonl, 0, &lat_wgs_l, &lon_wgs_l, &z, datum);
	GPS_Math_Known_Datum_To_WGS84_M(latm, lonm, 0, &lat_wgs_m, &lon_wgs_m, &z, datum);

        // g_map
	if (((i==0)||(i==W-1))&&((j==H)||(j==1)))
          m.push_back(g_refpoint(lon_wgs,lat_wgs,i,H-j));

        h   = s.GETH(dPoint(lon_wgs, lat_wgs));
        hl  = s.GETH(dPoint(lon_wgs_l, lat_wgs_l));
        hm  = s.GETH(dPoint(lon_wgs_m, lat_wgs_m));

	if ((h < srtm_min) || (hl < srtm_min) || (hm < srtm_min) ||(ho < srtm_min) || (hoo < srtm_min)) { //hole
	    c1=c2=c3 = 200;
	    if (h>srtm_min) { ho=h/step; hoo=h/sstep; }
	}
	else {
	    c1=c2=c3=255;
	    int d1,d2,d;

/*    	    d1 = (h/step - ho);
	    d2 = (hl/step - ho);

	    if ((d1!=0)||(d2!=0)) c1=c2=c3=128;
*/

	    ho = (abs(d1)>abs(d2))? hl/step:h/step;

    	    d1 = (h/sstep - hoo);
	    d2 = (hl/sstep - hoo);

	    if ((d1!=0)||(d2!=0)) c1=c2=c3=0;
	    hoo = (abs(d1)>abs(d2))? hl/sstep:h/sstep;

            if ((c1==255) && (c2=255) && (c3=255)){
              double dhx = (h - hm)*k;
              double dhy = (h - hl)*k;
              double ngrad = sqrt(dhx*dhx + dhy*dhy);
              double deg = 180/M_PI*atan(ngrad);

/*
	      //rainbow
              if (deg<=30) {
                c1=c2=c3=255;
                //c1=c2=c3= char(255 - (atan(grey_trans*dhx/ngrad)/atan(grey_trans)+1)*grey_depth);
              }


              else if ((deg>30)&&(deg<=35)) {c1=255; c2=255; c3=255-int(255* (deg-30.0) / (35.0-30.0));} // white -> yellow
              else if ((deg>35)&&(deg<=40)) {c1=255; c2=255-int(255* (deg-35.0) / (40.0-35.0)); c3=0;} // yellow -> red
              else if ((deg>40)&&(deg<=45)) {c1=255; c2=0; c3=int(255* (deg-40.0) / (45.0-40.0));} // red -> magenta
              else if ((deg>45)&&(deg<=50)) {c1=255-int(255* (deg-45.0) / (50.0-45.0)); c2=0; c3=255;} //magenta -> blue
              else if ((deg>50)&&(deg<=55)) {c1=int(64 * (deg-50.0) / (55.0-50.0)); c2=int(64 * (deg-50.0) / (55.0-50.0)); c3=255-int(192* (deg-50.0) / (55.0-50.0));} // blue -> gray
              else {c1=64; c2=64; c3=64;}
*/
//подмосковное:
              if (deg<=5) {
                c1=c2=c3=255;
                //c1=c2=c3= char(255 - (atan(grey_trans*dhx/ngrad)/atan(grey_trans)+1)*grey_depth);
              }
              else if ((deg>5)&&(deg<=10)) {c1=255; c2=255; c3=255-int(255* (deg-5.0) / (10.0-5.0));} // white -> yellow
              else if ((deg>10)&&(deg<=15)) {c1=255; c2=255-int(255* (deg-10.0) / (15.0-10.0)); c3=0;} // yellow -> red
              else if ((deg>15)&&(deg<=20)) {c1=255; c2=0; c3=int(255* (deg-15.0) / (20.0-15.0));} // red -> magenta
              else if ((deg>20)&&(deg<=25)) {c1=255-int(255* (deg-20.0) / (25.0-20.0)); c2=0; c3=255;} //magenta -> blue
              else if ((deg>25)&&(deg<=30)) {c1=int(64 * (deg-25.0) / (30.0-25.0)); c2=int(64 * (deg-25.0) / (30.0-25.0)); c3=255-int(192* (deg-25.0) / (30.0-25.0));} // blue -> gray
              else {c1=64; c2=64; c3=64;}


            }

	    d1 = ((int)(X+1/k)/grid_step - (int)(X)/grid_step);
	    d2 = ((int)(Y+1/k)/grid_step - (int)(Y)/grid_step);
	    if ((d1>0)||(d2>0)) c1=c2=c3=128; // grid

	}

	std::cout << c1 << c2 << c3;
      }
    }
//    m.init(lon0);
//    options o;
    m.ensure_border();
    ofstream mf("out.map");
    oe::write_map_file(mf, m, Options());

} catch(std::domain_error e){ std::cerr << e.what(); }
}

