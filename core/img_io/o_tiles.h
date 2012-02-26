#ifndef O_TILES_H
#define O_TILES_H

#include "geo/geo_data.h"
#include "options/options.h"

namespace tiles {

    bool write_file (const char* filename, const geo_data & world, const Options & opt);

}
#endif
