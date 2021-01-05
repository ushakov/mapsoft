#include <string>
#include <cstring>
#include "options/m_getopt.h"
#include "vmap/vmap.h"
#include "vmap/zn.h"
#include "fig/fig.h"
#include "geo_io/io.h"

using namespace std;

// extract objects of a single type
// and write as geodata

int
main(int argc, char **argv){
try{

  if (argc!=5) {
    std::cerr << "usage: vmap_get_track <class> <type> <in map> <out geodata>\n";
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

  vmap::world V = vmap::read(ifile);
  geo_data data;
  g_waypoint_list wpts;

  for (auto const & o:V) {
    if (o.type != type) continue;
    if (o.size()<1) continue;
    if (cl == "POI" && o[0].size()>0){
      for (auto const & l:o) {
        for (auto const & p:l) {
          g_waypoint pt;
          pt.x = p.x;
          pt.y = p.y;
          pt.name = o.text;
          wpts.push_back(pt);
        }
      }
    }
    else {
      for (auto const & l:o) {
        g_track tr;
        for (auto const & p:l) {
          g_trackpoint pt;
          pt.x = p.x;
          pt.y = p.y;
          tr.push_back(pt);
        }
        data.trks.push_back(tr);
      }
    }
  }
  if (wpts.size()) data.wpts.push_back(wpts);

  io::out(ofile, data, Options());
}
catch (Err e) {
  cerr << e.get_error() << endl;
  return 1;
}
return 0;
}


