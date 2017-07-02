#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <list>

#include "img_io/gobj_pano.h"
//#include "jeeps/gpsmath.h"

#include "geo_io/io.h"
#include "geo/geo_convs.h"
#include "loaders/image_r.h"
#include "mp/mp.h"
#include "fig/fig.h"
//#include "err/err.h"

// панорама по srtm с геоданными. 

using namespace std;

void usage(const char *fname){
  cerr << "Usage: "<< fname << " <width, px> <lat, deg> <lon, deg> <alt, m> [<geodata1>...<geodatan>]\n";
  exit(0);
}

int main(int argc, char *argv[]) {

  list<string> infiles;

// разбор командной строки
  if (argc < 5) usage(argv[0]);
  int width   = atoi(argv[1]);
  double lat0 = atof(argv[2]);
  double lon0 = atof(argv[3]);
  double alt0 = atof(argv[4]);
  int height=width/4;

  Options opt;
  opt.put<int>("pano_width",   width);
  opt.put<double>("pano_minh", 0.0);
  opt.put<double>("pano_maxh", 6000.0);
  opt.put<double>("pano_alt",  alt0);
  opt.put<double>("pano_maxr", 60000.0);
  opt.put<dPoint>("pano_pt",   dPoint(lon0,lat0));

  cerr << "making pano " << width << " x " << height << " pix, ";
  cerr << "lon,lat = " << lon0 << "," << lat0 << " alt= " << alt0 << "\n";

  cerr << opt << "\n";

  SRTM3 s;
  GObjPano gobj(&s);
  gobj.set_opt(opt);
  iImage img(width, height);

  gobj.draw(img, iPoint(-width/2,0));

  fig::fig_world F;

  double kk=0.1*fig::cm2fig;

  fig::fig_object o1 = fig::make_object("2 5 0 1 17 -1 500 -1 -1 0.000 0 0 -1 0 0 5");
  o1.image_file="out_pano.jpg";
  o1.push_back(iPoint(-width*kk/2, 0));
  o1.push_back(iPoint(width*kk/2,  0));
  o1.push_back(iPoint(width*kk/2,  height*kk));
  o1.push_back(iPoint(-width*kk/2, height*kk));
  o1.push_back(iPoint(-width*kk/2, 0));
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

      iPoint fig_pt(dPoint(gobj.geo2xy((*l)[0]))*kk);

      if (fig_pt.y<=0) continue;
      double z = (double)s.geth4((*l)[0]);

      o2.clear(); o3.clear();
      o2.pen_color=o3.pen_color=c;

      o2.push_back(fig_pt);
      o3.push_back(fig_pt + iPoint(50,-100));

      ostringstream label_stream;
      label_stream << i->Label
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
    catch (Err e) {cerr << e.get_error() << endl;}
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
      if ((img.get(px,py) & 0xFF) < col) continue;
      
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
  image_r::save(img,"out_pano.jpg");
}
