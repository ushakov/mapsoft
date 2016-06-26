#ifndef SRTM_TRACERS_H
#define SRTM_TRACERS_H

#include <set>
#include <map>
#include <vector>
#include <2d/point_int.h>
#include <srtm/srtm3.h>

/* Useful functions for srtm tracing.
   Examples can be found in misc/srtm_riv and in viewer/actions. */

/********************************************************************/
// common object for all tracers
//
class trace_gear{
public:
  SRTM3 & S;
  std::set<iPoint> P, B; // processed points, border of processed area
  int n,ns;              // counters: all points, areas without sink
  iPoint p0,pp;          // starting point, last-step point
  int h0,hp;             // starting heigth, last-step height

  trace_gear(SRTM3 & S_, const iPoint & p0_):
      S(S_), p0(p0_), pp(p0_), n(0), ns(0){
    h0 = hp = S.geth(p0);
    P.insert(p0);
    for (int i=0; i<8; i++) B.insert(adjacent(p0,i));
  }

  // Do a single step to the next point.
  // If it is possible, the point is lower then previous,
  // then ns is set to 0.
  // down parameter -- is the flow goes down (river) of up (mountain)
  iPoint go(bool down){
    // find adjacent point (member of B) with minimum height
    iPoint p=*B.begin();
    int h = S.geth(p, true);
    std::set<iPoint>::iterator b;
    for (b = B.begin(); b!=B.end(); b++){
      int h1 = S.geth(*b, true);
      if (h1 < srtm_min) continue;
      if ((!down && h < h1) || ( down && h > h1)) { h=h1; p=*b;}
    }

    // add found point into P, recalculate B
    P.insert(p);
    B.erase(p);
    for (int i=0; i<8; i++){
      iPoint p2 = adjacent(p, i);
      if (P.count(p2)==0) B.insert(p2);
    }

    n++; ns++;
    // If found point is lower than that on a previous step,
    // do the next step, reset ns counter
    if ((!down && h > hp) || (down && h < hp)) { hp=h; ns=0; pp=p; }
    return p;
  }
};


/********************************************************************/
// Trace one river.
// Parameters:
//    p0 -- starting point
//    nmax -- no-sink area when we stop calculation
//    hmin -- threshold height where we want to stop calculation
//    down -- is the flow goes down (river) of up (mountain)
std::vector<iPoint>
trace_river(SRTM3 & S, const iPoint & p0, int nmax, int hmin, bool down){

  trace_gear G(S, p0);
  if (G.h0 < srtm_min) return std::vector<iPoint>(); // outside the map!

  std::vector<iPoint> L; // sorted vector of our steps
  L.push_back(p0);

  do {
    // do one step down
    L.push_back(G.go(down));

    // if we went far but didn't find lower point, then
    // it id the end of river!
    // Return back to previous lowest point and break.
    if (G.ns > nmax) { L.push_back(G.pp); break; }

    // stop the loop if point is lower then hmin (or we outside the data)
  } while (G.hp > hmin);

  iPoint p = L[L.size()-1];
  std::vector<iPoint> ret;

  // Now go back from p to p0 and build the river line.
  // On each step look for adjecent and the aerliest point in L.
  while (p!=p0){
    std::vector<iPoint>::const_iterator b;
    for (b = L.begin(); b!=L.end(); b++){
      if (isadjacent(*b, p)<0) continue;
      p=*b;
      ret.push_back(p);
      break;
    }
  }
  return ret;
}

/********************************************************************/
// Trace area of one river/mountain
//
class trace_area{
  public:
  std::set<iPoint> done;          // processed points
  std::map<iPoint,double> areas;  // processed areas
  std::map<iPoint,char> dirs;     // directions of flow

  bool down;   // is the flow goes down (river) of up (mountain)
  int dh;      // max height difference of a "wrong sink"
  int maxp;    // max size of a "wrong sink"
  double mina; // collect data for rivers larger then mina area, km^2
  SRTM3 & S;   // SRTM data

  double maxa; // max area (-1 for no limit)
  double suma;

  trace_area(SRTM3 & S_, int dh_, int maxp_, double mina_, bool down_):
          S(S_), dh(dh_), maxp(maxp_), mina(mina_), maxa(-1), suma(0), down(down_){ }

  // is there a sink from p1 to p2?
  bool is_flow(const iPoint &p1, const iPoint &p2){

    trace_gear G(S, p1);
    short h_thr = S.geth(p2,true) + (down? -dh: dh);

    do {
      iPoint p=G.go(down);
      if (p==p2) return true;
      // we found already processed point
      if (done.count(p)) return false;
      int h=S.geth(p,true);
      // dh limit
      if ((down && h < h_thr) || (!down && h > h_thr)) return false;
      // maxp limit
    } while (G.n < maxp);
    return false;
  }

  // recursively get area
  double get_a(const iPoint &p0){
    double a=S.area(p0) * 1e-6; // dot area in km^2

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

  // sort rivers
  std::list<std::list<iPoint> > sort_areas(){

    std::list<std::list<iPoint> > ret;
    std::set<iPoint> done;

    while(1){
      std::list<iPoint> riv;

      // find max area
      double ma = -1;
      iPoint mp;
      std::map<iPoint,double>::const_iterator ai;
      std::map<iPoint,char>::const_iterator di;
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

/********************************************************************/
// trace rectangular map

// построение прямоугольной хребтовки
void
go_down(SRTM3 & S, const iPoint & p00, int x, int y,
        cImage & dirs, int nmax, bool down){

  // проход вниз
  trace_gear G(S, p00+iPoint(x,y));
  if (G.h0 < srtm_min) return; // мы вне карты

  std::vector<iPoint> L; // упорядоченный список просмотренных точек
  L.push_back(G.p0);

  iPoint p;
  do {
    p=G.go(down);
    L.push_back(p);
    // если мы уже ушли далеко, а шаг не сделали, то точка
    // последнего шага - бессточная, вернемся в нее
    if (G.ns>nmax) { p=G.pp; dirs.set(p-p00,8); break; }

  // идем до края карты или уже обработанной точки
  } while (dirs.safe_get(p-p00,-2)==-1);

  // обратный проход от p до p0
  while (p!=G.p0){
    std::vector<iPoint>::const_iterator b;
    for (b = L.begin(); b!=L.end(); b++){
      int dir = isadjacent(*b, p);
      if (dir==-1) continue;
      p=*b;
      dirs.set(p-p00, dir);
      break;
    }
  }
}

dImage
trace_map(SRTM3 & S, const iPoint & p00,
          int w, int h, int nmax, bool down){

  cImage dirs(w,h,-1); // направления стока

  // для каждой необработанной точки карты трассируем
  // путь вниз до уже обработанной точки, до края или
  // до бессточной области
  for (int y=0; y<h; y++){
    for (int x=0; x<w; x++){
      if (dirs.get(x,y)!=-1) continue;
      go_down(S, p00, x,y, dirs, nmax, down);
    }
  }

  // определение площадей водосбора
  dImage areas(w,h,0); // площади водосбора
  double area = S.area(p00)/1e6; // примерная площадь одной точки
  for (int y=0; y<h; y++){
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
  return areas;
}

#endif
