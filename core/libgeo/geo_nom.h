#ifndef GEO_NOM_H
#define GEO_NOM_H

#include "../lib2d/rect.h"

namespace convs{

// Find region for a given soviet map nomenclature name.
// Coordinates are in Pulkovo-1942 datum
// (p.x is longitude, p.y is latitude)
dRect nom_range(const std::string & name);

// Find soviet map nomenclature name for a given point
// rscale is 1000000 for 1:1000000 etc.
// Coordinates must be in Pulkovo-1942 datum
// (p.x is longitude, p.y is latitude)
std::string nom_name(const dPoint & p, int rscale);

}//namespace

#endif
