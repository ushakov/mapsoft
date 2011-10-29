#ifndef MAP_H
#define MAP_H

#include <stdexcept>
#include <iostream>
#include <cmath>

#include <srtm/srtm3.h>
#include <2d/point.h>

// построение хребтовки-речевки

struct map_pt{ // точка на карте
  short  alt;        //altitude
  char   rdir, mdir; //directions 0-7, 
                     //-1 (неизвестное направление), 8 (бессточная точка)
  double rarea, marea;  //areas
                     // m- и r- -- для речной и хребтовой сети
  map_pt(){
    alt = srtm_undef;
    rdir=-1; mdir=-1;
    rarea=0; marea=0;
  }
};

struct map{
  std::vector<map_pt> data; // точки 
  int lat1,lat2,lon1,lon2;  // координаты углов в 3-х секундах
  int w;
  int h;
  map_pt p0; // точка по умолчанию.

  map(int Lat1,int Lon1, int Lat2, int Lon2, const char *dir=def_srtm_dir.c_str());

  map_pt* pt(int lat, int lon);
  map_pt* pt(iPoint p){ return pt(p.y, p.x);}

  short   geth(int lat, int lon);
  short   geth(iPoint p){ return geth(p.y, p.x);}


  void rtrace(iPoint p, int rmax);
  void mtrace(iPoint p, int mmax);
  void set_dirs(int rmax=1000, int mmax=1000);
  void set_areas(void);

};
#endif
