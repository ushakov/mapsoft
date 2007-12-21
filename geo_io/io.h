#ifndef MAPSOFT_IO_H
#define MAPSOFT_IO_H

// function for I/O

#include <map>
#include <string>
#include <iostream>
#include "geo_data.h"
#include "../utils/rect.h"
#include "../utils/mapsoft_options.h"

namespace gps {
    bool init_gps(const char* port);
    bool get_tracks (const char* port, geo_data & world, const Options & opt);
    bool get_waypoints (const char* port, geo_data & world, const Options & opt);
    bool get_all (const char* port, geo_data & world, const Options & opt);
    bool put_track (const char* port, const g_track & tr, const Options & opt);
    bool put_waypoints (const char* port, const g_waypoint_list & wp, const Options & opt);
    bool put_all (const char* port, const geo_data & world, const Options & opt);
}

namespace gu {
    bool read_file (const char* filename, geo_data & world, const Options & opt);
    bool write_file (const char* filename, const geo_data & world, const Options & opt);
}

namespace xml {
    bool read_file (const char* filename, geo_data & world, const Options & opt);
    bool write_file (const char* filename, const geo_data & world, const Options & opt);
}

namespace kml {
//    bool read_file (const char* filename, geo_data & world, const Options & opt);
    bool write_file (const char* filename, const geo_data & world, const Options & opt);
}

namespace oe {
    bool read_file (const char* filename, geo_data & world, const Options & opt);
    bool write_plt_file (std::ostream & f, const g_track & trk, const Options & opt);
    bool write_wpt_file (std::ostream & f, const g_waypoint_list & wpt, const Options & opt);
    bool write_map_file (std::ostream & f, const g_map & map, const Options & opt);
}

//namespace fig {
//    bool write (std::ostream & out, const geo_data & world, const Options & opt);
//    bool parse_file (const char* filename, geo_data & world, const Options & opt);
//}
namespace html {
    bool write (std::ostream & out, const geo_data & world, const Options & opt);
}

/*
namespace bmp{
	bool write_image(const std::string & outfile, const geo_data & world, Options const & opt);
}

namespace fig{
	bool write_file(const char *filename, const geo_data & world, Options const & opt);
	bool read_file (const char* filename, geo_data & world, const Options & opt);
}
*/
namespace io {
	bool in(const std::string & name, geo_data & world, const Options & opt);
	void out(const std::string & outfile, const geo_data & world, const Options & opt);
        bool testext(const std::string & nstr, char *ext);
}

namespace filters {
  // границы номенклатурного листа в wgs84
  Rect<double> nom_range(const std::string & key);

  // добавить границу в соответствии с названием номенклатурного листа
  void map_nom_brd(geo_data & world);
}

#endif

