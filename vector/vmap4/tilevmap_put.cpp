#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "options/m_getopt.h"
#include "2d/line_utils.h"
#include "2d/line_rectcrop.h"
#include "tmap.h"


using namespace std;
void usage(){
  const char * prog = "tilevmap_put";
  cerr
     << prog << " -- put tiles to the tiled vmap.\n"
     << "  usage: " << prog << " [<input_options>] <in file> [<input_options>]\\n"
     << "         (--out|-o) <map dir> [<output_options>]\n"
     << "  options:\n"
     << "  -a --add                 -- add objects to existing tiles\n"
     << "  -r --autorange           -- don't check input map range\n"
     << "  -l --label_source <src>  -- use label source (rmaps/tiles/[both]/none)"
     << "    (rmaps label source does not work in autorange mode)\n"
     << "  -b --backup              -- save old tiles in .bak files\n"
     << "  -v --verbose             -- be more verbose\n"
  ;
  exit(1);
}

static struct option in_options[] = {
  {"add",            0, 0, 'a'},
  {"autorange",      0, 0, 'r'},
  {"label_source",   1, 0, 'l'},
  {"backup",         0, 0, 'b'},
  {"verbose",        0, 0, 'v'},
  {"out",            0, 0, 'o'},
  {0,0,0,0}
};

main(int argc, char **argv){

/// PARSE CMDLINE

  if (argc==1) usage();

  // options before filename
  Options OI = parse_options(&argc, &argv, in_options, "out");

  if (OI.exists("out") || (argc<1)){
    cerr << "no input file\n";
    exit(1);
  }

  const char * in_file = argv[0];

  // options after filename
  Options OI1 = parse_options(&argc, &argv, in_options, "out");

  if (!OI1.exists("out") || (argc<1)){
    cerr << "no output filename\n";
    exit(1);
  }

  const char * map_dir = argv[0];

  OI.insert(OI1.begin(), OI1.end());

  // options after map_dir

  Options OO = parse_options(&argc, &argv, in_options);


/// READ MAP INFO

  vmap::world V0;
  double tsize = read_tmap_data(V0, map_dir);

/// GET TILE RANGE

  iRect trange;
  string map;

  int verbose = OI.get<int>("verbose",0);
  int autorange = OI.get<int>("autorange",0);
  int backup = OI.get<int>("backup",0);
  int add = OI.get<int>("add",0);
  string label_source = OI.get<string>("label_source","both");
  if (label_source != "rmaps" &&
      label_source != "tiles" &&
      label_source != "both" &&
      label_source != "none"){
    cerr << "Error: bad label_source value.\n";
    exit(1);
  }

  vmap::world V = vmap::read(in_file);
  split_labels(V);

  if (!autorange){
    double ntsize=0;
    istringstream sn(V.name);
    sn >> ntsize >> trange >> map;

    if (trange.empty() || (ntsize==0)){
      cerr << "bad input file - can't get tsize or trange values\n";
      exit(1);
    }

    if (tsize!=ntsize){
      cerr << "wrong tsize: " << tsize << " != " <<  ntsize << "\n";
      exit(1);
    }

    // write labels to rmap
    if ((label_source == "rmaps" || label_source == "both") && map != ""){
      vmap::world L;
      L.lbuf.swap(V.lbuf);
      string lname=map_dir;
      lname+="/"+map+"/labels.vmap";
      vmap::write(lname.c_str(), L, Options());
      L.lbuf.swap(V.lbuf);
    }
  }
  else { // add mode
    trange = rect_pump_to_int(V.range()/tsize);
  }
  if (verbose){
    cout << "reading from:      " << in_file << "\n"
         << "map tile size:     " << tsize << "\n"
         << "lonlat range:      " << dRect(trange)*tsize << "\n"
         << "tile range:        " << trange << "\n"
         << "writing to:        " << map_dir << "\n"
    ;
  }

  if ( label_source != "tiles" && label_source != "both") remove_labels(V);

  for (int j=0; j<trange.h; j++){
    for (int i=0; i<trange.w; i++){

      dRect crop_range = (trange.TLC() + dRect(i,j,1,1))*tsize;
      vmap::world V1(V0);

      /// crop objects
      for (vmap::world::const_iterator o=V.begin(); o!=V.end(); o++){
        bool closed = (o->get_class() == vmap::POLYGON);

        vmap::object o1 = *o;
        dMultiLine::iterator l;
        for (l = o1.begin(); l != o1.end(); l++){
          rect_crop(crop_range, *l, closed);

          dMultiLine ml = rect_split_cropped(crop_range, *l, closed);
          for (dMultiLine::iterator m=ml.begin(); m!=ml.end(); m++){
            l = o1.insert(l, *m) + 1;
          }
          l->clear();
        }
        if (o1.size()) V1.push_back(o1);
      }
      vmap::remove_empty(V1);      // remove empty objects and lines
      vmap::remove_dups(V1, 1e-5); // remove repeated points

      /// crop lbuf
      std::list<vmap::lpos_full>::iterator lb;
      for (lb=V.lbuf.begin(); lb!=V.lbuf.end(); lb++){
        if (point_in_rect(lb->ref, crop_range)) V1.lbuf.push_back(*lb);
      }

      // remove tails, clear empty lines
      vmap::remove_tails(V1, 1e-4, crop_range);


      ostringstream ss;
      ss << map_dir << "/" << "t." << i+trange.x
         << "." << j+trange.y << ".vmap";
      string fname = ss.str();

      // read old tile
      vmap::world V_old;
      ifstream test(fname.c_str());
      if (test.good()) V_old = vmap::read(fname.c_str());

      if (add) { V_old.add(V1); V1.swap(V_old); } // swap to keep map pars!
      else vmap::fix_diff(V_old,V1, 1e-4);

      // write backup
      if (backup) rename(fname.c_str(), (fname+".bak").c_str());

      // write tile
      V1.brd=rect2line(crop_range);
      if (V1.size()) vmap::write(fname.c_str(), V1, OO);
    }
  }
  // write log
  log_action(map_dir, "PUT", trange, map);
  exit(0);
}
