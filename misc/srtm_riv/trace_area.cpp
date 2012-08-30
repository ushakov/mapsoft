#include <map>
#include <2d/rainbow.h>
#include <2d/point_int.h>
#include <srtm/srtm3.h>
#include <loaders/image_png.h>

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
is_flow(const iPoint &p, int dir){
  iPoint p1 = adjacent(p, dir);

  set<iPoint> P;
  P.insert(p1);
  set<iPoint> B = border(P);

  short h_thr = S.geth(p,true) +
    (down? -max_height: max_height);

  int extr; // min for rivers, max for mountains
  do {
    // найдем минимум на границе и добавим его в список
    int extr = down?-srtm_min:srtm_min;
    iPoint pm(p1);
    for (set<iPoint>::iterator b = B.begin(); b!=B.end(); b++){
      int h = S.geth(*b, true);
      if (h < srtm_min) continue;
      if ((down && (extr > h)) ||
         (!down && (extr < h))) { extr=h; pm=*b;}
    }
    if (pm==p) return true;
    if (done.count(pm)) return false;
    if ((down &&(extr < h_thr)) ||
        (!down &&(extr > h_thr))) return false;
    add_pb(pm, P,B);

  // Повторяем пока не превысили максимальное число точек.
  } while (P.size() < max_points);
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

