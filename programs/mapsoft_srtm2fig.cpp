#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <list>
#include <stdexcept>
#include <cmath>

#include "srtm/srtm3.h"
#include "2d/line.h"

#include "2d/line_utils.h"
#include "2d/line_polycrop.h"
#include "2d/point_int.h"

#include "geo_io/geofig.h"
#include "geo/geo_data.h"
#include "geo/geo_convs.h"

// Перенесение данных srtm в привязанный fig-файл.

using namespace std;

void usage(){
    cerr << "usage: \n"
     " mapsoft_srtm2fig <fig> hor   <srtm_dir> <step1> <step2> [<acc, def=10>]\n"
     " mapsoft_srtm2fig <fig> scnt  <srtm_dir> <val> [<acc, def=10>]\n"
     " mapsoft_srtm2fig <fig> ver   <srtm_dir> [<DH, def=20> [<PS, def=500>]] \n"
     " mapsoft_srtm2fig <fig> holes <srtm_dir>\n";
}

int
main(int argc, char** argv){
  if (argc < 4) {
    usage();
    return 0;
  }

  std::string fig_name = argv[1];
  std::string cmd      = argv[2];
  std::string srtm_dir = argv[3];

  SRTM3 s(srtm_dir, 10);

  // read fig, build conversion fig -> wgs
  fig::fig_world F;
  if (!fig::read(fig_name.c_str(), F)) {
    std::cerr << "File is not modified, exiting.\n";
    return 1;
  }
  g_map fig_ref = fig::get_ref(F);
  convs::map2wgs fig_cnv(fig_ref);
  dRect range = fig_ref.range(); // fig range (lonlat)
  dLine border_ll = fig_cnv.line_frw(fig_ref.border); // fig border (lonlat)

  int count;
  if (cmd == "hor"){
    if (argc < 6){
      usage();
      return 0;
    }
    int step1 = atoi(argv[4]);
    int step2 = atoi(argv[5]);
    if (step2<step1) swap(step2,step1);
    double acc = 10; // "точность", в метрах - для генерализации горизонталей.
    if (argc>6) acc = atoi(argv[6]);

    std::cerr << "looking for contours\n";
    map<short, dMultiLine> hors = s.find_contours(range, step1);

    count = 0;
    cerr << "  merge and generalize: ";
    fig::fig_object o = fig::make_object("2 1 0 1 30453904 7 90 -1 -1 0.000 1 1 0 0 0 0");
    for(map<short, dMultiLine>::iterator im = hors.begin(); im!=hors.end(); im++){
      std::cerr << im->first << " ";
      generalize(im->second, acc/6380000/2/M_PI*180.0);
      split(im->second, 200);
      dMultiLine tmp;
      if (border_ll.size()) crop_lines(im->second, tmp, border_ll, true);

      for(dMultiLine::iterator iv = im->second.begin(); iv!=im->second.end(); iv++){
        if (iv->size()<3) continue;
        o.clear();
        if (im->first%step2==0) o.thickness = 2;
        else o.thickness = 1;
        o.set_points(fig_cnv.line_bck(*iv));
        o.comment.clear();
        o.comment.push_back(boost::lexical_cast<std::string>(im->first));
        F.push_back(o);
        count++;
      }
    }
    cerr << " - " << count << " pts\n";
  }

  else if (cmd == "scnt"){
    if (argc < 5){
      usage();
      return 0;
    }
    double val = atof(argv[4]);
    double acc = 10; // "точность", в метрах - для генерализации горизонталей.
    if (argc>5) acc = atoi(argv[5]);

    std::cerr << "looking for slope contours\n";
    dMultiLine cnt = s.find_slope_contour(range, val);

    count = 0;
    cerr << "  merge and generalize: ";
    generalize(cnt, acc/6380000/2/M_PI*180.0);
    join_polygons1(cnt);
    for (const auto & l:cnt){
      fig::fig_object o = fig::make_object("2 3 0 0 0 24 91 -1 20 0.000 0 0 -1 0 0 0");
      o.set_points(fig_cnv.line_bck(l));
      F.push_back(o);
      count++;
    }
    cerr << " - " << count << " pts\n";
  }



  else if (cmd == "ver"){
    int DH = 20;
    int PS = 500;
    if (argc>4) DH = atoi(argv[4]);
    if (argc>5) PS = atoi(argv[5]);

    int count = 0;
    std::cerr << "looking for mountains: ";
    map<dPoint, short> peaks = s.find_peaks(range, DH, PS);

    for(map<dPoint, short>::iterator i = peaks.begin(); i!=peaks.end(); i++){
      dPoint p1(i->first);
      if (border_ll.size() && !test_pt(p1, border_ll)) break;
      fig::fig_object o = fig::make_object("2 1 0 3 24 7  57 -1 -1 0.000 0 1 -1 0 0 1");
      fig_cnv.bck(p1);
      o.push_back(p1);
      o.comment.clear();
      o.comment.push_back(boost::lexical_cast<std::string>(i->second));
      F.push_back(o);
      count++;
    }
    cerr << peaks.size() << " pts\n";

  } 
  else if (cmd == "holes"){
    // поиск дырок
    cerr << "looking for srtm holes\n";
    dMultiLine aline = s.find_holes(range);
    dMultiLine tmp;
    if (border_ll.size()) crop_lines(aline, tmp, border_ll, true);

    for(dMultiLine::iterator iv = aline.begin(); iv!=aline.end(); iv++){
      if (iv->size()<3) continue;
      dLine l = fig_cnv.line_bck(*iv);
      fig::fig_object o = fig::make_object("2 3 0 0 0 4 110 -1 20 0.000 0 0 -1 0 0 0");
      o.insert(o.end(), l.begin(), l.end());
      F.push_back(o);
    }
    cerr << aline.size() << " polygons\n";
  }
  else {
    usage();
    return 0;
  }

  fig::write(fig_name, F);
  return 0;
}
