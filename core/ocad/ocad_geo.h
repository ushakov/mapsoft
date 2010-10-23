#ifndef OCAD_GEO_H
#define OCAD_GEO_H

#include "geo/geo_data.h"
#include "ocad_file.h"

namespace ocad{

/// get reference from ocad file
g_map get_ref(const ocad_file & O);

/// set reference to ocad
//void set_ref(const ocad_file & O, const g_ref & ref);


} // namespace
#endif
