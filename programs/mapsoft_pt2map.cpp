#include "geo/geo_convs.h"
#include "geo_io/io.h"
#include "options/read_conf.h"

// command line interface to convs::pt2pt conversions

using namespace std;
using namespace convs;

void usage(){
  cerr << "\n"
       << "Read points from stdin and converts them into map coordinates"
       << "usage: convs_pt2map  <maps>\n"
       << "\n"
  ;
  exit(1);
}

int main(int argc, char** argv){

  Options opts;

  if (!read_conf(argc, argv, opts)) usage();
  if (opts.exists("help")) usage();

  StrVec infiles = opts.get("cmdline_args", StrVec());
  geo_data world;
  for(StrVec::const_iterator i=infiles.begin(); i!=infiles.end(); i++)
    io::in(*i, world, opts);

  while (!cin.eof()){
    dPoint p;
    cin >> p.y >> p.x;
    for (vector<g_map>::iterator m=world.maps.begin(); m!=world.maps.end(); m++){
      if (! point_in_rect (p, m->range())) continue;
      convs::map2pt cnv(*m, Datum("wgs84"), Proj("lonlat"), Options());
      cnv.bck(p);
      cout << m->file << " "  << int(p.x) << " " << int(p.y) << "\n";
    }
  }
}
