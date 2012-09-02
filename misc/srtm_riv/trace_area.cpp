#include <map>
#include <iomanip>
#include <2d/rainbow.h>
#include <2d/point_int.h>
#include <srtm/srtm3.h>
#include <loaders/image_png.h>
#include "trace_gear.h"

// построение водосбора одной реки
using namespace std;


// есть ли сток из p1 в p2?
bool
is_flow(srtm3 & S,
        const iPoint &p1, const iPoint &p2,
        map<iPoint,double> & done,
        int dh, int maxp, bool down){

  trace_gear G(S, p1);
  short h_thr = S.geth(p2,true) + (down? -dh: dh);

  do {
    iPoint p=G.go(down);
    int h=S.geth(p,true);
    if (p==p2) return true;
    if (done.count(p)) return false;
    if ((down && h < h_thr) || (!down && h > h_thr)) return false;
    // Повторяем пока не превысили максимальное число точек.
  } while (G.n < maxp);
  return false;
}

// recursively get area
double
get_a(srtm3 & S,
      const iPoint &p0,
      map<iPoint,double> & areas, // общая карта площадей
      map<iPoint,char> & dirs,    // общая карта стоков
      int dh, int maxp, bool down){

  double a=S.area(p0) * 1e-6; // площадь точки

  areas[p0]=0.0; // set some value before call get_a!
  for (int i=0; i<8; i++){
    iPoint p = adjacent(p0,i);
    if (areas.count(p) ||
        !is_flow(S, p, p0, areas, dh, maxp, down)) continue;
    dirs[p]=i;
    a+=get_a(S, p, areas, dirs, dh, maxp, down);
  }
  areas[p0]=a;
  return a;
}

// сортировка рек
list<list<iPoint> >
sort_areas(const map<iPoint,double> & areas,
           const map<iPoint,char> dirs, double mina){

  list<list<iPoint> > ret;
  set<iPoint> done;

  while(1){
    list<iPoint> riv;

    // find max area
    double ma = -1;
    iPoint mp;
    map<iPoint,double>::const_iterator ai;
    map<iPoint,char>::const_iterator di;
    for (ai=areas.begin(); ai!=areas.end(); ai++){
      if (done.count(ai->first) || ma >= ai->second ) continue;
      ma=ai->second; mp=ai->first;
    }

    // all points processed
    if (ma<mina) break;

    // go up from found point
    while(1){
      done.insert(mp);
      riv.push_back(mp);
      ma = -1;
      iPoint mp1;
      // found adjacent point which flows to mp with maximal area
      for (int i=0; i<8; i++){
        iPoint p=adjacent(mp,i);
        ai = areas.find(p);
        di = dirs.find(p);
        if (di == dirs.end() || ai == areas.end() ||
            di->second != i || ma >= ai->second) continue;

        ma=ai->second; mp1=p;
      }
      if (ma<mina) break;
      mp=mp1;
    }
    ret.push_back(riv);
  }
  return ret;
}


main(){

// построение водосбора одной реки
  dPoint p0(95.438170, 54.097332);
  bool down = true;
// или горы:
  //dPoint p0(95.786934, 54.057950);
  //bool down = false;

  int maxp = down?1000:1000;// макс. размер "неправильного" стока
  int dh   = down?200:500;  // макс. разница высот "неправильного" стока
  map<iPoint,double> areas;  // сюда будем складывать площади
  map<iPoint,char> dirs;     // сюда будем складывать направления стока
  srtm3 S;

  iPoint p(p0*1200.0);
  if (down) S.move_to_min(p); // сдвигаемся в локальный минимум
  else      S.move_to_max(p);

  // посчитаем площади водосбора
  cerr << "area: " << get_a(S, p, areas, dirs, dh, maxp, down) << "\n";

  // сортировка рек
  list<list<iPoint> > rivs = sort_areas(areas, dirs, 0.5);

  // print river to stdout
  list<list<iPoint> >::iterator ri;
  for (ri = rivs.begin(); ri!=rivs.end(); ri++){
    list<iPoint>::iterator pi;

    iPoint p = *ri->begin();
    if (dirs.count(p))
      ri->push_front(adjacent(p, dirs[p]+4));

    for (pi = ri->begin(); pi!=ri->end(); pi++){
      cout << pi->x << " " << pi->y << " "
           << setw(9) << setprecision(4) << fixed << areas[*pi] << " "
           << int(dirs[*pi]) << " " << setw(4) << S.geth(*pi) << "\n";
    }
    cout << "\n";
  }


  // find data range and border
  iRect r(p,p);
  set<iPoint> brd;
  map<iPoint,double>::const_iterator b;
  for (b = areas.begin(); b!=areas.end(); b++){
    r = rect_pump(r,b->first);
    for (int i=0; i<8; i++){
      iPoint p=adjacent(b->first, i);
      if (areas.count(p)==0) brd.insert(p);
    }
  }
  r = rect_pump(r,5);

  // fill image
  iImage img(r.w, r.h);
  simple_rainbow R(500,3000);
  for (int y=0; y<r.h; y++){
    for (int x=0; x<r.w; x++){
      iPoint p(x+r.x, y+r.y);
      short h  = S.geth(p, true);
      int c = 0xffffff;

      if (h > srtm_min && areas.count(p)){
        if (areas[p] > 0.5) c = 0x0;
        else c = R.get(h);
      }
      if (brd.count(p)) c=0x7F7F7F;
      img.set(x,r.h-y-1,c);
    }
  }
  image_png::save(img, "trace_area.png");
}

