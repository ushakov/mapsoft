#ifndef IO_ZIP_H
#define IO_GU_H

#include <zip.h>
#include "geo/geo_data.h" 
#include "options/options.h"

namespace io_zip {
    bool read_file (const char* filename, geo_data & world, const Options & opt);
}

#endif
