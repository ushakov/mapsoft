#ifndef LAYER_WPT_H
#define LAYER_WPT_H

#include "layer_geo.h"
#include "geo/geo_convs.h"


/// Растровый слой для показа точек

class LayerWPT
#ifndef SWIG
  : public LayerGeo
#endif  // SWIG
{
private:
  g_waypoint_list * data; // указатель на геоданные
  convs::map2pt cnv;
  g_map mymap;
  iRect myrange;

public:

  LayerWPT (g_waypoint_list * _data);

  /// Refresh layer.
  void refresh();

  /// Get layer reference.
  g_map get_ref() const;

  /// Get layer conversion to wgs84 latlon.
  convs::map2pt get_cnv() const;

  /// Set some reasonable reference.
  g_map get_myref() const;

  /// Set layer reference.
  void set_ref(const g_map & map);

  /// Optimized get_image to return empty image outside of bounds.
  iImage get_image (iRect src);

  /// Draw on image.
  void draw(const iPoint origin, iImage & image);

  int find_waypoint (iPoint pt, int radius = 3);

  std::vector<int> find_waypoints (const iRect & r);

  /// Get pointer to the data object.
  g_waypoint_list * get_data() const;

  /// Get pointer to the n-th point.
  g_waypoint * get_pt(const int n) const;

  /// Get layer range.
  iRect range () const;
};


#endif
