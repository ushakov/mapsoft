#ifndef LAYER_TRK_H
#define LAYER_TRK_H

#include "layer.h"
#include "geo/geo_data.h"

/// Растровый слой для показа треков

class LayerTRK
#ifndef SWIG
  : public Layer
#endif  // SWIG
{
private:
  g_track * data; // указатель на геоданные
  Options opt;
  iRect myrange;

public:
  LayerTRK (g_track * _data, const Options & o = Options());

  void set_opt(const Options & o);
  Options get_opt(void) const;

  /// Refresh layer.
  void refresh();

  void set_cnv(Conv * c, int hint=-1);

  /// Get some reasonable reference.
  g_map get_myref() const;

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
