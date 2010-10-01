#include "libgeo_io/geofig.h"
#include "libgeo/geo_convs.h"
#include "lib2d/line_polycrop.h"

// Перенесение данных из одного fig-файла в другой с обрезкой 
// Обрезка пока неаккуратная!!!

using namespace std;

void usage(){
    cerr << "usage: \n"
     " mapsoft_fig2fig <dest fig> <source fig>\n";
    exit(0);
}

main(int argc, char** argv){
  if (argc < 3) usage();

  std::string out_fig = argv[1];
  std::string in_fig  = argv[2];

  // читаем fig
  fig::fig_world F1;
  if (!fig::read(out_fig.c_str(), F1)) {
    std::cerr << "File is not modified.\n";
    exit(1);
  }
  g_map out_ref = fig::get_ref(F1);

  fig::fig_world F2;
  if (!fig::read(in_fig.c_str(), F2)) {
    std::cerr << "File is not modified.\n";
    exit(1);
  }
  g_map in_ref = fig::get_ref(F2);

  convs::map2map cnv(in_ref, out_ref);
  rem_ref(F2);

  fig::fig_object brd;
  brd.set_points(out_ref.border);

//  int n=0;
  for (fig::fig_world::const_iterator i=F2.begin(); i!=F2.end(); i++){
//    n++;
//std::cerr << n << " ";
//if (n==11885) F1.push_back(*i);
//continue;
    fig::fig_object o = *i;
    o.clear();
    if ((o.type == 6)||(o.type == -6)) continue;
    if (o.type == 1){
      dPoint p(o.center_x, o.center_y);
      cnv.frw(p);
      o.push_back(p);
    } else {
      o.set_points(cnv.line_frw(*i));
    }
    vector<fig::fig_object> l1,l2; 
    l1.push_back(o);
    crop_lines(l1, l2, brd, true);
    F1.insert(F1.end(), l1.begin(), l1.end());
//    F1.push_back(o);
  }

//  fig::fig_world FT;
//  crop_lines(F1, FT, brd, true);

  exit(!fig::write(out_fig, F1));
}
