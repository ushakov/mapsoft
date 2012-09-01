#include <map>
#include <2d/rainbow.h>
#include <2d/point_int.h>
#include <srtm/srtm3.h>
#include <loaders/image_png.h>
#include "trace_gear.h"

// построение прямоугольной хребтовки

dPoint p00g(95.4, 53.9); // ЮЗ угол
int w=800, h=600;      // размер карты
bool down = true;
int nmax=1000; // максимальный размер бессточных областей

iPoint p00 = p00g*1200.0;
Image<char> dirs(w,h,-1); // направления стока
Image<double> areas(w,h,0); // площади водосбора
srtm3 S;

using namespace std;

void
go_down(int x, int y){
  // проход вниз
  trace_gear G(S, p00+iPoint(x,y));
  if (G.h0 < srtm_min) return; // мы вне карты

  vector<iPoint> L; // упорядоченный список просмотренных точек
  L.push_back(G.p0);

  iPoint p;
  do {
    p=G.go(down);
    L.push_back(p);
    // если мы уже ушли далеко, а шаг не сделали, то точка
    // последнего шага - бессточная, вернемся в нее
    if (G.ns>nmax) { p=G.pp; dirs.set(p.x-p00.x, p.y-p00.y,8); break; }

  // идем до края карты или уже обработанной точки
  } while (dirs.safe_get(p.x-p00.x, p.y-p00.y,-2)==-1);

  // обратный проход от p до p0
  while (p!=G.p0){
    vector<iPoint>::const_iterator b;
    for (b = L.begin(); b!=L.end(); b++){
      int dir = isadjacent(*b, p);
      if (dir==-1) continue;
      p=*b;
      dirs.set(p.x-p00.x, p.y-p00.y, dir);
      break;
    }
  }
}


main(){

  // для каждой точки карты трассируем путь вниз до уже
  // обработанной точки, до края или до бессточной области
  for (int y=0; y<h; y++){
    for (int x=0; x<w; x++){
      if (dirs.get(x,y)!=-1) continue;
      go_down(x,y);
    }
  }

  // определение площадей водосбора
  double area = S.area(p00)/1e6; // примерная площадь одной точки
  for (int y=0; y<h; y++){
    cerr << "y = " << y << "/" << h << "\n";
    for (int x=0; x<w; x++){
      iPoint p  = iPoint(x, y);
      do {
        areas.set(p.x,p.y, areas.get(p.x,p.y) + area);
        int dir = dirs.get(p.x,p.y);
        if (dir < 0 || dir > 7) break;
        p = adjacent(p, dir);
      } while (p.x>=0 && p.y>=0 && p.x<w && p.y<h);
    }
  }

  // fill image
  iImage img(w, h);
  simple_rainbow R(500,3000);
  for (int y=0; y<h; y++){
    for (int x=0; x<w; x++){
      int c = R.get(S.geth(p00+iPoint(x,y), true));
      if (areas.get(x,y)>0.5) c=0x0;
      img.set(x,h-y-1,c);
    }
  }
  image_png::save(img, "trace_map.png");
}

