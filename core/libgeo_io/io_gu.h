#ifndef IO_GU_H
#define IO_GU_H

#include "libgeo/geo_data.h"
#include "options/options.h"

namespace gu {
    bool read_file (const char* filename, geo_data & world, const Options & opt);
    bool write_file (const char* filename, const geo_data & world, const Options & opt);
}


#endif
