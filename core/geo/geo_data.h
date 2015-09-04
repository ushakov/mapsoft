#ifndef GEO_DATA_H
#define GEO_DATA_H

#include <vector>
#include <string>

#include "g_wpt.h"
#include "g_trk.h"
#include "g_map.h"

#include "2d/line.h"
#include "2d/rect.h"
#include "options/options.h"

/// geo data
struct geo_data {
  std::vector<g_waypoint_list> wpts;
  std::vector<g_track> trks;
  std::vector<g_map_list> maps;

  /// clear all data
  void clear() {
    wpts.clear(); trks.clear(); maps.clear();}

  bool empty() const {
    return wpts.empty() && trks.empty() && maps.empty(); }

  void push_back(const g_waypoint_list & d){wpts.push_back(d);}
  void push_back(const g_track & d)        {trks.push_back(d);}
  void push_back(const g_map_list & d)     {maps.push_back(d);}

  /// get range of all maps in lon-lat coords, fast
  dRect range_map() const;
  /// get range of all maps in lon-lat coords using file size
  dRect range_map_correct() const;
  /// get range of all tracks and waypoints in lon-lat coords
  dRect range_geodata() const;
  /// get range of all data in lon-lat coords
  dRect range() const;

  // add data from another geo_data object
  void add(const geo_data & w);
};

#endif

