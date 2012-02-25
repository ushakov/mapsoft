#ifndef LAYER_GEOMAP_H
#define LAYER_GEOMAP_H

#include <vector>
#include <map>
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
  g_map_list *data;                  // привязки карт
  std::vector<convs::map2map> m2ms;  // преобразования из каждой карты в mymap
  std::vector<double> scales;        // во сколько раз мы сжимаем карты при загрузке
  std::vector<int>    iscales;       // во сколько раз мы сжимаем карты при загрузке
  iRect myrange;                     // габариты карты
  Cache<int, iImage> image_cache;    // кэш изображений
  g_map mymap;
  bool drawborder;
  std::map<const g_map*, int> status; // visibility of refpoints, border, map image

public:

  LayerGeoMap (g_map_list *_data, bool _drawborder=true);

  /// Get layer reference.
  g_map get_ref() const;

  /// Get some reasonable reference.
  g_map get_myref() const;

  /// Get layer conversion to wgs84 latlon.
  convs::map2pt get_cnv() const;

  /// Set layer reference.
  void set_ref(const g_map & map);

  /// Get pointer to the data.
  g_map_list * get_data() const;

  /// Get pointer to the n-th map.
  g_map * get_map(const int n) const;

  /// Find map
  int find_map(const iPoint & pt) const;
  int find_map(const iRect & r) const;

  /// Show/hide reference points and border for a n-th map
  /// (n=-1 for all maps)
  void status_set(int mask, bool val=true, const g_map * m = NULL);
  void show_ref(const g_map * m = NULL);
  void hide_ref(const g_map * m = NULL);
  void show_brd(const g_map * m = NULL);
  void hide_brd(const g_map * m = NULL);
  void show_map(const g_map * m = NULL);
  void hide_map(const g_map * m = NULL);

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
