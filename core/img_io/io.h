#ifndef IMG_IO_H
#define IMG_IO_H

#include <string>
#include "geo/geo_data.h"
#include "options/options.h"
#include "vmap/vmap.h"

namespace io {

// write data to image file
void out_img(const std::string & out_name, geo_data & world, vmap::world & vm, const Options & opt);

} // namespace
#endif

