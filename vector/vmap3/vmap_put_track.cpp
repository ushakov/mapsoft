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

  bool skip_names = true;
  const char wpts_pref = '=';

  if (cl == "POI"){ }
  else if (cl == "POLYLINE") type |= zn::line_mask;
  else if (cl == "POLYGON")  type |= zn::area_mask;
  else throw Err() << "unknown object class: " << cl;

  vmap::world V = vmap::read(ofile);
  geo_data data;
  io::in(ifile, data);


  // put objects
  if (cl == "POI"){
    split_labels(V);

    // remove old objects:
    vmap::world::iterator i=V.begin();
    while (i!=V.end()){
      if (i->type != type) {i++; continue;}
      i=V.erase(i);
    }

    for (auto const & pts:data.wpts) {
      for (auto const & pt:pts) {
        vmap::object o;
        dLine l;
        l.push_back(pt);
        o.push_back(l);
        if (wpts_pref != 0) {
          if (pt.name.size()>0 && pt.name[0]==wpts_pref)
            o.text=pt.name.substr(1);
        }
        else
          o.text=pt.name;

        o.type=type;
        V.push_back(o);
      }
    }
    join_labels(V);
  }
  else {

    // remove old objects:
    vmap::world::iterator i=V.begin();
    while (i!=V.end()){
      if (i->type != type) {i++; continue;}
      if (skip_names && i->text != "") {i++; continue;}
      i=V.erase(i);
    }

    for (auto const & t:data.trks) {
      vmap::object o;
      if (!skip_names) o.text=t.comm;
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


