#ifndef IO_ZIP_H
#define IO_ZIP_H

#include <zip.h>
#include <vector>
#include <string>
#include "geo/geo_data.h" 
#include "options/options.h"

namespace io_zip {
    // read zipped files (any format)
    bool read_file (const char* filename, geo_data & world, const Options & opt);

    // write zipped files (ozi format)
    bool write_file (const char* filename, const std::vector<std::string> & files);
}

#endif
