#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <cstring>
#include <string>
#include <list>

#include <sys/stat.h>
#include <math.h>

#include "../geo_io/io.h"
#include "../geo_io/geo_convs.h"

using namespace std;

void usage(const char *fname){
  cerr << "Usage: "<< fname << " <in1> ... <inN> -O option1[=value1] ...\n";
  exit(0);
}
int main(int argc, char *argv[]) {

  Options opts;
  list<string> infiles;

// разбор командной строки
  for (int i=1; i<argc; i++){ 

    if ((strcmp(argv[i], "-h")==0)||
        (strcmp(argv[i], "-help")==0)||
        (strcmp(argv[i], "--help")==0)) usage(argv[0]);

    if (strcmp(argv[i], "-O")==0){
      if (i==argc-1) usage(argv[0]);
      i+=1;
      opts.put_string(argv[i]);
      continue;
    }

    infiles.push_back(argv[i]);
  }

// чтение файлов

  geo_data world;
  list<string>::const_iterator i;
  for(i=infiles.begin(); i!=infiles.end(); i++) io::in(*i, world, opts);

// Распечатка комментариев точек
//  vector<g_waypoint_list>::const_iterator t;
//  for(t=world.wpts.begin(); t!=world.wpts.end(); t++) {
//    vector<g_waypoint>::const_iterator p;
//    for(p=(*t).begin(); p!=(*t).end(); p++)
//       cout << (*p).comm.c_str() << endl;
//  }


  vector<g_track>::const_iterator t;
  for(t=world.trks.begin(); t!=world.trks.end(); t++) {
    convs::pt2ll pc(Datum("wgs84"), Proj("tmerc"), Options());
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
