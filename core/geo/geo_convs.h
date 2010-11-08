#ifndef GEO_CONVS_H
#define GEO_CONVS_H

// Геодезические преобразования

#include <vector>
#include "geo_data.h"
#include "options/options.h"
#include "2d/image.h"
#include "2d/point_conv.h"
#include <proj_api.h>

namespace convs{

/// Преобразование геодезических координат.
/// Точки преобразуются по ссылке, чтобы можно было не копируя
/// преобразовывать координаты в сложных штуках типа g_waypoint
struct pt2pt : Conv{
  pt2pt(const Datum & sD, const Proj & sP, const Options & sPo,
        const Datum & dD, const Proj & dP, const Options & dPo);

  pt2pt(const pt2pt & other);
  pt2pt & operator=(const pt2pt & other);
  ~pt2pt();

  void frw(dPoint & p) const;
  void bck(dPoint & p) const;

private:
  projPJ pr_src, pr_dst;
  void copy(const pt2pt & other);
  void destroy(void);
  int * refcounter;
};

/// Преобразование из точки карты в геодезическую точку.
struct map2pt : Conv{
  map2pt(const g_map & sM,
         const Datum & dD, const Proj & dP, const Options & dPo = Options());

  map2pt(const map2pt & other);
  map2pt & operator=(const map2pt & other);
  ~map2pt();

  void frw(dPoint & p) const;
  void bck(dPoint & p) const;

private:
  projPJ pr_ref, pr_map, pr_dst;
  AffConv lin_cnv;
  void copy(const map2pt & other);
  void destroy(void);
  int * refcounter;
};

/// Быстрая проверка границ.
struct border_tester{
private:
  struct side{
   int x1,x2,y1,y2;
   double k;
  };
  std::vector<side> sides;
  dLine border;
public:
  border_tester(const dLine & brd);
  /// попадает ли точка в пределы границы
  bool test(const int x, const int y) const;
  /// расстояние до ближайшей границы справа
  int nearest_border (const int x, const int y) const;
  /// "задевает" ли карта данный район
  bool test_range(iRect range) const;
};

/// Преобразование из карты в карту.
/// Здесь может быть суровое разбиение карты на куски и аппроксимация линейными преобразованиями...
/// Здесь же - преобразование картинок (с интерфейсом как у image loader'a).
struct map2map : Conv{
  map2map(const g_map & sM, const g_map & dM, bool test_brd_ = true);
  void frw(dPoint & p) const;
  void bck(dPoint & p) const;

  // src_scale -- во сколько раз была уменьшена растровая картинка при загрузке
  // cnv_rect - прямоугольник в плоскости _преобразованной картинки_!!!
  int image_frw(iImage & src_img, int src_scale, iRect cnv_rect,
                iImage & dst_img, iRect dst_rect) const;
  int image_bck(iImage & src_img, int src_scale, iRect cnv_rect, 
                iImage & dst_img, iRect dst_rect) const;

  map2pt c1,c2;

  dLine border_src; // граница sM
  dLine border_dst; // это след от границы sM на dM! 
  bool test_brd;
  border_tester tst_frw, tst_bck;
};


/// Autodetect map projection options (lon0) if needed.
Options map_popts(const g_map & M, Options O = Options());

/// Make some reasonable reference for geodata.
/// тип проекции -- по первой карте, или lonlat, если карт нет
/// осевой меридиан -- 6n+3, наиболее близкий к середине диапазона треков и точек,
/// а если их нет - к середине диапазона карт
/// масштаб -- соответствующий минимальному масштабу карт, если они есть,
/// или 1/3600 градуса на точку, если карт нет
g_map mymap(const geo_data & world); 

/// Get map scale in projection P units per point
double map_mpp(const g_map &map, Proj P);

/// Find central meridian for a given longitude.
double lon2lon0(const double lon);

/// Find prefix number  for a given longitude. (39.5 -> 7)
int lon2pref(const double lon);

/// Find central meridian for a given tmerc x coordinate with zone prefix.
double lon_pref2lon0(const double lon);

/// Remove zone prefix from tmerc x coordinate.
double lon_delprefix(const double lon);

}//namespace
#endif
