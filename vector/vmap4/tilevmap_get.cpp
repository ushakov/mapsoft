#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "options/m_getopt.h"
#include "2d/line_utils.h"
#include "tmap.h"

const double JE=1e-4; // accuracy for join objects (degree)
const double BE=1e-4; // accuracy for border calculations (degree)

using namespace std;
void usage(){
  const char * prog = "tilevmap_get";
  cerr
     << prog << " -- get region from the tiled vmap.\n"
     << "  usage: " << prog << " [<input_options>] <map dir> [<input_options>]\\n"
     << "         [ (--out|-o) <output_file> [<output_options>] ]\n"
     << "  options:\n"
     << "   -m --map <name>          -- get rectangular map\n"
     << "   -l --label_source <src>  -- use label source ([rmaps]/tiles/none)"
     << "   -r --range <range>       -- set range (lonlat wgs84)\n"
     << "   -p --print               -- print tile names\n"
     << "   -v --verbose             -- be verbose\n"
  ;
  exit(1);
}

static struct option in_options[] = {
  {"map",            1, 0, 'm'},
  {"label_source",   1, 0, 'l'},
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

/// READ TMAP INFO

  vmap::world V;
  double tsize = read_tmap_data(V, map_dir);

/// GET TILE RANGE

  dRect range = OI.get<dRect>("range");
  int verbose = OI.get<int>("verbose",0);
  int print = OI.get<int>("print",0);
  string map = OI.get<string>("map","");
  string label_source = OI.get<string>("label_source","rmaps");
  if (label_source != "rmaps" &&
      label_source != "tiles" &&
      label_source != "none"){
    cerr << "Error: bad label_source value.\n";
    exit(1);
  }

  // get tmerc range for given map
  dRect geom = read_rmap_geom(map_dir, map);
  string title = read_rmap_title(map_dir, map);

  dLine brd;
  // if range is not set explicitly, set it from the map geom.
  if (range.empty() && !geom.empty()){
    Options O;
    O.put<double>("lon0", convs::lon_pref2lon0(geom.x));
    geom.x=convs::lon_delprefix(geom.x);
    convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(),
                     Datum("pulk"), Proj("tmerc"), O);
    range=cnv.bb_bck(geom, BE);
    brd = cnv.line_bck(rect2line(geom), BE);
  }

  if (range.empty()){
    if (map!="") cerr << "Error: can't get geometry for a map " << map << "\n";
    else cerr << "Error: empty range. Use -m or -r option.\n";
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
        if (verbose) cerr << "skipping tile: " << fname << "\n";
        continue;
      }

      if (print) cout << fname << "\n";
      if (out_file) V.add(vmap::read(fname.c_str()));
    }
  }

  if (out_file){
    join_objects(V, JE);

    if (label_source=="rmaps"){
      remove_labels(V);
      vmap::world L = read_rmap_labels(map_dir, map);
      V.lbuf.swap(L.lbuf);
    }
    else if (label_source=="none"){
      remove_labels(V);
    }

    join_labels(V);
    create_labels(V);
    move_pics(V);

    // set correct name
    ostringstream sn;
    sn << tsize << " " << trange << " " << map;
    V.name = sn.str();

    // set border if it is not set yet
    if (brd.size()==0) brd = rect2line(range);
    V.brd.swap(brd);

    if (!vmap::write(out_file, V, OO)) exit(1);
  }
  exit(0);
}
