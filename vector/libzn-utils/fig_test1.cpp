#include <string>
#include <cstring>
#include "geo_io/geofig.h"

#include "geo_io/io.h"
#include "geo/geo_convs.h"

// для проверки сдвига fig-файла при многочисленных
// преобразованиях координат...

using namespace std;

bool testext(const string & nstr, const char *ext){
    int pos = nstr.rfind(ext);
    return ((pos>0)&&(pos == nstr.length()-strlen(ext)));
}

int main(int argc, char** argv){

  if (argc < 2){
    cerr  << "Insufficient arguments\n"; return 1;
  }

  string file = argv[1];

  fig::fig_world F;

  if (!fig::read(file.c_str(), F)) {
    cerr << "ERR: bad fig file " << file << "\n"; exit(1);
  }

  g_map ref = fig::get_ref(F);

  if (ref.size()<3){
    cerr << "ERR: not a GEO-fig\n"; exit(1);
  }
  convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"));

  for (fig::fig_world::iterator i=F.begin(); i!=F.end(); i++){
    for (fig::fig_object::iterator o = i->begin(); o!=i->end(); o++){
      dPoint p = *o;
      cnv.frw(p);
      cnv.bck(p);
      *o = p;
    }
//    dLine l=cnv.line_frw(*i);
//    i->set_points(cnv.line_bck(l));

//    cnv.line_frw(*i);
//    cnv.line_bck(*i);
  }
  fig::write(file, F);

  exit(0);
}
