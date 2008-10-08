#ifndef IO_OE_H
#define IO_OE_H

#include <fstream>

#include "../libgeo/geo_data.h"
#include "../utils/mapsoft_options.h"

namespace oe {
    bool read_file (const char* filename, geo_data & world, const Options & opt);
    bool write_plt_file (std::ostream & f, const g_track & trk, const Options & opt);
    bool write_wpt_file (std::ostream & f, const g_waypoint_list & wpt, const Options & opt);
    bool write_map_file (std::ostream & f, const g_map & map, const Options & opt);
}

#endif
