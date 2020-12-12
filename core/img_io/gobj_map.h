#ifndef GOBJ_MAP_H
#define GOBJ_MAP_H

#include <vector>
#include <map>
#include "gobj_geo.h"
#include "cache/cache.h"
#include "2d/point.h"
#include "2d/rect.h"
#include "2d/image.h"
#include "loaders/image_cache.h"
#include <boost/shared_ptr.hpp>

/// Растровый слой для показа привязанных карт.

#define SHOW_MAP 1
#define SHOW_BRD 2
#define SHOW_REF 4

class GObjMAP
#ifndef SWIG
  : public GObjGeo
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

public:

  GObjMAP (g_map_list *_data, const Options & opt = Options());

  g_map get_myref() const;
  void refresh(); /// Refresh layer.

  /// Get pointer to the data.
  g_map_list * get_data() const {return data;}

  /// Get pointer to the n-th map.
  g_map * get_map(const int n) const {return &(*data)[n];}

  /// Find map
  ssize_t find_map(const iPoint & pt) const;
  ssize_t find_map(const iRect & r) const;

  /// Show/hide reference points and border for a n-th map
  /// (n=-1 for all maps)
  void status_set(int mask, bool val=true, const g_map * m = NULL);
  void show_ref(const g_map * m = NULL) {status_set(SHOW_REF, true,  m);}
  void hide_ref(const g_map * m = NULL) {status_set(SHOW_REF, false, m);}
  void show_brd(const g_map * m = NULL) {status_set(SHOW_BRD, true,  m);}
  void hide_brd(const g_map * m = NULL) {status_set(SHOW_BRD, false, m);}
  void show_map(const g_map * m = NULL) {status_set(SHOW_MAP, true,  m);}
  void hide_map(const g_map * m = NULL) {status_set(SHOW_MAP, false, m);}

  /// Draw on image.
  int draw(iImage & image, const iPoint & origin);

  /// Get layer range.
  iRect range() const {return myrange;}

  /// Write to a file picture with map borders in fig format.
  void dump_maps(const char *file);

  /// Scale of map at a given point (first map or map m).
  /// > 1 if map pixel is smaller than layer one;
  /// < 0 if no maps at this point.
  double scale(const dPoint & p);
  double scale(const dPoint & p, const int m);
};


#endif
