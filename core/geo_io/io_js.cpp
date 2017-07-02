#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "io_js.h"
#include <cstring>
#include "err/err.h"

namespace js {

using namespace std;

void
write_file (const char* filename, const geo_data & world, const Options & opt){
  ofstream f(filename);

  if (opt.exists("verbose")) cerr <<
    "Writing data to JS file " << filename << endl;

  if (!f.good()) throw Err() << "Can't open file " << filename << " for writing";

/*  for (int i = 0; i < world.wpts.size(); i++) {
    for (g_waypoint_list::const_iterator wp = world.wpts[i].begin();
                                         wp != world.wpts[i].end(); ++wp) {
       f << fixed << setprecision(6)
         << "<wpt lat=\"" << wp->y
         << "\" lon=\""  << wp->x << "\">" << endl
         << setprecision(1)
         << "   <ele>"  << wp->z << "</ele>" << endl
         << "   <name>" << wp->name << "</name>" << endl
         << "   <cmt>"  << wp->comm << "</cmt>" << endl
         << "   <time>" << wp->t.gpx_str() << "</time>" << endl
         << "</wpt>" << endl;
    }
  }
*/

  for (int i = 0; i < world.trks.size(); ++i) {
    f << "track" << i << "={ \"type\": \"Feature\",\n";
    f << "\"properties\": {\n";
    f << "\"name\": \"" << world.trks[i].comm << "\"\n";
    f << "},\n";
    f << "\"geometry\": {\n";
    f << "\"type\":\"LineString\", \"coordinates\":[\n";
    for (g_track::const_iterator tp = world.trks[i].begin(); tp != world.trks[i].end(); ++tp) {
      if (tp != world.trks[i].begin()) f << ",\n";
      f << fixed << setprecision(6)
        << "[" << tp->x << "," << tp->y << "]";
    }
    f << "]\n}};\n";
  }
  if (!f.good()) throw Err() << "Can't write to file " << filename;
}



void
read_file(const char* filename, geo_data & world, const Options & opt) {

  throw Err() << "Reading is not supported";
}

} // namespace
