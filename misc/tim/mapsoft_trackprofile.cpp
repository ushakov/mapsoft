#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <list>
#include <stdexcept>
#include <cmath>
#include <cstring>

#include "2d/line.h"

#include "2d/line_utils.h"
#include "2d/line_polycrop.h"

#include "geo_io/io.h"
#include "geo/geo_data.h"
#include "geo/geo_convs.h"

#include "options/m_getopt.h"
#include "err/err.h"

using namespace std;

#define OPT1  1  // common options
#define OPT2  2  // common mapsoft options
#define OPT3  4  // program options
#define OPT_ALL  (OPT1 | OPT2 | OPT3)

static struct ext_option options[] = {
  {"help",                  0,'h', OPT1, "show this message"},
  {"pod",                   0,  0, OPT1, "show this message as POD template"},
  {"verbose",               0,'v', OPT1, "be verbose\n"},

  {"geom",          1,  0, OPT2, ""},
  {"datum",         1,  0, OPT2, ""},
  {"proj",          1,  0, OPT2, ""},
  {"lon0",          1,  0, OPT2, ""},
  {"wgs_geom",      1,  0, OPT2, ""},
  {"wgs_brd",       1,  0, OPT2, ""},
  {"nom",           1,  0, OPT2, ""},

  {"out",                   1,'o', OPT3, "output file name"},

  {0,0,0,0}
};

void usage(const char *fname, bool pod=false){

  string head = pod? "\n=head1 ":"\n";
  string bl = pod? "B<" : "";
  string br = pod? ">" : "";

  cerr << fname <<  " -- generate height profile for a set of tracks\n"
       << head << "Usage:\n"
       << "\t"<< fname << " <options> <track files> -o <output csv file>\n"
       << head << "Track input (format is determined by file extension):\n"
       << "  *.xml -- mapsoft native XML-like format\n"
       << "  *.plt -- OziExplorer\n"
       << "  *.gpx -- GPX format\n"
       << "  *.gu  -- old garmin-utils format\n"
       << "  *.zip -- zipped files\n"
       << "  gps: --  read data from Garmin GPS via autodetected serial device\n"
       << "  <character device> -- read data from Garmin GPS via serial device\n"
       << "Look at " << bl << "mapsoft_convert" << br << " help for up-to-date list\n"
       << head << "CSV output format\n"
       << "  - trackpoint number\n"
       << "  - date time\n"
       << "  - height\n"
       << "  - x coordinate\n"
       << "  - y coordinate\n"
       << "  - distance from start of track\n"
       << "  - track comment\n"
  ;

  cerr << head << "Program options:\n";
  print_options(options, OPT3, cerr, pod);
  cerr << head << "Common options:\n";
  print_options(options, OPT1, cerr, pod);
  cerr << head << "Common mapsoft options:\n";
  print_options(options, OPT2, cerr, pod);
  cerr << "Look at " << bl << "mapsoft_convert" << br << " help for up-to-date list\n";

  exit(1);
}

//координаты угла единичного квадрата по его номеру
iPoint crn (int k){
  k%=4;
  return iPoint(k/2, (k%3>0)?1:0);
}
//направление следующей за углом стороны (единичный вектор)
iPoint dir (int k){
  return crn(k+1)-crn(k);
}


main(int argc, char** argv){
  const char *program_name = argv[0];

  // разбор командной строки
  if (argc==1) usage(program_name);

  vector<string> infiles;
  Options opts = parse_options_all(&argc, &argv, options, OPT_ALL, infiles);

  if (opts.exists("help")) usage(program_name);
  if (opts.exists("pod")) usage(program_name, true);

  if (infiles.empty()) {
    cerr << "No track files specified\n";
    exit(1);
  }

  if (!opts.exists("out")){
    cerr << "No output files specified\n";
    exit(1);
  }

  string outfile = opts.get("out", string());

  ofstream out(outfile.c_str()); 

  geo_data world;
  vector<string>::const_iterator i;
  for(i=infiles.begin(); i!=infiles.end(); i++){
    try{io::in(*i, world, opts);}
    catch (Err e) {cerr << e.get_error() << endl;}
  }

  int n = 0;
  double len = 0; 
  vector<g_track>::const_iterator t;

  convs::pt2wgs pc(Datum("wgs84"), Proj("tmerc"), opts);

  for(t=world.trks.begin(); t!=world.trks.end(); t++) {
    double active_len = 0;
    time_t active_time = 0;
    g_trackpoint pp;
    vector<g_trackpoint>::const_iterator p;
    for(p=(*t).begin(); p!=(*t).end(); p++) {
      n++;
      g_trackpoint tp = *p;
      pc.bck(tp); // координаты -- в tmerc

      if (p != (*t).begin()) {
         double delta_len = hypot(tp.x - pp.x, tp.y - pp.y);
         len += delta_len;
         if (tp.t != pp.t && delta_len * 3600 / (tp.t.value - pp.t.value) / 1000 >= 1.0) {
            active_len += delta_len;
            active_time += tp.t.value - pp.t.value;
         }
      }

      short h = tp.z;
      
      out << n << "|" << tp.t.date_str() << " " << tp.t.time_str() << "|";
      if (h>0) out << h;
      out << "|" << (*p).x << "|" << (*p).y << "|" << len << "|" << (*t).comm.c_str() << endl;

      pp = tp;
    }
    out << endl;
  }
}
