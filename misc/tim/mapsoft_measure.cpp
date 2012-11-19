#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <cstring>
#include <string>
#include <list>

#include <sys/stat.h>
#include <math.h>

#include "geo_io/io.h"
#include "geo/geo_convs.h"
#include "options/m_getopt.h"
#include "utils/err.h"

using namespace std;

#define OPT1  1  // common options
#define OPT2  2  // common mapsoft options
#define OPT_ALL  (OPT1 | OPT2)

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

  {0,0,0,0}
};

void usage(const char *fname, bool pod=false){

  string head = pod? "\n=head1 ":"\n";
  string bl = pod? "B<" : "";
  string br = pod? ">" : "";

  cerr << fname <<  " -- measure length, speed, etc of a set of tracks\n"
       << head << "Usage:\n"
       << "\t"<< fname << " <options> <track files>\n"
       << head << "Track input (format is determined by file extension):\n"
       << "  *.xml -- mapsoft native XML-like format\n"
       << "  *.plt -- OziExplorer\n"
       << "  *.gpx -- GPX format\n"
       << "  *.gu  -- old garmin-utils format\n"
       << "  *.zip -- zipped files\n"
       << "  gps: --  read data from Garmin GPS via autodetected serial device\n"
       << "  <character device> -- read data from Garmin GPS via serial device\n"
       << "Look at " << bl << "mapsoft_convert" << br << " help for up-to-date list\n"
  ;

  cerr << head << "Common options:\n";
  print_options(options, OPT1, cerr, pod);
  cerr << head << "Common mapsoft options:\n";
  print_options(options, OPT2, cerr, pod);
  cerr << "Look at " << bl << "mapsoft_convert" << br << " help for up-to-date list\n";

  exit(1);
}

int main(int argc, char *argv[]) {
  const char *program_name = argv[0];

  // разбор командной строки
  if (argc==1) usage(program_name);

  vector<string> infiles;
  Options opts = parse_options_all(&argc, &argv, options, OPT_ALL, infiles);

  if (opts.exists("help")) usage(program_name);
  if (opts.exists("pod")) usage(program_name, true);

  // чтение файлов
  geo_data world;
  vector<string>::const_iterator i;
  for(i=infiles.begin(); i!=infiles.end(); i++){
    try {io::in(*i, world, opts);}
    catch (MapsoftErr e) {cerr << e.str() << endl;}
  }

  // Распечатка комментариев точек
  //  vector<g_waypoint_list>::const_iterator t;
  //  for(t=world.wpts.begin(); t!=world.wpts.end(); t++) {
  //    vector<g_waypoint>::const_iterator p;
  //    for(p=(*t).begin(); p!=(*t).end(); p++)
  //       cout << (*p).comm.c_str() << endl;
  //  }


  vector<g_track>::const_iterator t;
  for(t=world.trks.begin(); t!=world.trks.end(); t++) {
    convs::pt2wgs pc(Datum("wgs84"), Proj("tmerc"), opts);
    double len = 0; 
    double active_len = 0;
    time_t active_time = 0;
    g_trackpoint pp;
    vector<g_trackpoint>::const_iterator p;
    for(p=(*t).begin(); p!=(*t).end(); p++) {
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
      pp = tp;
    }

    Time start((*t).front().t);
    Time finish((*t).back().t);
    time_t delta = finish.value - start.value;
    int delta_h = delta/3600;
    int delta_m = (delta-3600*delta_h)/60;
    int delta_s = delta-60*delta_m-3600*delta_h;

    cout << setfill('0');
    cout << (*t).comm.c_str() <<
      " Length: " << len << " m" <<
      " Start: " << start.date_str() << " " << start.time_str() <<
      " Finish: " << finish.date_str() << " " << finish.time_str() <<
      " Time: " << setw(2) << delta_h << ":" << setw(2) << delta_m << ":" << setw(2) << delta_s;
    if (delta != 0)
      cout << " AVG: " << len * 3600 / delta / 1000 << " km/h";
    if (active_time != 0)
      cout << " Active length: " << active_len << " m" <<
        " Active AVG: " << active_len * 3600 / active_time / 1000 << " km/h";
    cout << endl;
  }
}
