#ifndef O_IMG_H
#define O_IMG_H

#include "libgeo/geo_data.h"
#include "utils/options.h"

namespace img {

    bool write_file (const char* filename, const geo_data & world, Options opt);

}
#endif
