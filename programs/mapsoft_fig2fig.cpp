#include "../geo_io/geofig.h"
#include "../geo_io/geo_convs.h"
#include <fstream>

// Перенесение данных из одного fig-файла в другой с обрезкой 

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
  fig::fig_world F1 = fig::read(out_fig.c_str());
  g_map out_ref = fig::get_ref(F1);
  fig::fig_world F2 = fig::read(in_fig.c_str());
  g_map in_ref = fig::get_ref(F2);

  convs::map2map cnv(in_ref, out_ref);
  rem_ref(F2);

  for (fig::fig_world::const_iterator i=F2.begin(); i!=F2.end(); i++){
    fig::fig_object o = *i;
    o.set_points(cnv.line_frw(*i));
    F1.push_back(o);
  }
  fig::fig_object brd;
  brd.set_points(out_ref.border);
  fig::fig_world FT;
  crop_lines(F1, FT, brd, true);

  std::ofstream f(out_fig.c_str());
  fig::write(f, F1);
}
