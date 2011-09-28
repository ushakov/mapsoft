#ifndef IO_GPX_H
#define IO_GPX_H

#include "geo/geo_data.h"
#include "options/options.h"

namespace gpx {
    bool read_file (const char* filename, geo_data & world, const Options & opt);
    bool write_file (const char* filename, const geo_data & world, const Options & opt);
}

#endif
