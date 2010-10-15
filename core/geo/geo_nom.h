#ifndef GEO_NOM_H
#define GEO_NOM_H

#include "2d/rect.h"
#include <string>
#include <vector>

namespace convs{

// Returns region and set scale for a given soviet nomenclature map.
// Coordinates are in Pulkovo-1942 datum
dRect nom_to_range(const std::string & name, int & rscale);

// the same but throw away scale
dRect nom_to_range(const std::string & name);

// Find soviet map nomenclature name for a given point
// rscale is 1000000 for 1:1000000 etc.
// Coordinates must be in Pulkovo-1942 datum
// (p.x is longitude, p.y is latitude)
std::string pt_to_nom(const dPoint & p, int rscale);

// Return nomenclature name of neighbor (defined as shift)
std::string nom_shift(const std::string & name, const iPoint & shift);

// Return nomenclature name list for a region
std::vector<std::string> range_to_nomlist(const dRect & range, int rscale);

}//namespace

#endif
