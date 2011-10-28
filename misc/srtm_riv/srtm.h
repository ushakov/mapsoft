#ifndef SRTM_H
#define SRTM_H

#include <string>
#include <vector>
#include <map>

#include "point.h"

// Получение высоты любой точки из директории с srtm-данными
// Кэширование кусочков

// srtm-данные надо скачивать с ftp://e0mss21u.ecs.nasa.gov/srtm/

// Специальные значения для высоты
const short srtm_min      = -32000; // для проверки
const short srtm_nofile   = -32767; // нет такого файла
const short srtm_undef    = -32768; // Дырка.
const short srtm_zer_interp   = 15000; // добавлено к интерполированным значениям
const short srtm_min_interp   = 10000; // для проверки

const int srtm_width = 1200; // файлы 1201х1201

const unsigned mode_interp = 1;
const unsigned mode_noint  = 0;

const int max_lat = 90;
const int max_lon = 180;

class srtm {
  public: ////////////////////////////////

  srtm(const std::string srtm_dir="./", 
       const unsigned cache_size=10, 
       const unsigned mode=0
      ); 
  int geth(int lat, int lon); 
   
  private: ///////////////////////////////

  std::set<point> plane(const point& p);
  int geth_(int x, int y); 
  int seth_(int x, int y, short h);

  struct file{
    int nx, ny;
    short data[(srtm_width+1)*(srtm_width+1)];
  };
  
  int empty;
  std::vector<file*> world;
  std::vector<file>  cache;
  bool interp;
  
  std::vector<file>::iterator new_file;
  std::string dir;

  bool load(short *d, int nx, int ny);
};

#endif
