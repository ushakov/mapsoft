#ifndef LAYER_TRK_H
#define LAYER_TRK_H

#include "layer_geo.h"
#include "geo/geo_convs.h"

/// Растровый слой для показа точек и треков

class LayerTRK
#ifndef SWIG
  : public LayerGeo
#endif  // SWIG
{
private:
  geo_data * world; // указатель на геоданные
  convs::map2pt cnv;
  g_map mymap;
  iRect myrange;

public:
  LayerTRK (geo_data * _world);

  /// Refresh layer.
  void refresh();

  /// Get layer reference.
  g_map get_ref() const;

  /// Set layer reference.
  void set_ref(const g_map & map);

  /// Set some reasonable reference.
  void set_ref();

  /// Optimized get_image to return empty image outside of bounds.
  iImage get_image (iRect src);

  /// Draw on image.
  void draw(const iPoint origin, iImage & image);

  std::pair<int, int> find_trackpoint (iPoint pt, int radius = 3);

  // поиск трека. Находится сегмент, в которые тыкают, возвращается
  // первая точка сегмента (0..size-2).
  // если тыкают в первую точку - возвращается -2, если в последнюю -- -3.
  std::pair<int, int> find_track (iPoint pt, int radius = 3);

  /// Get pointer to the world object.
  geo_data * get_world() const;

  /// Get layer range.
  iRect range() const;
};

#endif
