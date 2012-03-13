#ifndef IO_GU_H
#define IO_GU_H

#include "geo/geo_data.h"
#include "options/options.h"

namespace gu {
    void read_file (const char* filename, geo_data & world, const Options & opt);
    void write_file (const char* filename, const geo_data & world, const Options & opt);
}


#endif
