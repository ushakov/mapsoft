#include <string>
#include <cstring>
#include "options/m_getopt.h"
#include "vmap/vmap.h"
#include "vmap/zn.h"
#include "fig/fig.h"
#include "geo_io/io.h"

using namespace std;

// replace objects of a single type
// in a map using geodata

int
main(int argc, char **argv){
try{

  if (argc!=5) {
    std::cerr << "usage: vmap_put_track <class> <type> <in geodata> <out map> \n";
    return 1;
  }
  std::string cl(argv[1]);
  int type = atoi(argv[2]);
  const char * ifile = argv[3];
  const char * ofile = argv[4];

  if (cl == "POI"){ }
  else if (cl == "POLYLINE") type |= zn::line_mask;
  else if (cl == "POLYGON")  type |= zn::area_mask;
  else throw Err() << "unknown object class: " << cl;

  vmap::world V = vmap::read(ofile);
  geo_data data;
  io::in(ifile, data);

  // remove old objects:
  vmap::world::iterator i=V.begin();
  while (i!=V.end()){
    if (i->type == type) {
      i=V.erase(i);
      continue;
    }
    i++;
  }

  // put objects

  if (cl == "POI"){
    for (auto const & pts:data.wpts) {
      for (auto const & pt:pts) {
        vmap::object o;
        dLine l;
        l.push_back(pt);
        o.push_back(l);
        o.text=pt.name;
        o.type=type;
        V.push_back(o);
      }
    }
  }
  else {
    for (auto const & t:data.trks) {
      vmap::object o;
//      o.text=t.comm;
      o.type=type;
      dLine l;
      for (auto const & pt:t) {
        if (pt.start && l.size()){
          o.push_back(l);
          V.push_back(o);
          l.clear();
          o.clear();
        }
        l.push_back(pt);
      }
      if (l.size()){
        o.push_back(l);
        V.push_back(o);
      }
    }
  }

  vmap::write(ofile, V);
}
catch (Err e) {
  cerr << e.get_error() << endl;
  return 1;
}
return 0;
}


