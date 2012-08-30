#include "tracer.h"
#include <2d/point_int.h>

tracer::tracer(const std::string & dir, int cache_size):
    S(dir, cache_size){
}

void
tracer::trace(const dPoint & p, bool down=true){
  start_pt = p*3600/3; // 3-sec srtm units

  max_points=1000;
  min_area=0.5;

  if (down){
    S.move_to_min(start_pt);
    max_height=200;
  }
  else{
    S.move_to_max(start_pt);
    max_height=500;
  }

  river=down;

  done.clear();
  res.clear();
  get_a(start_pt);
}

// есть ли сток из направления dir в данную точку?
bool
tracer::is_flow(const iPoint &p, int dir){
  iPoint p1 = adjacent(p, dir);

  std::set<iPoint> P;
  P.insert(p1);
  std::set<iPoint> B = border(P);

  short h_thr = S.geth(p,true) +
    (river? -max_height: max_height);

  int extr; // min for rivers, max for mountains
  do {
    // найдем минимум на границе и добавим его в список
    int extr = river?-srtm_min:srtm_min;
    iPoint pm(p1);
    for (std::set<iPoint>::iterator b = B.begin(); b!=B.end(); b++){
      int h = S.geth(*b, true);
      if (h < srtm_min) continue;
      if ((river && (extr > h)) ||
         (!river && (extr < h))) { extr=h; pm=*b;}
    }
    if (pm==p) return true;
    if (done.count(pm)) return false;
    if ((river &&(extr < h_thr)) ||
        (!river &&(extr > h_thr))) return false;

    add_pb(pm, P,B);

  // Повторяем пока не превысили максимальное число точек.
  } while (P.size() < max_points);

  return false;
}

// recursively get area
double
tracer::get_a(iPoint p0){
  double a=S.area(p0) * 1e-6;
  done.insert(p0);
  for (int i=0; i<8; i++){
    if (done.count(adjacent(p0,i))) continue;
    if (is_flow(p0, i))
      a+=get_a(adjacent(p0,i));
  }
//  if (a>min_area)
  res[p0]=a;
  return a;
}

iRect
tracer::get_range() const{
  iRect ret;
  std::set<iPoint>::const_iterator i;
  for (i=done.begin(); i!=done.end(); i++){
    if (i==done.begin()) ret=iRect(*i,*i);
    else ret=rect_pump(ret, *i);
  }
  return ret;
}

std::set<iPoint>
tracer::get_border() const{
  return border(done);
}

