#ifndef LAYER_TRK_H
#define LAYER_TRK_H

#include "layer_geo.h"
#include "geo/geo_convs.h"

/// Растровый слой для показа треков

class LayerTRK
#ifndef SWIG
  : public LayerGeo
#endif  // SWIG
{
private:
  g_track * data; // указатель на геоданные
  convs::map2pt cnv;
  g_map mymap;
  iRect myrange;
  bool draw_dots, draw_arrows;

public:
  LayerTRK (g_track * _data, const Options & opt = Options());

  /// Refresh layer.
  void refresh();

  /// Get layer reference.
  g_map get_ref() const;

  /// Get layer conversion to wgs84 latlon.
  convs::map2pt get_cnv() const;

  /// Get some reasonable reference.
  g_map get_myref() const;

  /// Set layer reference.
  void set_ref(const g_map & map);

  /// Optimized get_image to return empty image outside of bounds.
  iImage get_image (iRect src);

  /// Draw on image.
  void draw(const iPoint origin, iImage & image);

  int find_trackpoint (iPoint pt, int radius = 3);

  std::vector<int> find_trackpoints (const iRect & r);

  // Поиск трека. Находится сегмент, в который тыкают, возвращается
  // первая точка сегмента (0..size-2).
  // Таким способом не ищется трек, состоящий из одной точки.
  int find_track (iPoint pt, int radius = 3);

  /// Get pointer to the data object.
  g_track * get_data() const;

  /// Get pointer to the n-th point.
  g_trackpoint * get_pt(const int n) const;

  /// Get layer range.
  iRect range() const;
};

#endif
