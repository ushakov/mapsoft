#include <map>
#include <iomanip>
#include <2d/rainbow.h>
#include <2d/point_int.h>
#include <srtm/srtm3.h>
#include <loaders/image_png.h>
#include "trace_gear.h"

// построение водосбора одной реки
using namespace std;


// нахождение площади водосбора одной реки/горы
class trace_area{
  public:
  set<iPoint> done;          // обработанные точки
  map<iPoint,double> areas;  // сюда будем складывать площади
  map<iPoint,char> dirs;     // сюда будем складывать направления стока

  bool down;   // направление стока true=вниз
  int dh;      // макс. разница высот "неправильного" стока
  int maxp;    // макс. размер "неправильного" стока
  double mina; // минимальный размер рек, который запоминается
  srtm3 & S;   // интерфейс к srtm-данным

  double maxa; // максимальная площадь водосбора (-1, если нет ограничения)
  double suma;

  trace_area(srtm3 & S_, int dh_, int maxp_, double mina_, bool down_):
          S(S_), dh(dh_), maxp(maxp_), mina(mina_), maxa(-1), suma(0), down(down_){ }

  // есть ли сток из p1 в p2?
  bool is_flow(const iPoint &p1, const iPoint &p2){

    trace_gear G(S, p1);
    short h_thr = S.geth(p2,true) + (down? -dh: dh);

    do {
      iPoint p=G.go(down);
      if (p==p2) return true;
      if (done.count(p)) return false;
      int h=S.geth(p,true);
      if ((down && h < h_thr) || (!down && h > h_thr)) return false;
      // Повторяем пока не превысили максимальное число точек.
    } while (G.n < maxp);
    return false;
  }

  // recursively get area
  double get_a(const iPoint &p0){
    double a=S.area(p0) * 1e-6; // площадь точки

    if (maxa > 0 && (suma+=a) > maxa) return 0;

    done.insert(p0);
    for (int i=0; i<8; i++){
      iPoint p = adjacent(p0,i);
      if (done.count(p) || !is_flow(p, p0)) continue;
      double a1 = get_a(p);
      if (a1>mina) dirs[p]=i;
      a+=a1;
    }
    if (a>mina) areas[p0]=a;
    return a;
  }

  // сортировка рек
  list<list<iPoint> > sort_areas(){

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
      if (ma<0) break;

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
        if (ma<0) break;
        mp=mp1;
      }
      ret.push_back(riv);
    }
    return ret;
  }

};





main(){

// построение водосбора одной реки
  dPoint p0(67.804847, 37.076098); // Амударья
  bool down = true;
//  dPoint p0(95.438170, 54.097332); // Кизир
//  bool down = true;
// или горы:
  //dPoint p0(95.786934, 54.057950);
  //bool down = false;

  double mina = 0.5;
  int maxp = down?10000:10000;// макс. размер "неправильного" стока
  int dh   = down?200:200;  // макс. разница высот "неправильного" стока
  srtm3 S;

  trace_area T(S, dh, maxp, mina, down);

//  T.maxa=400;

  iPoint p(p0*1200.0);
  if (down) S.move_to_min(p); // сдвигаемся в локальный минимум
  else      S.move_to_max(p);

  // посчитаем площади водосбора
  cerr << "area: " << T.get_a(p) << "\n";

  // сортировка рек
  list<list<iPoint> > rivs = T.sort_areas();

  // print river to stdout
  list<list<iPoint> >::iterator ri;
  for (ri = rivs.begin(); ri!=rivs.end(); ri++){
    list<iPoint>::iterator pi;

    iPoint p = *ri->begin();
    if (T.dirs.count(p))
      ri->push_front(adjacent(p, T.dirs[p]+4));

    for (pi = ri->begin(); pi!=ri->end(); pi++){
      cout << pi->x << " " << pi->y << " "
           << setw(9) << setprecision(4) << fixed << T.areas[*pi] << " "
           << int(T.dirs[*pi]) << " " << setw(4) << S.geth(*pi) << "\n";
    }
    cout << "\n";
  }


  // find data range and border
  iRect r(p,p);
  set<iPoint> brd;
  set<iPoint>::const_iterator b;
  for (b = T.done.begin(); b!=T.done.end(); b++){
    r = rect_pump(r,*b);
    for (int i=0; i<8; i++){
      iPoint p=adjacent(*b, i);
      if (T.done.count(p)==0) brd.insert(p);
    }
  }
  r = rect_pump(r,5);

  // fill image
  iImage img(r.w, r.h);
  simple_rainbow R(500,6000);
  for (int y=0; y<r.h; y++){
    for (int x=0; x<r.w; x++){
      iPoint p(x+r.x, y+r.y);
      short h  = S.geth(p, true);
      int c = 0xffffff;

      if (h > srtm_min && T.done.count(p)){
        if (T.areas.count(p)) c = 0x0;
        else c = R.get(h);
      }
      if (brd.count(p)) c=0x7F7F7F;
      img.set_na(x,r.h-y-1,c);
    }
  }
  image_png::save(img, "trace_area.png");
}

