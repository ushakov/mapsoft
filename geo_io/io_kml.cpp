#include <fstream>
#include <iostream>
#include <iomanip>

#include <vector>
#include <string>
#include <map>

#include "io.h"
#include "geo_enums.h"
#include "geo_names.h"
#include "geo_convs.h"
#include "../utils/mapsoft_options.h"

#include <math.h>

#include "../jeeps/gpsmath.h"
#include "../jeeps/gpsdatum.h"


namespace kml {

    // Записывает в KML-файл треки и точки
    // Не записывает карты! (хм, а может, надо?)
    bool write_file (const char* filename, const geo_data & world, const Options & opt){
	std::ofstream f(filename);
	if (!f.good()) return false;

	f << "<?xml version=\"1.0\" encoding=\"koi8-r\"?>" << std::endl;
	f << "<kml xmlns=\"http://earth.google.com/kml/2.1\">" << std::endl;

	for (int i = 0; i < world.wpts.size(); i++) {
	    f << "  <Folder>" << std::endl;
	    f << "    <name>WPTS_" << i << "</name>" << std::endl;
	    for (g_waypoint_list::const_iterator wp = world.wpts[i].begin();
		 wp != world.wpts[i].end(); ++wp) {
		f << "    <Placemark>" << std::endl;
		f << "      <name><![CDATA[" << wp->name << "]]></name>" << std::endl;
		f << "      <description><![CDATA[" << wp->comm << "]]></description>" << std::endl;
		f << "      <Point>" << std::endl;
		f << "        <coordinates>" << wp->x << "," << wp->y << "</coordinates>" << std::endl;
		f << "      </Point>" << std::endl;
		f << "    </Placemark>" << std::endl;
	    }
	    f << "  </Folder>" << std::endl;
	}

	for (int i = 0; i < world.trks.size(); ++i) {
	    f << "  <Placemark>" << std::endl;
	    f << "    <description><![CDATA[" << world.trks[i].comm << "]]></description>" << std::endl;
	    f << "    <MultiGeometry>" << std::endl;
	    for (g_track::const_iterator tp = world.trks[i].begin(); tp != world.trks[i].end(); ++tp) {
		if (tp->start || tp == world.trks[i].begin()) {
		    if (tp != world.trks[i].begin()) {
			f << "        </coordinates>" << std::endl;
			f << "      </LineString>" << std::endl;
		    }
		    f << "      <LineString>" << std::endl;
		    f << "        <tesselate>1</tesselate>" << std::endl;
		    f << "        <coordinates>" << std::endl;
		}	
		f << "          " << tp->x << "," << tp->y << std::endl;
	    }
	    f << "        </coordinates>" << std::endl;
	    f << "      </LineString>" << std::endl;
	    f << "    </MultiGeometry>" << std::endl;
	    f << "  </Placemark>" << std::endl;
	}
	
	f << "</kml>" << std::endl;
	
	return f.good();
    }

}
