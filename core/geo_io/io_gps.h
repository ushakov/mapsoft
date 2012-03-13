#ifndef IO_GPS_H
#define IO_GPS_H

#include "geo/geo_data.h"
#include "options/options.h"

namespace gps {

    void init_gps(const char* port, const Options &opt = Options());
    void get_tracks (const char* port, geo_data & world, const Options & opt = Options());
    void get_waypoints (const char* port, geo_data & world, const Options & opt = Options());
    void get_all (const char* port, geo_data & world, const Options & opt = Options());
    void put_track (const char* port, const g_track & tr, const Options & opt = Options());
    void put_waypoints (const char* port, const g_waypoint_list & wp, const Options & opt = Options());
    void turn_off (const char* port, const Options &opt = Options());
    void put_all (const char* port, const geo_data & world, const Options & opt = Options());

// options:
//   --gps_off     -- turn off gps device after use
//
// TODO:
//   --data=[W|T|A]*
//   --data_in=[W|T|A]*
//   --data_in=[W|T|A]*
}

#endif
