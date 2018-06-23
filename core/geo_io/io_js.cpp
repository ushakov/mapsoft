#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "io_js.h"
#include <cstring>
#include "err/err.h"

namespace js {

using namespace std;

/* see:
 * http://geojson.org/
 * https://tools.ietf.org/html/rfc7946
 * https://leafletjs.com/examples/geojson/
 */

void
write_file (const char* filename, const geo_data & world, const Options & opt){
  ofstream f(filename);

  if (opt.exists("verbose")) cerr <<
    "Writing data to JS file " << filename << endl;

  if (!f.good()) throw Err() << "Can't open file " << filename << " for writing";

  // write some information
  dRect rng = world.range();
  f << "geojson_data_cnt=["
    << fixed << setprecision(6) << rng.CNT().x << "," << rng.CNT().y << "];\n";
  f << "geojson_data_tlc=["
    << fixed << setprecision(6) << rng.TLC().x << "," << rng.TLC().y << "];\n";
  f << "geojson_data_brc=["
    << fixed << setprecision(6) << rng.BRC().x << "," << rng.BRC().y << "];\n";

  // start a FeatureCollection
  f << "geojson_data={\"type\": \"FeatureCollection\", \"features\": [\n";

  bool first_feature=true; // first-feature marker

  // tracks
  // Each track is a feature with MultiLineString objects.
  // First we write tracks to have them below points on leaflet maps.
  for (int i = 0; i < world.trks.size(); ++i) {
    if (!first_feature) f << ",\n";
    f << "  { \"type\": \"Feature\",\n"
      << "    \"properties\": {\n"
      << "      \"name\": \"" << world.trks[i].comm << "\"\n"
      << "    },\n"
      << "    \"geometry\": {\n"
      << "      \"type\":\"MultiLineString\",\n"
      << "      \"coordinates\":[\n"
      << "         [";
    for (g_track::const_iterator tp = world.trks[i].begin(); tp != world.trks[i].end(); ++tp) {
      if (tp != world.trks[i].begin())
        f << (tp->start? "],\n         [":",");
      f << fixed << setprecision(6) << "[" << tp->x << "," << tp->y << "]";
    }
    f << "]\n";
    f << "      ]\n"
      << "    }\n"
      << "  }";
    first_feature=false;
  }

  // waypoints
  for (int i = 0; i < world.wpts.size(); i++) {
    for (g_waypoint_list::const_iterator wp = world.wpts[i].begin();
                                         wp != world.wpts[i].end(); ++wp) {
       if (!first_feature) f << ",\n";
       f << "  { \"type\": \"Feature\",\n"
         << "    \"properties\": {\n"
         << "      \"name\": \"" << wp->name << "\",\n"
         << "      \"cmt\":  \"" << wp->comm << "\",\n"
         << "      \"ele\":  \"" << fixed << setprecision(1) << wp->z << "\",\n"
         << "      \"time\": \"" << wp->t.gpx_str() << "\"\n"
         << "    },\n"
         << "    \"geometry\": {\n"
         << "      \"type\": \"Point\",\n"
         << "      \"coordinates\": [" 
         << fixed << setprecision(6) << wp->x << "," << wp->y << "]\n"
         << "    }\n"
         << "  }";
       first_feature=false;
    }
  }

  // close FeatureCollection
  f << "\n]};\n";

  if (!f.good()) throw Err() << "Can't write to file " << filename;
}



void
read_file(const char* filename, geo_data & world, const Options & opt) {

  throw Err() << "Reading is not supported";
}

} // namespace
