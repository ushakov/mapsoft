#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "options/m_getopt.h"
#include "2d/line_utils.h"
#include "tmap.h"


using namespace std;
void usage(){
  const char * prog = "tilevmap_get";
  cerr
     << prog << " -- get region from the tiled vmap.\n"
     << "  usage: " << prog << " [<input_options>] <map dir> [<input_options>]\\n"
     << "         [ (--out|-o) <output_file> [<output_options>] ]\n"
     << "  options:\n"
     << "   -r --range    -- set range (lonlat wgs84)\n"
     << "   -p --printe   -- print tile names\n"
     << "   -v --verbose  -- be verbose\n"
  ;
  exit(1);
}

static struct option in_options[] = {
  {"range",          1, 0, 'r'},
  {"out",            0, 0, 'o'},
  {"print",          0, 0, 'p'},
  {"verbose",        0, 0, 'v'},
  {0,0,0,0}
};



main(int argc, char **argv){

/// PARSE CMDLINE

  if (argc==1) usage();

  // options before map dir
  Options OI = parse_options(&argc, &argv, in_options, "out");

  if (OI.exists("out") || (argc<1)){
    cerr << "no input map\n";
    exit(1);
  }

  const char * map_dir = argv[0];

  // options after map_dir
  Options OI1 = parse_options(&argc, &argv, in_options, "out");
  OI.insert(OI1.begin(), OI1.end());

  const char * out_file = NULL;
  Options OO;

  if (OI1.exists("out") && (argc>0)){
    out_file = argv[0];
    // options after output file
    Options OO = parse_options(&argc, &argv, in_options);
  }

/// READ MAP INFO

  vmap::world V;
  double tsize = read_tmap_data(V, map_dir);

/// GET TILE RANGE

  dRect range = OI.get<dRect>("range");
  int verbose = OI.get<int>("verbose",0);
  int print = OI.get<int>("print",0);

  if (range.empty()){
    cerr << "Error: empty range. Use -r option.\n";
    exit(1);
  }

  dRect trange = rect_pump_to_int(range/tsize);

  if (verbose){
    cerr << "reading map from:  " << map_dir << "\n"
         << "map tile size:     " << tsize << "\n"
         << "lonlat range:      " << range << "\n"
         << "tile range:        " << trange << "\n"
         << "writing result to: " << out_file << "\n"
    ;
  }

  for (int j=0; j<trange.h; j++){
    for (int i=0; i<trange.w; i++){
      ostringstream ss;
      ss << map_dir << "/" << "t." << i+trange.x
         << "." << j+trange.y << ".vmap";
      string fname = ss.str();
      ifstream test(fname.c_str());
      if (!test.good()){
        if (verbose) cerr << "skipping " << fname << "\n";
        continue;
      }
      if (print) cout << fname << "\n";
      if (out_file) V.add(vmap::read(fname.c_str()));
    }
  }

  if (out_file){
    join_objects(V, 1e-4);
    join_labels(V);
    create_labels(V);
    move_pics(V);

    // set correct name and border
    ostringstream sn;
    sn << tsize << " " << trange;
    V.name = sn.str();
    V.brd = rect2line(range);

    if (!vmap::write(out_file, V, OO)) exit(1);
  }
  exit(0);
}