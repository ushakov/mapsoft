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

// create PROJ4 projection object from our D, P and options
projPJ mkproj(const Datum & D, const Proj & P, const Options & o);

// преобразование геодезических координат
// точки преобразуются по ссылке, чтобы можно было не копируя
// преобразовывать координаты в сложных штуках типа g_waypoint

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



// преобразование из точки карты в геодезическую точку
// здесь же - выяснение всяких параметров карты (размер изображения, масштам метров/точку)
// сюда же - преобразование линий!
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
    double k_map2geo[6];
    double k_geo2map[6];
    void copy(const map2pt & other);
    void destroy(void);
    int * refcounter;

  public:
    dLine border;
    dLine border_geo;
};

// autodetect map projection options (lon0) if needed
Options map_popts(const g_map & M, Options O = Options());

// Быстрая проверка границ
struct border_tester{
  private:
  struct side{
   int x1,x2,y1,y2;
   double k;
  };
  std::vector<side> sides;
  dLine border;
  public:
  border_tester(dLine & brd);
  // попадает ли точка в пределы границы
  bool test(const int x, const int y) const;
  // расстояние до ближайшей границы справа
  int nearest_border (const int x, const int y) const;
  // "задевает" ли карта данный район
  bool test_range(iRect range) const;
};


// преобразование из карты в карту
// здесь может быть суровое разбиение карты на куски и аппроксимация линейными преобразованиями...
// здесь же - преобразование линий
// здесь же - преобразование картинок (с интерфейсом как у image loader'a)

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

  iRect bb_frw_i(const iRect & R) const;
  iRect bb_bck_i(const iRect & R) const;

    bool test_brd;
    map2pt c1,c2;
    border_tester tst_frw, tst_bck;
    dLine border_src; // граница sM
    dLine border_dst; // это след от границы sM на dM! 
};

g_map mymap(const geo_data & world); // естественная привязка геоданных
// тип проекции -- по первой карте, или lonlat, если карт нет
// осевой меридиан -- 6n+3, наиболее близкий к середине диапазона треков и точек,
// а если их нет - к середине диапазона карт
// масштаб -- соответствующий минимальному масштабу карт, если они есть,
// или 1/3600 градуса на точку, если карт нет

// масштаб карты, единиц проекции P в точке
double map_mpp(const g_map &map, Proj P);

// find central meridian for a given longitude/point/rect
double lon2lon0(const double lon);
double lon_pref2lon0(const double lon);
double lon_delprefix(const double lon);

}//namespace
#endif