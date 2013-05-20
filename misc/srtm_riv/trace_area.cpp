#include <map>
#include <iomanip>
#include <2d/rainbow.h>
#include <2d/point_int.h>
#include <loaders/image_png.h>
#include <srtm/tracers.h>

// построение водосбора одной реки
using namespace std;

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

