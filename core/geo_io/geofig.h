#ifndef GEOFIG_H
#define GEOFIG_H

#include <vector>

#include "fig/fig.h"
#include "geo/geo_data.h"


namespace fig {

  /// get geo reference from fig_world object
  g_map get_ref(const fig_world & w);
  /// remove geo reference from fig_world object
  void rem_ref(fig_world & w);
  /// add geo reference to fig_world object
  void set_ref(fig_world & w, const g_map & m, const Options & o);

  /// get waypoints, tracks, map refrences from fig
  void get_wpts(const fig_world & w, const g_map & m, geo_data & d);
  void get_trks(const fig_world & w, const g_map & m, geo_data & d);
  void get_maps(const fig_world & w, const g_map & m, geo_data & d);

  /// remove waypoins, tracks, maps, or map borders:
  void rem_wpts(fig_world & w);
  void rem_trks(fig_world & w);
  void rem_maps(fig_world & w);
  void rem_brds(fig_world & w);

  /// add waypoints or tracks from to fig
  void put_wpts(fig_world & w, const g_map & m, const geo_data & d);
  void put_trks(fig_world & w, const g_map & m, const geo_data & d);
}
#endif
