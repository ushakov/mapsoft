#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>

#include "utils/srtm3.h"
#include "geo/geo_data.h"
#include "geo/geo_convs.h"
#include "geo_io/io_oe.h"

#define GETH geth16

// построение растровых горизонталей и выделение крутых уклонов цветами
// (СК Пулково, координаты Г-К)

// todo -- найти древнюю функцию rainbow для вычисления плавных переходов цветов
// todo -- вынести в параметры настройки на Сибирь/Подмосковья (разные диапазоны уклонов)
// todo -- стандартные параметры --geom --lon0 ...

const char* srtm_dir = "/d/MAPS/SRTMv2/"; 

struct rainbow_data{
  double v;
  int c;
};

int get_rainbow(double val, struct rainbow_data RD[], int rd_size){
  int r=0, g=0, b=0;
  for (int i=0; i<rd_size; i++){

    r  = (RD[i].c >> 16) & 0xFF;
    g  = (RD[i].c >> 8) & 0xFF;
    b  = RD[i].c & 0xFF;

    int ip = (i==0)? i : i-1;
    if (val <= RD[ip].v) break;

    if (val <= RD[i].v){
      int rp = (RD[ip].c >> 16) & 0xFF;
      int gp = (RD[ip].c >> 8) & 0xFF;
      int bp = RD[ip].c & 0xFF;

      r +=  (r - rp) * (val - RD[ip].v) / (RD[i].v-RD[ip].v);
      g +=  (g - gp) * (val - RD[ip].v) / (RD[i].v-RD[ip].v);
      b +=  (b - bp) * (val - RD[ip].v) / (RD[i].v-RD[ip].v);
      break;
    }
  }
  return (r << 16) + (g << 8) + b;
}

struct rainbow_data RD_podm[]={
  {5.0,  0xFFFFFF},
  {10.0, 0xFFFF00},
  {15.0, 0xFF0000},
  {20.0, 0xFF00FF},
  {25.0, 0x0000FF},
  {30.0, 0x404040}
};
int RDS_podm=sizeof(RD_podm)/sizeof(rainbow_data);

struct rainbow_data RD_hr[]={
  {30.0, 0xFFFFFF},
  {35.0, 0xFFFF00},
  {40.0, 0xFF0000},
  {45.0, 0xFF00FF},
  {50.0, 0x0000FF},
  {55.0, 0x404040}
};
int RDS_hr=sizeof(RD_hr)/sizeof(rainbow_data);


using namespace std;
int main(int argc, char *argv[]) {
try{
    if (argc!=11) {
	std::cerr << "usage: " << argv[0] << " scale dpi lon0 X1 X2 Y1 Y2 step sstep style\n";
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
    std::string style  = argv[10];

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
    m.file = "srtm_pic_out.pnm";
    m.map_proj = Proj("tmerc");

    Options O;
    O.put<double>("lon0", lon0);
    convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(),
                     Datum("pulkovo"), Proj("tmerc"), O);

    for (int j = H; j > 0; j--){
      std::cerr << j << " ";
      ho = srtm_undef;
      for (int i = 0; i < W; i++){

        dPoint p_tmerc((double)i/k + X1, (double)j/k + Y1);
	dPoint p  = p_tmerc;
        dPoint pl = p_tmerc + dPoint(0, 1/k);
        dPoint pm = p_tmerc + dPoint(1/k, 0);
        cnv.bck(p), cnv.bck(pl), cnv.bck(pm);

        // g_map
	if (((i==0)||(i==W-1))&&((j==H)||(j==1))){
          m.push_back(g_refpoint(p.x, p.y, i, H-j));
          m.border.push_back(dPoint(i,H-j));
        }

        h   = s.GETH(p);
        hl  = s.GETH(pl);
        hm  = s.GETH(pm);

	if ((h < srtm_min) || (hl < srtm_min) || (hm < srtm_min) ||
            (ho < srtm_min) || (hoo < srtm_min)) { //hole
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

              //rainbow
              int c;
              if (style == "podm")
                c=get_rainbow(deg, RD_podm, RDS_podm);
              else
                c=get_rainbow(deg, RD_hr, RDS_hr);
              c1  = (c >> 16) & 0xFF;
              c2  = (c >> 8) & 0xFF;
              c3  = c & 0xFF;
            }

	    d1 = ((int)(p_tmerc.x+1/k)/grid_step - (int)(p_tmerc.x)/grid_step);
	    d2 = ((int)(p_tmerc.y+1/k)/grid_step - (int)(p_tmerc.y)/grid_step);
	    if ((d1>0)||(d2>0)) c1=c2=c3=128; // grid

	}

	std::cout << c1 << c2 << c3;
      }
    }
    if (m.border.size()>=4) m.border[2].swap(m.border[3]);
    ofstream mf("srtm_pic_out.map");
    oe::write_map_file(mf, m, Options());

} catch(std::domain_error e){ std::cerr << e.what(); }
}

