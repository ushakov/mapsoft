#ifndef LAYER_WPT_H
#define LAYER_WPT_H

#include "layer_geo.h"
#include "geo/geo_convs.h"


/// Растровый слой для показа точек и треков

class LayerWPT
#ifndef SWIG
  : public LayerGeo
#endif  // SWIG
{
private:
  geo_data * world; // указатель на геоданные
  convs::map2pt cnv;
  g_map mymap;
  iRect myrange;
  Color wpt_color_override;
  Color wpt_bgcolor_override;
  int dot_width;

public:

  LayerWPT (geo_data * _world);

  /// Refresh layer.
  void refresh();

  /// Get layer reference.
  g_map get_ref() const;

  /// Set layer reference.
  void set_ref(const g_map & map);

  /// Set some reasonable reference.
  void set_ref();

  /// Get layer configuration.
  Options get_config() const;

  /// Set layer configuration from Options.
  void set_config(const Options& opt);

  /// Optimized get_image to return empty image outside of bounds.
  iImage get_image (iRect src);

  /// Draw on image.
  void draw(const iPoint origin, iImage & image);

  std::pair<int, int> find_waypoint (iPoint pt, int radius = 3);

  /// Get pointer to the world object.
  geo_data * get_world() const;

  /// Get layer range.
  iRect range () const;
};


#endif
