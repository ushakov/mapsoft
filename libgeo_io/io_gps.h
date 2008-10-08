#ifndef IO_GPS_H
#define IO_GPS_H

#include "../libgeo/geo_data.h"
#include "../utils/mapsoft_options.h"

namespace gps {

    bool init_gps(const char* port);
    bool get_tracks (const char* port, geo_data & world, const Options & opt);
    bool get_waypoints (const char* port, geo_data & world, const Options & opt);
    bool get_all (const char* port, geo_data & world, const Options & opt);
    bool put_track (const char* port, const g_track & tr, const Options & opt);
    bool put_waypoints (const char* port, const g_waypoint_list & wp, const Options & opt);
    bool put_all (const char* port, const geo_data & world, const Options & opt);

// options:
//   --gps_off     -- turn off gps device after use
//
// TODO:
//   --data=[W|T|A]*
//   --data_in=[W|T|A]*
//   --data_in=[W|T|A]*
}

#endif
