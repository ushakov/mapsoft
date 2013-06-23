#ifndef LAYER_MAP_H
#define LAYER_MAP_H

#include <vector>
#include <map>
#include "gred/gobj.h"
#include "geo/geo_convs.h"
#include "2d/cache.h"
#include "2d/point.h"
#include "2d/rect.h"
#include "2d/image.h"
#include "loaders/image_cache.h"
//#include "layers/img.h"
#include <boost/shared_ptr.hpp>

/// Растровый слой для показа привязанных карт.

class LayerMAP
#ifndef SWIG
  : public GObj
#endif  // SWIG
{
private:
  g_map_list *data;                  // привязки карт
  std::vector< boost::shared_ptr<Conv> > m2ms;
                                     // преобразования из каждой карты
  std::vector<double> scales;        // во сколько раз мы сжимаем карты при загрузке
  std::vector<int>    iscales;       // во сколько раз мы сжимаем карты при загрузке
  std::vector<iLine>  borders;       // границы (в координатах слоя)
  iRect myrange;                     // габариты карты
  std::map<const g_map*, int> status; // visibility of refpoints, border, map image

  std::map<std::string, Cache<iPoint, iImage> > tmap_cache;
  ImageCache image_cache;
//  Cache<std::string, LayerImg> img_cache;

public:

  LayerMAP (g_map_list *_data, const Options & opt = Options());

  /// Get some reasonable reference.
  g_map get_myref() const;

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

  /// Draw on image.
  int draw(iImage & image, const iPoint & origin);

  /// Get layer range.
  iRect range() const;

  /// Write to a file picture with map borders in fig format.
  void dump_maps(const char *file);

  /// Scale of map at a given point (first map or map m).
  /// > 1 if map pixel is smaller than layer one;
  /// < 0 if no maps at this point.
  double scale(const dPoint & p);
  double scale(const dPoint & p, const int m);

private:
  /// calculate conversions and range
  void make_m2ms();
};


#endif
