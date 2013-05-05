#include <vector>
#include <set>
#include <2d/point_int.h>
#include <2d/rainbow.h>
#include <srtm/srtm3.h>
#include <loaders/image_png.h>
#include "trace_gear.h"

// алгоритм нахождения русла реки
using namespace std;

set<iPoint>
trace(srtm3 & S, const iPoint & p0, int nmax, int hmin, bool down){

  trace_gear G(S, p0);
  if (G.h0 < srtm_min) return set<iPoint>(); // мы вне карты

  vector<iPoint> L; // упорядоченный список просмотренных точек
  L.push_back(p0);

  do {
    L.push_back(G.go(down));
    // если мы уже ушли далеко, а шаг не сделали, то точка
    // последнего шага - бессточная. Возвращаемся к ней.
    if (G.ns > nmax) { L.push_back(G.pp); break; }

    // критерий выхода: до высоты hmin, или края данных
  } while (G.hp > hmin);

  iPoint p = L[L.size()-1];
  set<iPoint> ret;
  // обратный проход от p до p0
  while (p!=p0){
    vector<iPoint>::const_iterator b;
    for (b = L.begin(); b!=L.end(); b++){
      if (isadjacent(*b, p)<0) continue;
      p=*b;
      ret.insert(p);
      break;
    }
  }
  return ret;
}

main(){
  dPoint p0(95.786934, 54.057950);
  srtm3 S;

  bool down=true;
  int nmax=1000; // максимальный размер бессточных областей
  int hmin=600;  // минимальная высота - критерий остановки

  set<iPoint> R = trace(S, p0*1200, nmax, hmin, down);
  if (!R.size()) exit(1);

  // find data range
  iRect r(*R.begin(),*R.begin());
  set<iPoint>::const_iterator b;
  for (b = R.begin(); b!=R.end(); b++) r = rect_pump(r,*b);
  r = rect_pump(r,5);

  //
  iImage img(r.w, r.h);
  simple_rainbow rb(500,3000);

  for (int y=0; y<r.h; y++){
    for (int x=0; x<r.w; x++){
      iPoint p(x+r.x, y+r.y);
      short h  = S.geth(p, true);
      int c = rb.get(h);
      if (R.count(p)) c = 0x0;
      img.set_na(x,r.h-y-1,c);
    }
  }
  image_png::save(img, "trace_one.png");
}
