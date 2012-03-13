#ifndef IO_OE_H
#define IO_OE_H

#include <fstream>

#include "geo/geo_data.h"
#include "options/options.h"

namespace oe {
    void read_file (const char* filename, geo_data & world, const Options & opt = Options());
    void write_plt_file (const char *filename, const g_track & trk, const Options & opt = Options());
    void write_wpt_file (const char *filename, const g_waypoint_list & wpt, const Options & opt = Options());
    void write_map_file (const char *filename, const g_map & map, const Options & opt = Options());
}

#endif
