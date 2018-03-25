#ifndef IO_ZIP_H
#define IO_ZIP_H

#include <vector>
#include <string>
#include "geo/geo_data.h" 
#include "options/options.h"

namespace io_zip {
    // read zipped files (any format)
    void read_file (const char* filename, geo_data & world, const Options & opt = Options());

    // write zipped files (ozi format)
    void write_file (const char* filename, const std::vector<std::string> & files, const Options & opt = Options());
}

#endif
