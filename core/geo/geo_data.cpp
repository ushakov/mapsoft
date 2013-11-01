#include "geo_data.h"
#include "geo_convs.h"
#include "2d/line_utils.h"


/// get range of all maps in lon-lat coords
/// диапазон карт определяется по точкам привязки, и по границам, если
/// они есть
dRect geo_data::range_map() const {
  dRect ret(0,0,0,0);
  if (maps.size()>0) ret=maps[0].range();
  else return ret;
  for (std::vector<g_map_list>::const_iterator i = maps.begin();
    i!=maps.end();i++) ret = rect_bounding_box(ret, i->range());
  return ret;
}

/// get range of all maps in lon-lat coords
/// то же самое, но сначала делается попытка узнать границы из
/// графического файла, если их нет
dRect geo_data::range_map_correct() const {
  dRect ret(0,0,0,0);
  if (maps.size()>0) ret=maps[0].range_correct();
  else return ret;
  for (std::vector<g_map_list>::const_iterator i = maps.begin();
    i != maps.end(); ++i) ret = rect_bounding_box(ret, i->range_correct());
  return ret;
}

/// get range of all tracks and waypoints in lon-lat coords
dRect geo_data::range_geodata() const{
  dRect ret(0,0,0,0);
  if (wpts.size()>0) ret=wpts[0].range();
  else if (trks.size()>0) ret=trks[0].range();
  else return ret;

  for (std::vector<g_waypoint_list>::const_iterator i = wpts.begin(); 
    i!=wpts.end();i++) ret = rect_bounding_box(ret, i->range());
  for (std::vector<g_track>::const_iterator i = trks.begin(); 
    i!=trks.end();i++) ret = rect_bounding_box(ret, i->range());
  return ret;
}

/// get range of all data in lon-lat coords
dRect geo_data::range() const{
  return rect_bounding_box(range_map(), range_geodata());
}

void
geo_data::add(const geo_data & w){
  wpts.insert(wpts.end(), w.wpts.begin(), w.wpts.end());
  trks.insert(trks.end(), w.trks.begin(), w.trks.end());
  maps.insert(maps.end(), w.maps.begin(), w.maps.end());
}

