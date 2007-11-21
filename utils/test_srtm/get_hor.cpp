#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>
#include <cmath>

#include "../srtm3.h"


// получаем название .hgt файла, отдаем .mp файл с горизонталями на stdout

using namespace std;

void usage(){
    cerr << "usage: get_hor lon1 lon2 lat1 lat2 step1 step2 > out.mp\n";
    exit(0);
}

//координаты угла по его номеру
Point<int> crn (int k){
  k%=4;
  return Point<int>(k/2, (k%3>0)?1:0);
}
//направление следующей за углом стороны (единичный вектор)
Point<int> dir (int k){
  return crn(k+1)-crn(k);
}


main(int argc, char** argv){
  if (argc != 7) usage();
  int lon1  = int(1200*atof(argv[1]));
  int lon2  = int(1200*atof(argv[2]));
  int lat1  = int(1200*atof(argv[3]));
  int lat2  = int(1200*atof(argv[4]));
  int step1 = atoi(argv[5]);
  int step2 = atoi(argv[6]);

  if (lat2<lat1)   swap(lat2,lat1);
  if (lon2<lon1)   swap(lon2,lon1);
  if (step2<step1) swap(step2,step1);

//  srtm3 s("/d/MAPS/SRTMv2/", 10, interp_mode_off);
  srtm3 s("./", 10, interp_mode_off);

cout << 
  "[IMG ID]\r\n" <<
  "ID=14401206\r\n" <<
  "Name=srtm_hor\r\n" <<
  "Elevation=M\r\n" <<
  "Preprocess=F\r\n" <<
  "CodePage=1251\r\n" <<
  "LblCoding=9\r\n" <<
  "TreSize=511\r\n" <<
  "TreMargin=0.000000\r\n" <<
  "RgnLimit=127\r\n" <<
  "POIIndex=N\r\n" <<
  "Levels=4\r\n" <<
  "Level0=22\r\n" <<
  "Level1=21\r\n" <<
  "Level2=19\r\n" <<
  "Level3=17\r\n" <<
  "Zoom0=0\r\n" <<
  "Zoom1=1\r\n" <<
  "Zoom2=2\r\n" <<
  "Zoom3=3\r\n" <<
  "[END-IMG ID]\r\n\r\n";


  for (int lat=lat2; lat>lat1; lat--){
    for (int lon=lon1; lon<lon2-1; lon++){


      Point<int> p(lon,lat);
//      cout << "---" << p  << "\n"; 

      // пересечения четырех сторон клетки с горизонталями:
      multimap<short, double> pts;
      for (int k=0; k<4; k++){
        Point<int> p1 = p+crn(k);
        Point<int> p2 = p+crn(k+1);
        short h1 = s.geth(p1);
        short h2 = s.geth(p2);
        if ((h1<srtm_min) || (h2<srtm_min)) continue;
        int min = (h1<h2)? h1:h2;
        int max = (h1<h2)? h2:h1;
        min = int( floor(double(min)/step1)) * step1;
        max = int( ceil(double(max)/step1))  * step1;
        if (h2==h1){
          if (h1%step1 ==0) pts.insert(pair<short, double>(h1,0.5+k)); 
          continue;
        }
        for (int s = min; s<=max; s+=step1){
          double x = double(s-h1)/double(h2-h1);
          if ((x<0)||(x>=1)) continue;
          pts.insert(pair<short, double>(s,x+k));
        }
      }
      // найдем, какие горизонтали пересекают квадрат дважды,
      // поместим их в специальный список горизонталей
      multimap<short, vector<Point<double> > > hors;
      short h=srtm_undef;
      double x1,x2;

      for (multimap<short,double>::const_iterator i=pts.begin(); i!=pts.end(); i++){
        if (h!=i->first){
          h  = i->first;
          x1 = i->second;
        } else{
          x2 = i->second;
          Point<double> p1 = Point<double>(p) + Point<double>(crn(int(x1))) + Point<double>(dir(int(x1)))*(x1-int(x1));
          Point<double> p2 = Point<double>(p) + Point<double>(crn(int(x2))) + Point<double>(dir(int(x2)))*(x2-int(x2));
//          cout << h << " " << x1 << " " << x2 << "\n";
//          cout << p1 << " " << p2 <<  "\n";
    cout << 
      "[POLYLINE]\r\n" <<
      "Type=0x21\r\n" <<
      "Label=" << h << "\r\n" << 
      "Data0=(" << p1.y/1200.0 << "," << p1.x/1200.0 << "),("<< p2.y/1200.0 << "," << p2.x/1200.0 << ")";
    cout << "\r\n[END]\r\n\r\n";

//          vector<point> pst;
//          pts.insert(p1);
//          pts.insert(p2);
//          hors.insert(pair<short, vector<point> >(h, pts));
        }
      }
      //

    }
  }
}
/*

#define PRINT_PT(P) cout << "(" << lat + (srtm_width - P.y + 0.5)/(double)srtm_width << "," << lon + (P.x+0.5)/(double)srtm_width << ")";

  while (brd1.size()!=0){
    point p0 = brd1.begin()->first;
    point p1 = brd1.begin()->second;
    brd1.erase(brd1.begin());

    cout << 
      "[POLYGON]\r\n" <<
      "Type=" << mp_type << "\r\n" <<
      "Levels=1\r\n" << 
      "Data0=";

    PRINT_PT(p0);
    cout << ",";

    while (p1!=p0){
      PRINT_PT(p1);
      cout << ",";

      set<pair<point,point> >::iterator v;
      for (v = brd1.begin(); v!=brd1.end(); v++)
        if (v->first == p1) break;

      if (v!=brd1.end()){
        p1 = v->second;
        brd1.erase(v);
      }
      else throw domain_error("Unclosed border!\n");
    }
    PRINT_PT(p0);
    cout << "\r\n[END]\r\n\r\n";
  }
    
//cout << lat + i/(double)srtm_width << " " << lon + j/(double)srtm_width << "\n";

}
*/
