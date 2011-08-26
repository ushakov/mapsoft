#ifndef LAYER_GEOMAP_H
#define LAYER_GEOMAP_H

#include <vector>
#include "layer_geo.h"
#include "geo/geo_convs.h"
#include "2d/cache.h"
#include "2d/point.h"
#include "2d/rect.h"
#include "2d/image.h"


/// Растровый слой для показа привязанных карт.

class LayerGeoMap
#ifndef SWIG
  : public LayerGeo
#endif  // SWIG
{
private:
  const geo_data *world;             // привязки карт
  std::vector<convs::map2map> m2ms;  // преобразования из каждой карты в mymap
  std::vector<double> scales;        // во сколько раз мы сжимаем карты при загрузке
  std::vector<int>    iscales;       // во сколько раз мы сжимаем карты при загрузке
  iRect myrange;                     // габариты карты
  Cache<int, iImage> image_cache;    // кэш изображений
  g_map mymap;
  bool drawborder;

public:

  LayerGeoMap (const geo_data *_world, bool _drawborder=true);

  /// Get layer reference.
  g_map get_ref() const;

  /// Get some reasonable reference.
  g_map get_myref() const;

  /// Set layer reference.
  void set_ref(const g_map & map);

  /// Get pointer to the world object.
  const geo_data * get_world() const;

  /// Refresh layer.
  void refresh();

  // Optimized get_image to return empty image outside of bounds.
  iImage get_image (iRect src);

  /// Draw on image.
  void draw(const iPoint origin, iImage & image);

  /// Get layer range.
  iRect range() const;

  /// Write to a file picture with map borders in fig format.
  void dump_maps(const char *file);

private:
  /// calculate conversions and range
  void make_m2ms();
};


#endif
