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

using namespace std;

void usage(const char *fname){
    cerr << "Usage: " << fname << " <in1.plt> <in2.plt> ... -o <out.csv>\n";
    exit(0);
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
  if (argc < 4) usage(argv[0]);

  Options opts;
  list<string> infiles;
  string outfile = "";

// разбор командной строки
  for (int i=1; i<argc; i++){ 

    if ((strcmp(argv[i], "-h")==0)||
        (strcmp(argv[i], "-help")==0)||
        (strcmp(argv[i], "--help")==0)) usage(argv[0]);

    if (strcmp(argv[i], "-o")==0){
      if (i==argc-1) usage(argv[0]);
      i+=1;
      outfile = argv[i];
      continue;
    }

//    if (strcmp(argv[i], "-O")==0){
//      if (i==argc-1) usage(argv[0]);
//      i+=1;
//      opts.put_string(argv[i]);
//      continue;
//    }

    infiles.push_back(argv[i]);
  }

  if (outfile == "") usage(argv[0]);

  ofstream out(outfile.c_str()); 

  geo_data world;
  list<string>::const_iterator i;
  for(i=infiles.begin(); i!=infiles.end(); i++){
    try{io::in(*i, world, opts);}
    catch (MapsoftErr e) {cerr << e.str() << endl;}
  }

  int n = 0;
  double len = 0; 
  vector<g_track>::const_iterator t;

  convs::pt2pt pc(Datum("wgs84"), Proj("tmerc"), Options(),
                  Datum("wgs84"), Proj("lonlat"), Options());

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
