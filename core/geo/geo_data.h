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
  void clear();

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

template <typename T>
Options to_options_skipdef (const T & x){
    Options opt=x.to_options();
    T defx;
    Options def=defx.to_options();
    Options::iterator i,j;
    for (i=def.begin(); i!=def.end(); i++){
      j=opt.find(i->first);
      if ((j!=opt.end()) && (j->second==i->second)) opt.erase(j->first);
    }
    return opt;
}

#endif

