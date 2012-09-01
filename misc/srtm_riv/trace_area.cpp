#include <map>
#include <2d/rainbow.h>
#include <2d/point_int.h>
#include <srtm/srtm3.h>
#include <loaders/image_png.h>
#include "trace_gear.h"

// построение водосбора одной реки
dPoint p0(95.438170, 54.097332);
bool down = true;
// или горы:
//dPoint p0(95.786934, 54.057950);
//bool down = false;

int max_points = down?1000:1000;// макс. размер "неправильного" стока
int max_height = down?200:500;  // макс. разница высот "неправильного" стока
std::map<iPoint,double> done;   // сюда будем складывать результат
srtm3 S;

using namespace std;


// есть ли сток из направления dir в данную точку?
bool
is_flow(const iPoint &p0, int dir){
  iPoint p1 = adjacent(p0, dir);
  trace_gear G(S, p1);
  short h_thr = S.geth(p0,true) + (down? -max_height: max_height);

  do {
    iPoint p=G.go(down);
    int h=S.geth(p,true);
    if (p==p0) return true;
    if (done.count(p)) return false;
    if ((down && h < h_thr) || (!down && h > h_thr)) return false;
  // Повторяем пока не превысили максимальное число точек.
  } while (G.n < max_points);
  return false;
}

// recursively get area
double
get_a(iPoint p0){
  double a=S.area(p0) * 1e-6;
  done[p0]=0.0;
  for (int i=0; i<8; i++){
    if (done.count(adjacent(p0,i))) continue;
    if (is_flow(p0, i))
      a+=get_a(adjacent(p0,i));
  }
  done[p0]=a;
  return a;
}


main(){

  iPoint p(p0*1200.0);
  if (down) S.move_to_min(p);
  else      S.move_to_max(p);

  cerr << "area: " << get_a(p) << "\n";

  // find data range and border
  iRect r(p,p);
  set<iPoint> brd;
  map<iPoint,double>::const_iterator b;
  for (b = done.begin(); b!=done.end(); b++){
    r = rect_pump(r,b->first);
    for (int i=0; i<8; i++){
      iPoint p=adjacent(b->first, i);
      if (done.count(p)==0) brd.insert(p);
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

      if (h > srtm_min && done.count(p)){
        if (done[p] > 0.5) c = 0x0;
        else c = R.get(h);
      }
      if (brd.count(p)) c=0x7F7F7F;
      img.set(x,r.h-y-1,c);
    }
  }
  image_png::save(img, "trace_area.png");
}

