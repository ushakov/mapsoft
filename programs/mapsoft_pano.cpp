#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <list>
#include <vector>

#include "srtm/srtm3.h"
#include "jeeps/gpsmath.h"

#include "geo_io/io.h"
#include "geo/geo_convs.h"
#include "loaders/image_r.h"
#include "mp/mp.h"
#include "fig/fig.h"
#include "utils/err.h"

// панорама по srtm с геоданными. 

using namespace std;

const char* srtm_dir = def_srtm_dir.c_str();

const int max_r = 60000; // m
const int min_r = 100;    // m

const double min_b = -M_PI/4.0; // панорама от min_b до max_b по вертикали
const double max_b =  M_PI/4.0; //
const double min_a = -M_PI;   //
const double max_a =  M_PI;   //

const double Re=6380000;


void usage(const char *fname){
  cerr << "Usage: "<< fname << " <width, px> <lat, deg> <lon, deg> <alt, m> [<geodata1>...<geodatan>]\n";
  exit(0);
}

dPoint fast_cnv(double lon0, dPoint p){
  return dPoint((p.x-lon0)*Re*cos(p.y), p.y*Re);
}
dPoint fast_rcnv(double lon0, dPoint p){
  return dPoint(lon0 + p.x/Re/cos(p.y/Re), p.y/Re);
}

int r2col(double r){
  if (r>max_r) r=max_r;
  int color = 255*(r-min_r)/(max_r-min_r);
  color |= (color&0xFF) << 8;
  color |= (color&0xFF) << 16;
  return color;
}

int main(int argc, char *argv[]) {

  list<string> infiles;

// разбор командной строки
  if (argc < 5) usage(argv[0]);
  int width   = atoi(argv[1]);
  double lat0 = atof(argv[2])*M_PI/180.0;
  double lon0 = atof(argv[3])*M_PI/180.0;
  double alt0 = atof(argv[4]);

  double rad2pt = double(width)/(max_a-min_a);
  double pt2rad = 1.0/rad2pt;
  int height    = (max_b-min_b)*rad2pt;


  srtm3 s(srtm_dir, 10);

  iImage data(width, height);

  // примерный шаг srtm
  int dr = int(1/1200.0 * Re *M_PI/90.0 *cos(lat0));

  dPoint p0 = fast_cnv(lon0, dPoint(lon0, lat0));
  double x0 = p0.x;
  double y0 = p0.y;
  double z0 = alt0;


  cerr << "making pano " << width << " x " << height << " pix, " 
       << int((max_a-min_a)*180/M_PI) << " x " << int((max_b-min_b)*180/M_PI) <<" deg\n";
  cerr << "x0 = " << x0 << " y0 = " << y0 << " alt0= " << alt0 << "\n";

  
  for (int x=0; x<width; x++){
    int zo=0;

    double a = max_a - x*pt2rad;

    cerr << "[" << x << "]";

    for (int i = min_r/dr; i<max_r/dr; i++){
      int r = i*dr;

      dPoint p = fast_rcnv(lon0, dPoint(r*cos(a)+x0, r*sin(a)+y0)) * (180.0/M_PI);
      double alt = (double)s.geth4(p);
      if (alt > srtm_min_interp) alt-=srtm_zer_interp;

      int zn = int((atan((alt-alt0)/double(r)) - min_b)*rad2pt);
      if (zn<=zo) continue;
      if (zn>=height) zn=height;

      for (int z = zo; z<zn; z++ ) data.set(x,height-z-1, r2col(r));
      zo=zn;
 
      double altx = alt0+r*tan(zn/rad2pt + min_b);
      if ((altx > 10000) || (altx<-1000)) r=max_r;
    }
    for (int z = zo; z<height; z++ ) data.set(x,height-z-1,0xFF0000); 
  }


  fig::fig_world F;

  double kk=0.1*fig::cm2fig;

  fig::fig_object o1 = fig::make_object("2 5 0 1 17 -1 500 -1 -1 0.000 0 0 -1 0 0 5");
  o1.image_file="out_pano.jpg";
  o1.push_back(iPoint(0,0));
  o1.push_back(iPoint(int(width*kk), 0));
  o1.push_back(iPoint(int(width*kk), int(height*kk)));
  o1.push_back(iPoint(0, int(height*kk)));
  o1.push_back(iPoint(0,0));
  F.push_back(o1);

  fig::fig_object o2 = fig::make_object("2 1 0 5 0 7 50 -1 -1 0.000 0 1 7 0 0 1");
  fig::fig_object o3 = fig::make_object("4 0 0 52 -1 18 20 1.5708 4");


  mp::mp_world W;
  for (int i=5; i<argc; i++) mp::read(argv[i], W);

  for (mp::mp_world::const_iterator i=W.begin(); i!=W.end(); i++){
    for (mp::mp_object::const_iterator l=i->begin(); l!=i->end(); l++){
      if (i->Class!="POI") continue;
      if ((i->Type!=0xd00) && (i->Type!=0xf00) && (i->Type!=0x1100) && // вершина
          (i->Type!=0x6406) && ((i->Type < 0x6620) || (i->Type > 0x6626))) // перевал
          continue;
      int c=(i->Type>0x6000)?0x009000:0x000090;
      dPoint pw = fast_cnv(lon0, (*l)[0] * (M_PI/180.0));

      double r = sqrt(pow(pw.x-p0.x,2)+pow(pw.y-p0.y,2));
      if ((r > max_r)||(r<min_r)) continue;

      double a = atan2(pw.y-p0.y, pw.x-p0.x);
      if ((a > max_a)||(a<min_a)) continue;

      double z = (double)s.geth4((*l)[0]);
      if (z > srtm_min_interp) z-=srtm_zer_interp;
      if (z < srtm_min) continue;

      double b = atan2(z-z0, r);
      if ((b > max_b)||(b<min_b)) continue;

      int px = (max_a-a)*rad2pt;
      int py = (max_b-b)*rad2pt;

      // blue channel!
      int col = r2col(r+1000)>>16;
      if ((data.get(px,py)>>16) < col) continue;

      iPoint fig_pt(int((max_a-a)/(max_a-min_a)*width*kk), int((max_b-b)/(max_b-min_b)*height*kk));    
      o2.clear();
      o3.clear();

      o2.pen_color=c;
      o3.pen_color=c;

      o2.push_back(fig_pt);
      o3.push_back(fig_pt + iPoint(50,-100));

      ostringstream label_stream;
      label_stream 
           << i->Label << " "
           << fixed << setprecision(2) << r/1000.0 << " "
           << fixed << setprecision(0) << z;

      o3.text=label_stream.str();
      F.push_back(o2);
      F.push_back(o3);
    }
  }

/*
  geo_data world;

  for (int i=5; i<argc; i++) infiles.push_back(argv[i]);
  list<string>::const_iterator fb=infiles.begin(), fe=infiles.end(), f;
  for(f=fb; f!=fe; f++){
    try{io::in(*f, world);}
    catch (MapsoftErr e) {cerr << e.str() << endl;}
  }

  for (int i = 0; i<world.wpts.size(); i++){
    fig << "6 0 0 " << int(width*kk) << " " << int(height*kk) << "\n";
    for (int j=0; j<world.wpts[i].size(); j++){

      dPoint pw = fast_cnv(lon0, world.wpts[i][j] * (M_PI/180.0));

      double r = sqrt(pow(pw.x-p0.x,2)+pow(pw.y-p0.y,2));
      if ((r > max_r)||(r<min_r)) continue;

      double a = atan2(pw.y-p0.y, pw.x-p0.x);
      if ((a > max_a)||(a<min_a)) continue;

      double z = (double)s.geth4(world.wpts[i][j]);
      if (z > srtm_min_interp) z-=srtm_zer_interp;
      if (z < srtm_min) continue;

      double b = atan2(z-z0, r);
      if ((b > max_b)||(b<min_b)) continue;

      int px = (a-min_a)*rad2pt;
      int py = (max_b-b)*rad2pt;

      int col = r2col(r+50) & 0xFF;
      if ((data.get(px,py) & 0xFF) < col) continue;
      
      fig << "# " << world.wpts[i][j].comm << "\n";
      fig << "2 1 0 5 8 7 50 -1 -1 0.000 0 1 7 0 0 1\n";
      fig << " " << int((max_a-a)/(max_a-min_a)*width*kk) 
           << " " << int((max_b-b)/(max_b-min_b)*height*kk) << "\n";
      fig << "4 0 8 52 -1 18 20 1.5708 4 0 0 " 
           << int((max_a-a)/(max_a-min_a)*width*kk) + 50 << " "
           << int((max_b-b)/(max_b-min_b)*height*kk) - 100 << " "
           << world.wpts[i][j].name << " " 
           << fixed << setprecision(2) << r/1000.0 << " "
           << fixed << setprecision(0) << z << " "
            << "\\001\n";
    }
    fig << "-6\n";
  }
*/

  fig::write("out_pano.fig", F);
  image_r::save(data,"out_pano.jpg",Options());
}
