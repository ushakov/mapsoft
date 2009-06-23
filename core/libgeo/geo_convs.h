#ifndef GEO_CONVS_H
#define GEO_CONVS_H

// Геодезические преобразования

#include <vector>
#include "geo_data.h"
#include "../utils/options.h"
#include "../lib2d/image.h"

// все сделано в виде объектов, чтобы в начале спокойно обработать все параметры 
// преобразования (прочитать текстовые параметры и т.п.), а потом быстро преобразовывать...

namespace convs{

// преобразование геодезических координат
// точки преобразуются по ссылке, чтобы можно было не копируя
// преобразовывать координаты в сложных штуках типа g_waypoint

// удобно разбить преобразование на части:
struct pt2ll{ // преобразование к широте-долготе и обратно
  pt2ll(const Datum & D = Datum("wgs84"), const Proj & P = Proj("lonlat"), const Options & Po = Options());
  void frw(g_point & p); // может поменять lon0!
  void bck(g_point & p); // может поменять lon0!

  private:
    double lat0,lon0,E0,N0,k;
    int zone;
    char zc;
    double a,f;
  public:
    Datum datum;
    Proj  proj;
};

struct ll2wgs{ // преобразование широты-долготы в wgs84 и обратно
  ll2wgs(const Datum & D = Datum("wgs84"));
  void frw(g_point & p) const;
  void bck(g_point & p) const;

  private:
    Datum datum;
};


// а вот общее преобразование:
struct pt2pt{

  pt2pt(const Datum & sD, const Proj & sP, const Options & sPo, 
        const Datum & dD, const Proj & dP, const Options & dPo);

//  pt2pt(const char * sD, const char * sP, const Options & sPo, 
//        const char * dD, const char * dP, const Options & dPo);

  pt2pt();

  void frw(g_point & p);
  void bck(g_point & p);
  void frw_safe(g_point & p);
  void bck_safe(g_point & p);
  // преобразования линий
  // точность acc - в координатах исходной проекции
  g_line line_frw(const g_line & l, double acc, int max=100);
  g_line line_bck(const g_line & l, double acc, int max=100);
  // преобразование прямоугольника (в произвольную фигуру) и нахождение 
  // минимального прямоугольника, в котором она лежит
  dRect bb_frw(const Rect<double> & R, double acc);
  dRect bb_bck(const Rect<double> & R, double acc);

  private:
    pt2ll pc1, pc2;
    ll2wgs dc1, dc2;
    bool triv1, triv2;
};

// преобразование из точки карты в геодезическую точку
// здесь же - выяснение всяких параметров карты (размер изображения, масштам метров/точку)
// сюда же - преобразование линий!
struct map2pt{
  map2pt(const g_map & sM,
         const Datum & dD, const Proj & dP, const Options & dPo = Options());
//  map2pt(const g_map & sM,
//         const char * dD, const char * dP, const Options & dPo = Options());

  void frw(g_point & p);
  void bck(g_point & p);
  void frw_safe(g_point & p);
  void bck_safe(g_point & p);
  g_line line_frw(const g_line & l, int max=100);
  g_line line_bck(const g_line & l, int max=100);
  // преобразование прямоугольника (в произвольную фигуру) и нахождение 
  // минимального прямоугольника, в котором она лежит
  dRect bb_frw(const iRect & R);
  iRect bb_bck(const dRect & R);
  private:
    pt2ll pc1, pc2;
    ll2wgs dc;
    double k_map2geo[6];
    double k_geo2map[6];
  public:
    g_line border;
    g_line border_geo;
};

// Быстрая проверка границ
struct border_tester{
  private:
  struct side{
   int x1,x2,y1,y2;
   double k;
  };
  std::vector<side> sides;
  g_line border;
  public:
  border_tester(g_line & brd);
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

struct map2map{
  map2map(const g_map & sM, const g_map & dM, bool test_brd_ = true);
  void frw(g_point & p);
  void bck(g_point & p);

  g_line line_frw(const g_line & l, int max=100);
  g_line line_bck(const g_line & l, int max=100);
  // src_scale -- во сколько раз была уменьшена растровая картинка при загрузке
  // cnv_rect - прямоугольник в плоскости _преобразованной картинки_!!!
  int image_frw(iImage & src_img, int src_scale, iRect cnv_rect,
                iImage & dst_img, iRect dst_rect);
  int image_bck(iImage & src_img, int src_scale, iRect cnv_rect, 
                iImage & dst_img, iRect dst_rect);

  // новая версия
  //void image_frw(iImage & src_img, int src_scale, iPoint origin, Image<int> & image);
  //void image_bck(iImage & src_img, int src_scale, iPoint origin, Image<int> & image);

  // преобразование прямоугольника (в произвольную фигуру) и нахождение 
  // минимального прямоугольника, в котором она лежит
  iRect bb_frw(const Rect<int> & R);
  iRect bb_bck(const Rect<int> & R);

    bool test_brd;
    map2pt c1,c2;
    border_tester tst_frw, tst_bck;
    g_line border_src; // граница sM
    g_line border_dst; // это след от границы sM на dM! 
};

g_map mymap(const geo_data & world); // естественная привязка геоданных
// тип проекции -- по первой карте, или lonlat, если карт нет
// осевой меридиан -- 6n+3, наиболее близкий к середине диапазона треков и точек,
// а если их нет - к середине диапазона карт
// масштаб -- соответствующий минимальному масштабу карт, если они есть,
// или 1/3600 градуса на точку, если карт нет

// масштаб карты, единиц проекции P в точке
double map_mpp(const g_map &map, Proj P);

}//namespace
#endif
