#include <iostream>
#include <map>
#include "map.h"

map::map(char *dir, int Lat1,int Lon1, int Lat2, int Lon2){
  lat1 = (Lat1<Lat2)? Lat1:Lat2;
  lat2 = (Lat1<Lat2)? Lat2:Lat1;
  lon1 = (Lon1<Lon2)? Lon1:Lon2;
  lon2 = (Lon1<Lon2)? Lon2:Lon1;
  w = lon2-lon1;
  h = lat2-lat1;
  // директория с hgt-файлами, размер кэша, интерполяция
  srtm s(dir, w/1200+2, mode_interp);
  // w/1200+2, так как приятно, когда не нужно перегружать картинки при проходе
  // каждой строки. Соседние могут пригодится при интерполяции

  for (int lat=lat1; lat<lat2; lat++){
    for (int lon=lon1; lon<lon2; lon++){
      map_pt p; 
      p.alt = s.geth(lat, lon);
      if (p.alt >srtm_min_interp) p.alt -=srtm_zer_interp;
      data.push_back(p);
    }
  }
}

map_pt* map::pt(int lat, int lon){
  if ((lat<lat1)||(lat>=lat2)||(lon<lon1)||(lon>=lon2)) return &p0;
  return &data[(lon-lon1)+(lat-lat1)*w];
}

short map::geth(int lat, int lon){
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

void map::rtrace(point p0, int rmax){

  int h0=geth(p0); // h0 - высота на последнем шаге
                   // (шаг происхоит строго вверх либо строго вниз)

  if (h0 < srtm_min) return; // мы вне карты
  if (pt(p0)->rdir != -1) return; // мы на уже обработаной территории

  std::list<point> L;            // список просмотренных точек
  L.push_back(p0);
  std::set<point> B = border(L); // его граница

  int n=0; // счетчик бессточных территорий

  point p;     // текущая точка
  point pe=p0; // точка последнего шага

  int min;
  do {
    // найдем минимум на границе и добавим его в список
    min = -srtm_min;
    for (std::set<point>::iterator b = B.begin(); b!=B.end(); b++){
      int h = geth(*b);
      if (min > h){ min=h; p=*b;}
    }
    add_pb(p, L,B);

    // если мы нашли более низкую точку, чем прошлая - делаем шаг:
    if ((min> srtm_min)&&(min<h0)) {pe=p; h0=min; n=0;}

    n++;
    // если мы уже ушли далеко, а шаг не сделали, то точка последнего 
    if (n>rmax) {pt(pe)->rdir = 8; p=pe; break;} // шага - бессточная

  } while ((min>srtm_min)&&(pt(p)->rdir==-1)); // пока не нашли сток (м.б. край карты)

  //теперь p - точка стока
  //делаем обратный проход:
  while (p!=p0){
    for (std::list<point>::iterator b = L.begin(); b!=L.end(); b++){
      int dir = b->isadjacent(p);
      if (dir != -1) { p=*b; pt(p)->rdir=dir; break;}
    }
  }
  return;
}


void map::mtrace(point p0, int mmax){

  int h0=geth(p0); // h0 - высота на последнем шаге
                   // (шаг происхоит строго вверх либо строго вниз)

  if (h0 < srtm_min) return; // мы вне карты
  if (pt(p0)->mdir != -1) return; // мы на уже обработаной территории

  std::list<point> L;            // список просмотренных точек
  L.push_back(p0);
  std::set<point> B = border(L); // его граница

  int n=0; // счетчик бессточных территорий

  point p;     // текущая точка
  point pe=p0; // точка последнего шага

  int max;
  do {
    // найдем максимум на границе и добавим его в список
    max = srtm_min;
    for (std::set<point>::iterator b = B.begin(); b!=B.end(); b++){
      int h = geth(*b);
      if (max < h){ max=h; p=*b;}
      if (h < srtm_min){ max=-srtm_min; p=*b;} // заграница считается стоком
    }
    add_pb(p, L,B);

    // если мы нашли более высокую точку, чем прошлая - делаем шаг:
    if ((max< -srtm_min)&&(max>h0)) {pe=p; h0=max; n=0;}

    n++;
    // если мы уже ушли далеко, а шаг не сделали, то точка последнего 
    if (n>mmax) {pt(pe)->mdir = 8; p=pe; break;} // шага - бессточная

  } while ((max < -srtm_min)&&(pt(p)->mdir==-1)); // пока не нашли сток (м.б. край карты)

  //теперь p - точка стока
  //делаем обратный проход:
  while (p!=p0){
    for (std::list<point>::iterator b = L.begin(); b!=L.end(); b++){
      int dir = b->isadjacent(p);
      if (dir != -1) { p=*b; pt(p)->mdir=dir; break;}
    }
  }
  return;
}


// Определение направлений для всех точек карты

void map::set_dirs(int rmax, int mmax){

  for (int lat=lat1; lat<lat2; lat+=1){
    std::cerr << lat-lat1  << "\n";
    for (int lon=lon1; lon<lon2; lon+=1){
      rtrace(point(lon,lat), rmax);
      mtrace(point(lon,lat), mmax);
    }
  }
}

// определение площадей для всех точек карты
void map::set_areas(void){
  for (int lat=lat1; lat<lat2; lat++){
    double area = pow(6380.0 * M_PI/srtm_width/180, 2)*
      cos((1.0*lat)/srtm_width * M_PI/180.0);
    std::cerr << lat-lat1  << "\n";

    for (int lon=lon1; lon<lon2; lon++){
      point p  = point(lon, lat);

      pt(p)->marea+=area;
      int dir = pt(p)->mdir;
      while ((dir > -1)&&(dir < 8)){ 
        p = p.adjacent(dir);
        pt(p)->marea+=area;
        dir = pt(p)->mdir;
      }
      p  = point(lon, lat);
      pt(p)->rarea+=area;
      dir = pt(p)->rdir;
      while ((dir > -1)&&(dir < 8)){ 
        p = p.adjacent(dir);
        pt(p)->rarea+=area;
        dir = pt(p)->rdir;
      }
    }
  }
}
