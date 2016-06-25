#ifndef AM_SRTM_RIV_H
#define AM_SRTM_RIV_H

#include "action_mode.h"
#include <srtm/tracers.h>

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
          mapview->get_cnv()->frw(start_pt);
          mapview->rubber.add_src_sq(p, 2);
          mapview->rubber.add_dst_sq(2);
          mapview->rubber.add_line(p);
          mystate=1;
        } else { // second point, trace
          dPoint end_pt = p;
          mapview->get_cnv()->frw(end_pt);
          int hmin = mapview->panel_misc.srtm.geth4(end_pt)-1;
          int nmax = 10000;
          bool down = true;
          size_t srtmw = mapview->panel_misc.srtm.get_width()-1;
          std::vector<iPoint> points =
            trace_river(mapview->panel_misc.srtm, start_pt*srtmw, nmax, hmin, down);
          std::vector<iPoint>::const_iterator i;

          boost::shared_ptr<g_track> track(new g_track());
          for (i = points.begin(); i!=points.end(); i++){
            g_trackpoint pt;
            pt.dPoint::operator=(*i);
            pt/=srtmw;
            pt.z = mapview->panel_misc.srtm.geth(*i);
            track->push_back(pt);
          }
          mapview->panel_trks.add(track);
          mapview->rubber.clear();
          abort();
        }
    }

private:
    dPoint start_pt;
    int mystate; // 0 - start point, 1 - trace
};

#endif
