#ifndef IO_KML_H
#define IO_KML_H

#include "libgeo/geo_data.h"
#include "utils/options.h"

namespace kml {
//    bool read_file (const char* filename, geo_data & world, const Options & opt);
    bool write_file (const char* filename, const geo_data & world, const Options & opt);
}

#endif
