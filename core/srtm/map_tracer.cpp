#include <iostream>
#include <map>
#include "map_tracer.h"
#include <2d/point_int.h>

map_tracer::map_tracer(int Lat1,int Lon1, int Lat2, int Lon2,
                       const std::string & dir){
  lat1 = (Lat1<Lat2)? Lat1:Lat2;
  lat2 = (Lat1<Lat2)? Lat2:Lat1;
  lon1 = (Lon1<Lon2)? Lon1:Lon2;
  lon2 = (Lon1<Lon2)? Lon2:Lon1;
  w = lon2-lon1;
  h = lat2-lat1;
  // директория с hgt-файлами, размер кэша
  srtm3 s(dir, 2*w/1200+2);
  // 2*w/1200+2, так как приятно, когда не нужно перегружать картинки при проходе
  // каждой строки. Соседние могут пригодится при интерполяции

  for (int lat=lat1; lat<lat2; lat++){
    for (int lon=lon1; lon<lon2; lon++){
      map_pt p;
      p.alt = s.geth(lon, lat, true);
      if (p.alt >srtm_min_interp) p.alt -=srtm_zer_interp;
      data.push_back(p);
    }
  }
}

map_pt* map_tracer::pt(int lat, int lon){
  if ((lat<lat1)||(lat>=lat2)||(lon<lon1)||(lon>=lon2)) return &p0;
  return &data[(lon-lon1)+(lat-lat1)*w];
}

short map_tracer::geth(int lat, int lon){
  if ((lat<lat1)||(lat>=lat2)||(lon<lon1)||(lon>=lon2)) return srtm_undef;
  return data[(lon-lon1)+(lat-lat1)*w].alt;
}


/*
Процедура движения вниз по реке (или вверх по хребту) до упора:
1. строим список точек, начиная с исходной
   h0 - высота исходной точки
2. в границе списка ищим самую нижнюю точку
3. если она не ниже h0 -- добавляем ее в список и -> п.2.
   если размер списка > max_list -- отмечаем исходную точку как бессточную
Мы нашли точку, более низкую, чем исходная. 
   теперь h0 - высота этой точки.
Повторяем все, пока не дойдем до края карты или до уже обработанной
точки (у которой отмечено направление). Это будет конечная точка.

Строим путь от нее к  исходной точке: на каждом шаге среди соседей
данной точки ищем самую  раннюю точку из списка и переходим к ней. В
точках отмечаем направления.

При построения профеля отдельной реки имейте в виду
обратное и боковое затопление! (см. one_river.cpp)
*/

void map_tracer::trace(iPoint p0, bool up, int rmax){

  int h0=geth(p0); // h0 - высота на последнем шаге
                   // (шаг происхоит строго вверх либо строго вниз)

  if (h0 < srtm_min) return; // мы вне карты
  if ((up && (pt(p0)->mdir != -1)) ||
     (!up && (pt(p0)->rdir != -1))) return; // мы на уже обработаной территории

  std::vector<iPoint> L;            // список просмотренных точек
  std::set<iPoint> P;
  L.push_back(p0);
  P.insert(p0);
  std::set<iPoint> B = border(P); // его граница

  int n=0; // счетчик бессточных территорий

  iPoint p;     // текущая точка
  iPoint pe=p0; // точка последнего шага

  int extr; // min for rivers, max for mountains
  do {
    // найдем минимум на границе и добавим его в список
    extr = up?srtm_min:-srtm_min;
    for (std::set<iPoint>::iterator b = B.begin(); b!=B.end(); b++){
      int h = geth(*b);
      if (h < srtm_min) continue;
      if ((up && (extr < h)) ||
         (!up && (extr > h))) { extr=h; p=*b;}
    }
    add_pb(p, P,B);
    L.push_back(p);

    // если мы нашли более низкую точку, чем прошлая - делаем шаг:
    if ((up && (extr > h0)) ||
       (!up && (extr < h0))) {pe=p; h0=extr; n=0;}

    n++;
    // если мы уже ушли далеко, а шаг не сделали, то точка последнего 
    if (n>rmax) {
      if (up) pt(pe)->mdir = 8;
      else    pt(pe)->rdir = 8;
      p=pe; break;
    } // шага - бессточная

  } while ((up && (pt(p)->mdir==-1)) ||
          (!up && (pt(p)->rdir==-1))); // пока не нашли сток (м.б. край карты)

  //теперь p - точка стока
  //делаем обратный проход:
  while (p!=p0){
    for (std::vector<iPoint>::const_iterator b = L.begin(); b!=L.end(); b++){
      int dir = isadjacent(*b, p);
      if (dir != -1) {
        p=*b;
        if (up) pt(p)->mdir=dir;
        else    pt(p)->rdir=dir;
        break;
      }
    }
  }
  return;
}

// Определение направлений для всех точек карты

void map_tracer::set_dirs(int rmax, int mmax){

  for (int lat=lat1; lat<lat2; lat+=1){
    std::cerr << lat-lat1  << "\n";
    for (int lon=lon1; lon<lon2; lon+=1){
      trace(iPoint(lon,lat), false, rmax);
      trace(iPoint(lon,lat), true,  mmax);
    }
  }
}

// определение площадей для всех точек карты
void map_tracer::set_areas(void){
  for (int lat=lat1; lat<lat2; lat++){
    double area = pow(6380.0 * M_PI/srtm_width/180, 2)*
      cos((1.0*lat)/srtm_width * M_PI/180.0);
    std::cerr << lat-lat1  << "\n";

    for (int lon=lon1; lon<lon2; lon++){
      iPoint p  = iPoint(lon, lat);

      pt(p)->marea+=area;
      int dir = pt(p)->mdir;
      while ((dir > -1)&&(dir < 8)){ 
        p = adjacent(p, dir);
        pt(p)->marea+=area;
        dir = pt(p)->mdir;
      }
      p  = iPoint(lon, lat);
      pt(p)->rarea+=area;
      dir = pt(p)->rdir;
      while ((dir > -1)&&(dir < 8)){ 
        p = adjacent(p, dir);
        pt(p)->rarea+=area;
        dir = pt(p)->rdir;
      }
    }
  }
}
