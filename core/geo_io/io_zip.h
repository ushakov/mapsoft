#ifndef IO_ZIP_H
#define IO_ZIP_H

#include <zip.h>
#include <vector>
#include <string>
#include "geo/geo_data.h" 
#include "options/options.h"

namespace io_zip {
    bool read_file (const char* filename, geo_data & world, const Options & opt);
    bool write_file (const char* filename, const std::vector<std::string> & files);
}

#endif
