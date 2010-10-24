#ifndef OCAD_GEO_H
#define OCAD_GEO_H

#include "geo/geo_data.h"
#include "ocad_file.h"

namespace ocad{

/// get reference from ocad file
g_map get_ref(const ocad_file & O);

/// set reference from rscale and wgs coords of zero point
void set_ref(ocad_file & O, double rscale, const dPoint & p0);

} // namespace
#endif
