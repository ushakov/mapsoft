#ifndef O_IMG_H
#define O_IMG_H

#include <vector>
#include <fstream>

#include "../libgeo/geo_data.h"
#include "../utils/mapsoft_options.h"

namespace img {

    bool write_file (const char* filename, const geo_data & world, const Options & opt);

}
#endif
