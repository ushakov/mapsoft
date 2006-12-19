#ifndef MAPSOFT_CONVS_H
#define MAPSOFT_CONVS_H

// Геодезические преобразования

// все сделано в виде объектов, чтобы в начале спокойно обработать все параметры 
// преобразования (прочитать текстовые параметры и т.п.), а потом быстро преобразовывать...

#include <vector>
#include "../geo_io/geo_data.h"
#include "mapsoft_options.h"
#include "mapsoft_geo.h"
#include "image.h"

namespace convs{

// преобразование геодезических координат
// точки преобразуются по ссылке, чтобы можно было не копируя
// преобразовывать координаты в сложных штуках типа g_waypoint

// удобно разбить преобразование на части:
struct pt2ll{ // преобразование к широте-долготе и обратно
  pt2ll(const Datum & D, const Proj & P, const Options & Po);
  void frw(g_point & p) const;
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
  ll2wgs(const Datum & D);
  void frw(g_point & p) const;
  void bck(g_point & p) const;

  private:
    Datum datum;
};


// а вот общее преобразование:
struct pt2pt{
  pt2pt(const Datum & sD, const Proj & sP, const Options & sPo, 
        const Datum & dD, const Proj & dP, const Options & dPo);
  void frw(g_point & p);
  void bck(g_point & p);

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
         const Datum & dD, const Proj & dP, const Options & dPo);
  void frw(g_point & p);
  void bck(g_point & p);
  std::vector<g_point> line_frw(const std::vector<g_point> & l);
  std::vector<g_point> line_bck(const std::vector<g_point> & l);
  private:
    pt2ll pc1, pc2;
    ll2wgs dc;
    double k_map2geo[6];
    double k_geo2map[6];
  public:
    std::vector<g_point> border;
    std::vector<g_point> border_geo;
};

// Быстрая проверка границ
struct border_tester{
  private:
  struct side{
   int x1,x2,y1,y2;
   double k;
  };
  std::vector<side> sides;
  std::vector<g_point> border;
  public:
  border_tester(std::vector<g_point> & brd);
  // попадает ли точка в пределы границы
  bool test(const int x, const int y) const;
  // расстояние до ближайшей границы справа
  int nearest_border (const int x, const int y) const;
  // "задевает" ли карта данный район
  bool test_range(Rect<int> range) const;
};


// преобразование из карты в карту
// здесь может быть суровое разбиение карты на куски и аппроксимация линейными преобразованиями...
// здесь же - преобразование линий
// здесь же - преобразование картинок (с интерфейсом как у image loader'a)

struct map2map{
  map2map(const g_map & sM, const g_map & dM, const Options & O);
  void frw(g_point & p);
  void bck(g_point & p);

  std::vector<g_point> line_frw(const std::vector<g_point> & l);
  std::vector<g_point> line_bck(const std::vector<g_point> & l);
  // src_scale -- во сколько раз была уменьшена растровая картинка при загрузке
  // cnv_rect - прямоугольник в плоскости _преобразованной картинки_!!!
  int image_frw(Image<int> & src_img, int src_scale, Rect<int> cnv_rect,
                Image<int> & dst_img, Rect<int> dst_rect);
  int image_bck(Image<int> & src_img, int src_scale, Rect<int> cnv_rect, 
                Image<int> & dst_img, Rect<int> dst_rect);
    map2pt c1,c2;
    border_tester tst_frw, tst_bck;
    std::vector<g_point> border_src; // граница sM
    std::vector<g_point> border_dst; // это след от границы sM на dM! 
};



}//namespace
#endif
