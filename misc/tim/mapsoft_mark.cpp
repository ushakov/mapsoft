#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <cstring>
#include <string>
#include <list>
#include <vector>

#include <sys/stat.h>
#include <math.h>

#include "geo_io/io.h"
#include "geo/geo_convs.h"
#include "options/m_getopt.h"
#include "utils/err.h"

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

  cerr << fname <<  " -- generate distance measuring waypoints for every kilometer of input tracks\n"
       << head << "Usage:\n"
       << "\t"<< fname << " <options> <track files> -o <output waypoint file>\n"
       << head << "Track input (format is determined by file extension):\n"
       << "  *.xml -- mapsoft native XML-like format\n"
       << "  *.plt -- OziExplorer\n"
       << "  *.gpx -- GPX format\n"
       << "  *.gu  -- old garmin-utils format\n"
       << "  *.zip -- zipped files\n"
       << "  gps: --  read data from Garmin GPS via autodetected serial device\n"
       << "  <character device> -- read data from Garmin GPS via serial device\n"
       << "Look at " << bl << "mapsoft_convert" << br << " help for up-to-date list\n"
       << head << "Waypoints output (format is determined by file extension):\n"
       << "  *.xml -- mapsoft native XML-like format\n"
       << "  *.wpt -- OziExplorer format\n"
       << "  *.zip -- zipped OziExplorer files\n"
       << "  *.gpx -- GPX format\n"
       << "  *.kml -- Google KML format\n"
       << "  *.kmz -- zipped kml\n"
       << "  *.gu  -- old garmin-utils format\n"
       << " gps: --  send data to Garmin GPS via autodetected serial device\n"
       << " <character device> -- send data to Garmin GPS via serial device\n"
       << "Look at " << bl << "mapsoft_convert" << br << " help for up-to-date list\n"
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


void add(g_waypoint_list &list, dPoint &tp, double dst, Options &opts) {
  convs::pt2wgs tolonlat(Datum("wgs84"), Proj("tmerc"), opts);

  ostringstream dst_str;
  dst_str << dst;
  g_waypoint wp;
  wp.x = tp.x;
  wp.y = tp.y;
  wp.name = dst_str.str();
  tolonlat.frw(wp);

  list.push_back(wp);
}

int main(int argc, char *argv[]) {
  const char *program_name = argv[0];

  // разбор командной строки
  if (argc==1) usage(program_name);

  vector<string> infiles;
  Options opts = parse_options_all(&argc, &argv, options, OPT_ALL, infiles);

  if (opts.exists("help")) usage(program_name);
  if (opts.exists("pod")) usage(program_name, true);

  if (!opts.exists("out")){
    cerr << "No output files specified\n";
    exit(1);
  }

  string outfile = opts.get("out", string());

  // чтение файлов
  geo_data world, result;
  vector<string>::const_iterator i;
  for(i=infiles.begin(); i!=infiles.end(); i++){
    try {io::in(*i, world, opts);}
    catch (MapsoftErr e) {cerr << e.str() << endl;}
  }


  vector<g_track>::const_iterator t;
  for(t=world.trks.begin(); t!=world.trks.end(); t++) {
    g_waypoint_list wp_list;

    convs::pt2wgs pc(Datum("wgs84"), Proj("tmerc"), opts);
    double len = 0; 
    double active_len = 0;
    time_t active_time = 0;
    double next = 1000;
    g_trackpoint pp, tp;
    vector<g_trackpoint>::const_iterator p;

    p=(*t).begin();
    tp = *p;
    pc.bck(tp); // координаты -- в tmerc

    add(wp_list, tp, 0, opts);

    for(p++; p!=(*t).end(); p++) {
      pp = tp;
      tp = *p;
      pc.bck(tp); // координаты -- в tmerc

      double delta_len = hypot(tp.x - pp.x, tp.y - pp.y);

      while (len + delta_len > next) {
        dPoint where;
        double frac = (next - len) / delta_len;
        where = pp * (1 - frac) + tp * frac;
        add(wp_list, where, next/1000, opts);
        next += 1000;
      }
      len += delta_len;
    }
    add(wp_list, pp, len/1000.0, opts);

    result.wpts.push_back(wp_list);
  }

  try{io::out(outfile, result, opts);}
  catch (MapsoftErr e) {cerr << e.str() << endl;}
}
