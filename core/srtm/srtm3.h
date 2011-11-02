#ifndef SRTM3_H
#define SRTM3_H

#include <string>

#include <2d/image.h>
#include <2d/cache.h>

// Получение высоты любой точки из директории с srtm-данными
// Кэширование кусочков

// srtm-данные надо скачивать с ftp://e0mss21u.ecs.nasa.gov/srtm/

// Специальные значения для высоты
const short srtm_min      = -32000; // для проверки
const short srtm_nofile   = -32767; // нет такого файла
const short srtm_undef    = -32768; // Дырка.
const short srtm_zer_interp   = 15000; // добавлено к интерполированным значениям
const short srtm_min_interp   = 10000; // для проверки

const int srtm_width = 1201; // файлы 1201х1201

const int max_lat = 90;
const int max_lon = 180;

extern const std::string def_srtm_dir;


class srtm3 {
public:

  srtm3(const std::string & _srtm_dir=def_srtm_dir,
        const unsigned cache_size=4 );

  // вернуть высоту точки
  short geth(const iPoint & p, const bool interp=false);
  short geth(const int x, const int y, const bool interp=false);

  // поменять высоту точки (только в кэше!)
  short seth(const iPoint & p, const short h);

  // вернуть высоту точки (вещественные координаты,
  // простая интерполяция по четырем соседним точкам)
  short geth4(const dPoint & p, const bool interp=false);

  // вернуть высоту точки (вещественные координаты,
  // интерполяция по 16 соседним точкам)
  short geth16(const dPoint & p, const bool interp=false);

  // найти множество соседних точек одной высоты (не более max точек)
  std::set<iPoint> plane(const iPoint& p, int max=1000);

  // move p0 to the local extremum (interpolation is always on)
  void move_to_extr(iPoint & p0, bool max);
  void move_to_min(iPoint & p0);
  void move_to_max(iPoint & p0);

  const double size0; // size (m) of 3 sec lat bow

  const double area0; // area (m^2) of 3x3 sec cell on equator

  // area (km2) of 3x3 sec cell at the given point
  double area(const iPoint &p) const;

  // slope (degrees) at a given point (holes are interpolated)
  double slope(const iPoint &p);

private:
  // директория с srtm-файлами
  std::string srtm_dir;

  // Ключ - широта в градусах, долгота в градусах
  Cache<iPoint, Image<short> > srtm_cache;

  // загрузить в кэш нужный файл 
  // (проверку, что он уже есть, здесь не производим)
  bool load(iPoint key);

/*
  // interpolate function between 4 points
  // for use in int16()
  short int4(int x1, int x2, int x3, int x4,
              int f1, int f2, int f3, int f4, double x);

  // the same with fixed distance between points
  short int4(int x1, int f1, int f2, int f3, int f4, double x);
*/
  short cubic_interp(const double h[4], const double x) const;

  void int_holes(double h[4]) const;

};

#endif
