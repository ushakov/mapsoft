#ifndef GEOFIG_H
#define GEOFIG_H

#include <vector>

#include "fig.h"
#include "geo_data.h"


namespace fig {

  /// get geo reference from fig_world object
  g_map get_ref(const fig_world & w);
  /// remove geo reference from fig_world object
  void rem_ref(fig_world & w);
  /// add geo reference from fig_world object
  void set_ref(fig_world & w, const g_map & m, const Options & o);

  /// get waypoints from w and put them to d
  void get_wpts(const fig_world & w, const g_map & m, geo_data & d);
  /// get tracks from w and put them to d
  void get_trks(const fig_world & w, const g_map & m, geo_data & d);
  /// get refrences for raster images from w and put them to d
  void get_maps(const fig_world & w, const g_map & m, geo_data & d);
  /// add waypoints from d to w
  void put_wpts(fig_world & w, const g_map & m, const geo_data & d);
  /// add tracks from d to w
  void put_trks(fig_world & w, const g_map & m, const geo_data & d);
}
#endif
