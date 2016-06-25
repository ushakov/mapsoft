#ifndef SRTM3_H
#define SRTM3_H

#include <string>
#include <glibmm.h>

#include <2d/image.h>
#include <2d/cache.h>

/*
 SRTM3 data
 Original data can be downloaded from ftp://e0mss21u.ecs.nasa.gov/srtm/
 Fixed data can be downloaded from http://www.viewfinderpanoramas.org/dem3.html

 Default data directory is DIR=$HOME/.srtm_data
 Files are searched in
   $DIR/fixed/$file.gz, $DIR/fixed/$file, $DIR/$file.gz, $DIR/$file,
 so you can keep original data in $HOME/.srtm_data and fixed data
 in $HOME/.srtm_data/fixed
*/

const short srtm_min      = -32000; // value for testing
const short srtm_nofile   = -32767; // file not found
const short srtm_undef    = -32768; // hole in srtm data
const short srtm_zer_interp   = 15000; // добавлено к интерполированным значениям
const short srtm_min_interp   = 10000; // для проверки


const int max_lat = 90;
const int max_lon = 180;

class SRTM3 {
public:

  SRTM3(
    const std::string & _srtm_dir=std::string(), // data directory ($HOME/.srtm_data)
    const unsigned cache_size=4
  );

  void set_dir(const std::string & str);
  const std::string & get_dir(void) const;
  const unsigned get_width(void) const;

  // вернуть высоту точки
  short geth(const iPoint & p, const bool interp=false);
  short geth(const int x, const int y, const bool interp=false);

  // Slope (degrees) at a given point
  // Slope is calculated for p+(1/2,1/2)
  double slope(const iPoint &p, const bool interp=true);
  double slope(const int x, const int y, const bool interp=true);

  // change data (in cache only!)
  short seth(const iPoint & p, const short h);

  // вернуть высоту точки (вещественные координаты,
  // простая интерполяция по четырем соседним точкам)
  short geth4(const dPoint & p, const bool interp=false);

  // Slope (degrees) at a given point
  double slope4(const dPoint &p, const bool interp=true);

  // вернуть высоту точки (вещественные координаты,
  // интерполяция по 16 соседним точкам)
  short geth16(const dPoint & p, const bool interp=false);


  // найти множество соседних точек одной высоты (не более max точек)
  std::set<iPoint> plane(const iPoint& p, int max=1000);

  // move p0 to the local extremum (interpolation is always on)
  void move_to_extr(iPoint & p0, bool max);
  void move_to_min(iPoint & p0);
  void move_to_max(iPoint & p0);

  double size0; // size (m) of 3 sec lat bow
  double area0; // area (m^2) of 3x3 sec cell on equator
  size_t srtm_width; // файлы 1201х1201

  // area (km2) of 3x3 sec cell at the given point
  double area(const iPoint &p) const;

  // making some vector data: contours, peaks, srtm holes
  std::map<short, dMultiLine> find_contours(const dRect & range, int step);
  std::map<dPoint, short>  find_peaks(const dRect & range, int DH, int PS);
  dMultiLine find_holes(const dRect & range);

private:
  // data directories
  std::string srtm_dir;

  // data cache. key is lon,lat in degrees
  Cache<iPoint, sImage > srtm_cache;

  // load data into cache
  bool load(const iPoint & key);

  short cubic_interp(const double h[4], const double x) const;

  void int_holes(double h[4]) const;

  Glib::Mutex mutex;
};

#endif
