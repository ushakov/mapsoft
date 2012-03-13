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
#include "options/read_conf.h"

using namespace std;

void usage(const char *fname){
  cerr << "\nGenerate distance measuring waypoints for every kilometer of input tracks\n";
  cerr << "Usage: "<< fname << " <in1> ... <inN> -O option[=value] ... -o <out>\n";
  exit(0);
}


void add(g_waypoint_list &list, dPoint &tp, double dst, Options &opts) {
   static convs::pt2pt tolonlat = convs::pt2pt(Datum("wgs84"), Proj("lonlat"), opts, Datum("wgs84"), Proj("tmerc"), opts);

   ostringstream dst_str;
   dst_str << dst;
   g_waypoint wp;
   wp.x = tp.x;
   wp.y = tp.y;
   wp.name = dst_str.str();
   tolonlat.bck(wp);

   list.push_back(wp);
}

int main(int argc, char *argv[]) {

  Options opts;

// разбор командной строки
  if (!read_conf(argc, argv, opts)) usage(argv[0]);
  if (opts.exists("help")) usage(argv[0]);

  StrVec infiles = opts.get("cmdline_args", StrVec());

  string outfile = opts.get("out", string());
  if (outfile == "") usage(argv[0]);

// чтение файлов

  geo_data world, result;
  StrVec::const_iterator i;
  for(i=infiles.begin(); i!=infiles.end(); i++){
    try {io::in(*i, world, opts);}
    catch (MapsoftErr e) {cerr << e.str() << endl;}
  }


  vector<g_track>::const_iterator t;
  for(t=world.trks.begin(); t!=world.trks.end(); t++) {
     g_waypoint_list wp_list;

    convs::pt2pt pc(Datum("wgs84"), Proj("tmerc"), opts, Datum("wgs84"), Proj("lonlat"), opts);
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
