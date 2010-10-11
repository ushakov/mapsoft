#ifndef SRTM3_H
#define SRTM3_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <string>
#include <vector>
#include <map>
#include <queue>

#include "2d/image.h"
#include "2d/point_int.h"
#include "2d/cache.h"

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
  Cache<iPoint, Image<short> > srtm_cache;

  // загрузить в кэш нужный файл 
  // (проверку, что он уже есть, здесь не производим)
  bool load(iPoint key){
    char NS='N';
    char EW='E';
    if (key.y<0) {NS='S'; key.y=-key.y;}
    if (key.x<0) {EW='W'; key.x=-key.x;}
    // название файла
    std::ostringstream file;
    file << srtm_dir << "/" 
         << NS << std::setfill('0') << std::setw(2) << key.y
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
          im.set(iPoint(x,srtm_width-1-y), (short)(unsigned char)c2 + ((short)(unsigned char)c1 <<8) );
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
  short geth_(const iPoint & p){
    iPoint key = p/(srtm_width-1);
    iPoint crd = p - key*(srtm_width-1);
 
    // в исправленных srtm-данных последняя строчка 
    // (которая стала 0-й) иногда содержит нули.
    // и ее не надо использовать!
    if (crd.y==0) {crd.y=srtm_width-1; key.y--;}

    while (key.x < -max_lon) key.x+=2*max_lon;
    while (key.x >= max_lon) key.x-=2*max_lon;
    if ((key.y<-max_lat)||(key.y>=max_lat)) return srtm_nofile;

    if ((!srtm_cache.contains(key)) && (!load(key))) return srtm_nofile;
    if (srtm_cache.get(key).empty()) return srtm_nofile;
    
    return srtm_cache.get(key).get(crd);
  }

  // поменять высоту точки (только в кэше!)
  short seth_(const iPoint & p, short h){
    iPoint key = p/(srtm_width-1);
    iPoint crd = p - key*(srtm_width-1);
    crd.y = (srtm_width-1)-crd.y;

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
  short geth(const iPoint & p){
    //сюда надо написать мого чего про интерполяцию!!!
    return geth_(p);
  }
  short geth(const int x, const int y){
    geth(iPoint(x,y));
  }

  // вернуть высоту точки (вещественные координаты, 
  // простая интерполяция по четырем соседним точкам)
  short geth(const dPoint & p){
    double x = p.x*1200;
    double y = p.y*1200;
    int x1 = floor(x);
    int x2 = x1+1;
    int y1 = floor(y);
    int y2 = y1+1;

    short h1=geth(x1,y1);
    short h2=geth(x1,y2);

    if ((h1<srtm_min)||(h2<srtm_min)) return srtm_undef;
    short h12 = (int)( h1+ (h2-h1)*(y-y1) );

    short h3=geth(iPoint(x2,y1));
    short h4=geth(iPoint(x2,y2));
    if ((h3<srtm_min)||(h4<srtm_min)) return srtm_undef;
    short h34 = (int)( h3 + (h4-h3)*(y-y1) );

    return (short)( h12 + (h34-h12)*(x-x1) );
  }


  // interpolate function between 4 points
  // for use in int16()
  short int4(int x1, int x2, int x3, int x4,
              int f1, int f2, int f3, int f4, double x){

    x-=x1; x2-=x1; x3-=x1; x4-=x1; x1=0;

    double k2=double(f2-f1)/double(x2-x1);
    double k3=double(f3-f2)/double(x3-x2);
    double k4=double(f4-f3)/double(x4-x3);

    double m3=(k3-k2)/double(x3-x1);
    double m4=(k4-k3)/double(x4-x2);

    double a = (m4-m3)/double(x4-x1);
    double b = m3 - a*(x1+x2+x3);
    double c = k2 - (x1*x1+x2*x2+x1*x2)*a - (x1+x2)*b;
    double d = f1 - a*x1*x1*x1 - b*x1*x1 - c*x1;

//    double c = k2 - (x1+x2)*m3 - (x1*x2 + x1*x3 + x2*x3)*a;
//    double d = f1 - x1*k2 + x1*x2*m3 - a*x1*x2*x3;

    return short(a*x*x*x + b*x*x + c*x + d);
  }

  // the same with fixed distance between points
  short int4(int x1, int f1, int f2, int f3, int f4, double x){

    x-=x1;
    x1=0;
    int x2=1;
    int x3=2;
    int x4=3;

    double k2=f2-f1;
    double k3=f3-f2;
    double k4=f4-f3;

    double m3=(k3-k2)/2.0;
    double m4=(k4-k3)/2.0;

    double a = (m4-m3)/3.0;
    double b = m3 - a*(x1+x2+x3);
    double c = k2 - (x1*x1+x2*x2+x1*x2)*a - (x1+x2)*b;
    double d = f1 - a*x1*x1*x1 - b*x1*x1 - c*x1;

//    double c = k2 - (x1+x2)*m3 - (x1*x2 + x1*x3 + x2*x3)*a;
//    double d = f1 - x1*k2 + x1*x2*m3 - a*x1*x2*x3;

    return short(a*x*x*x + b*x*x + c*x + d);
  }


  // вернуть высоту точки (вещественные координаты,
  // интерполяция по 16 соседним точкам)
  short geth16(const dPoint & p){

    double x = p.x*1200;
    double y = p.y*1200;
    int x0 = floor(x)-1;
    int y0 = floor(y)-1;

    int hx[4], hy[4];

    for (int i=0; i<4; i++){
      for (int j=0; j<4; j++) hx[j]=geth(x0+j, y0+i);
      hy[i]= int4(x0, hx[0],hx[1],hx[2],hx[3], x);
    }
    return int4(y0, hy[0],hy[1],hy[2],hy[3], y);
  }

  // найти множество соседних точек одной высоты (не более max точек)
  std::set<iPoint> plane(const iPoint& p, int max=1000){
    std::set<iPoint> ret;
    std::queue<iPoint> q;
    short h = geth_(p);

    q.push(p);
    ret.insert(p);

    while (!q.empty()){
      iPoint p1 = q.front();
      q.pop();
      for (int i=0; i<8; i++){
        iPoint p2 = adjacent(p1, i);
        if ((geth_(p2) == h)&&(ret.insert(p2).second)) q.push(p2);
      }
      if ((max!=0)&&(ret.size()>max)) break;
    }
    return ret;
  }

};



#endif
