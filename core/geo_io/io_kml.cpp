#include <fstream>
#include <iostream>
#include <iomanip>

#include <vector>
#include <string>
#include <map>

#include "io_kml.h"

#include <math.h>

#include "utils/err.h"

namespace kml {
using namespace std;

// Записывает в KML-файл треки и точки
// Не записывает карты! (хм, а может, надо?)
void write_file (const char* filename, const geo_data & world, const Options & opt){

  if (opt.exists("verbose")) cerr <<
    "Writing data to KML file " << filename << endl;

  ofstream f(filename);
  if (!f.good()) throw MapsoftErr("GEO_IO_KML_OPENW")
                << "Can't open KML file " << filename << " for writing";

  f << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  f << "<kml xmlns=\"http://earth.google.com/kml/2.1\">" << endl;
  f << "  <Document>" << endl;

  for (int i = 0; i < world.wpts.size(); i++) {
    f << "  <Folder>" << endl;
    f << "    <name>WPTS_" << i << "</name>" << endl;
    g_waypoint_list::const_iterator wp;
    for (wp = world.wpts[i].begin(); wp != world.wpts[i].end(); ++wp) {
      f << "    <Placemark>" << endl;
      f << "      <name><![CDATA[" << wp->name << "]]></name>" << endl;
      f << "      <description><![CDATA[" << wp->comm << "]]></description>" << endl;
      f << "      <Point>" << endl;
      f << "        <coordinates>" << wp->x << "," << wp->y << "," << wp->z << "</coordinates>" << endl;
      f << "      </Point>" << endl;
      f << "    </Placemark>" << endl;
    }
    f << "  </Folder>" << endl;
  }

  for (int i = 0; i < world.trks.size(); ++i) {
    f << "  <Placemark>" << endl;
    f << "    <description><![CDATA[" << world.trks[i].comm << "]]></description>" << endl;
    f << "    <MultiGeometry>" << endl;
    g_track::const_iterator tp;
    for (tp = world.trks[i].begin(); tp != world.trks[i].end(); ++tp) {
      if (tp->start || tp == world.trks[i].begin()) {
        if (tp != world.trks[i].begin()) {
          f << "        </coordinates>" << endl;
          f << "      </LineString>" << endl;
        }
        f << "      <LineString>" << endl;
        f << "        <tesselate>1</tesselate>" << endl;
        f << "        <coordinates>" << endl;
      }
      f << "          " << tp->x << "," << tp->y << "," << tp->z << endl;
    }
    f << "        </coordinates>" << endl;
    f << "      </LineString>" << endl;
    f << "    </MultiGeometry>" << endl;
    f << "  </Placemark>" << endl;
  }

  f << "  </Document>" << endl;
  f << "</kml>" << endl;

  if (!f.good()) throw MapsoftErr("GEO_IO_KML_WRITE")
                  << "Can't write data to KML file " << filename;
}

} // namespace
