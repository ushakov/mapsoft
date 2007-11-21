#ifndef SRTM3_H
#define SRTM3_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <string>
#include <vector>
#include <map>

#include "image.h"
#include "point.h"
#include "cache.h"

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

const unsigned interp_mode_off = 0;
const unsigned interp_mode_on  = 1; // интерполировать
const unsigned interp_mode_add = 2; // интерполировать, добавлять srtm_zer_interp

const int max_lat = 90;
const int max_lon = 180;


class srtm3 {
  // директория с srtm-файлами
  std::string srtm_dir;
 
  // режим интерполяции
  unsigned interp_mode;

  // Ключ - широта в градусах, долгота в градусах
  Cache<Point<int>, Image<short> > srtm_cache;

  // загрузить в кэш нужный файл 
  // (проверку, что он уже есть, здесь не производим)
  bool load(Point<int> key){
    char NS='N';
    char EW='E';
    if (key.y<0) {NS='S'; key.y=-key.y;}
    if (key.x<0) {EW='W'; key.x=-key.x;}
    // название файла
    std::ostringstream file;
    file << srtm_dir << NS << std::setfill('0') << std::setw(2) << key.y
                     << EW << std::setw(3) << key.x << ".hgt";

    std::ifstream in(file.str().c_str());
    if (!in){
      std::cerr << "can't find file " << file.str() << '\n';
      srtm_cache.add(key, Image<short>(0,0));
      return false;
    }
    char c1, c2;
    Image<short> im(srtm_width,srtm_width);
    for (int y=0; y < srtm_width; y++){
      for (int x=0; x < srtm_width; x++){
        if (in.get(c1) && in.get(c2))
          im.set(Point<int>(x,y), (short)(unsigned char)c2 + ((short)(unsigned char)c1 <<8) );
        else {
          std::cerr << "error while reading from file " << file.str() << '\n';
          srtm_cache.add(key, Image<short>(0,0));
          return false;
        }
      }
    }
    srtm_cache.add(key, im);
    return true;
  }

  // Узнать высоту точки без интерполяции дыр
  short geth_(const Point<int> & p){
    Point<int> key = p/(srtm_width-1);
    Point<int> crd = p - key*(srtm_width-1);

    while (key.x < -max_lon) key.x+=2*max_lon;
    while (key.x >= max_lon) key.x-=2*max_lon;
    if ((key.y<-max_lat)||(key.y>=max_lat)) return srtm_nofile;

    if ((!srtm_cache.contains(key)) && (!load(key))) return srtm_nofile;
    if (srtm_cache.get(key).empty()) return srtm_nofile;
    
    return srtm_cache.get(key).get(crd);
  }

  // поменять высоту точки (только в кэше!)
  short seth_(const Point<int> & p, short h){
    Point<int> key = p/(srtm_width-1);
    Point<int> crd = p - key*(srtm_width-1);

    if ((!srtm_cache.contains(key)) && (!load(key))) return srtm_nofile;
    srtm_cache.get(key).set(crd, h);
    return h;
  }

  
  public: ////////////////////////////////

  srtm3(const std::string _srtm_dir="./", 
       const unsigned cache_size=10, 
       const unsigned mode=interp_mode_off
      ) : srtm_cache(cache_size), srtm_dir(_srtm_dir), interp_mode(mode){};

  // вернуть высоту точки
  short geth(const Point<int> & p){
    //сюда надо написать мого чего про интерполяцию!!!
    return geth_(p);
  } 

  // вернуть высоту точки (вещественные координаты, 
  // простая интерполяция по четырем соседним точкам)
  short geth(const Point<double> & p){
    int y1 = floor(p.y*1200);
    int y2 = ceil(p.y*1200);
    int x1 = floor(p.x*1200);
    int x2 = ceil(p.x*1200);

    short h1=geth(Point<int>(x1,y1));
    short h2=geth(Point<int>(x1,y2));

    if ((h1<srtm_min)||(h2<srtm_min)) return srtm_undef;
    short h12 = (int)( h1+ (h2-h1)*(p.y*1200-y1)/double(y2-y1) );

    short h3=geth(Point<int>(x2,y1));
    short h4=geth(Point<int>(x2,y2));
    if ((h3<srtm_min)||(h4<srtm_min)) return srtm_undef;
    short h34 = (int)( h3 + (h4-h3)*(p.y*1200-y1)/double(y2-y1) );

    return (short)( h12 + (h34-h12)*(p.x*1200-x1)/double(x2-x1) );
  }
   
//  std::set<Point<int> > plane(const Point<int>& p, int max=0);
};

#endif
