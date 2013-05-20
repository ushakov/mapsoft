#ifndef AM_SRTM_RIV_H
#define AM_SRTM_RIV_H

#include "action_mode.h"
#include <srtm/trace_gear.h>

// see misc/trace_one.cpp
std::vector<iPoint>
trace(srtm3 & S, const iPoint & p0, int nmax, int hmin, bool down){

  trace_gear G(S, p0);
  if (G.h0 < srtm_min) return std::vector<iPoint>(); // мы вне карты

  std::vector<iPoint> L; // упорядоченный список просмотренных точек
  L.push_back(p0);

  do {
    L.push_back(G.go(down));
    // если мы уже ушли далеко, а шаг не сделали, то точка
    // последнего шага - бессточная. Возвращаемся к ней.
    if (G.ns > nmax) { L.push_back(G.pp); break; }

    // критерий выхода: до высоты hmin, или края данных
  } while (G.hp > hmin);

  iPoint p = L[L.size()-1];
  std::vector<iPoint> ret;
  // обратный проход от p до p0
  while (p!=p0){
    vector<iPoint>::const_iterator b;
    for (b = L.begin(); b!=L.end(); b++){
      if (isadjacent(*b, p)<0) continue;
      p=*b;
      ret.push_back(p);
      break;
    }
  }
  return ret;
}

//
class SrtmRiv : public ActionMode {
public:
    SrtmRiv (Mapview * mapview_) : ActionMode(mapview_) { }

    std::string get_name() { return "Trace river"; }

    void activate() { abort(); }

    void abort() {
      mystate=0;
    }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
        if (mystate==0){ // first point
          start_pt = p;
          mapview->cnv.frw(start_pt);
          mapview->rubber.add_src_sq(p, 2);
          mapview->rubber.add_dst_sq(2);
          mapview->rubber.add_line(p);
          mystate=1;
        } else { // second point, trace
          dPoint end_pt = p;
          mapview->cnv.frw(end_pt);
          int hmin = mapview->srtm.geth4(end_pt)-1;
          int nmax = 10000;
          bool down = true;
          std::vector<iPoint> points =
            trace(mapview->srtm, start_pt*1200, nmax, hmin, down);
          std::vector<iPoint>::const_iterator i;

          boost::shared_ptr<g_track> track(new g_track());
          for (i = points.begin(); i!=points.end(); i++){
            g_trackpoint pt;
            pt.dPoint::operator=(*i);
            pt/=1200;
            pt.z = mapview->srtm.geth4(pt);
            track->push_back(pt);
          }
          mapview->add_trks(track);
          mapview->rubber.clear();
          abort();
        }
    }

private:
    dPoint start_pt;
    int mystate; // 0 - start point, 1 - trace
};

#endif
