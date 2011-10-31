#ifndef GEO_REFS_H
#define GEO_REFS_H

// Привязки специальных карт и снимков

#include "geo_data.h"

/** Create some reasonable tmerc map reference from points.

Points must be given in wgs84 lonlat. Map border is set after points.
Map reference points are set to 4 corner points. Map scale
is set according to u_per_m value. If yswap = true y-coordinate is
swapped (some applications wants y-axis to be down-directed). Map
central meridian (lon0) is set from the points center. */
g_map mk_tmerc_ref(const dLine & points, double u_per_m, bool yswap);

g_map ref_google(int scale);
g_map ref_ks_old(int scale);
g_map ref_ks(int scale);

#endif
