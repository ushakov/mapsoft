#ifndef AM_SRTM_AREA_H
#define AM_SRTM_AREA_H

#include "action_mode.h"
#include <srtm/tracers.h>
#include "../dialogs/trace.h"

class SrtmArea : public ActionMode {

    DlgTrace dlg;
    iPoint   pt0;
    int      mystate;

public:
    SrtmArea (Mapview * mapview_) : ActionMode(mapview_) {
      dlg.set_transient_for(*mapview);
      dlg.signal_response().connect(
        sigc::mem_fun (this, &SrtmArea::on_response));
      dlg.set_title(get_name());
      mystate=0;
    }
    std::string get_name() { return "Trace area"; }
    void activate() { dlg.show_all(); mystate=0;}
    void abort()    { mystate=0;}
    void on_response(int r){ dlg.hide_all(); }


    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      Options o = dlg.get_opt();

      bool   down = o.get<bool>("down", true);
      double mina = o.get<double>("ma",   0.5);
      double ht   = o.get<double>("ht",   200);
      double at   = o.get<double>("at",   1000);
      double pst  = o.get<bool>("point_start", true);

      size_t srtmw = mapview->panel_misc.srtm.get_width()-1;

      // convert user point to srtm integer coordinates
      dPoint gpt(p);
      mapview->get_cnv()->frw(gpt);
      iPoint pt(gpt*srtmw);

      // if wee need a line - save first point
      if (!pst){
        if (mystate==0){ // 1st point
          mapview->rubber.add_line(p);
          pt0=pt;
          mystate=1;
          return;
        }
        mapview->rubber.clear();
        mystate=0;
      }

      trace_area T(mapview->panel_misc.srtm, ht, at, mina, down);

      // find exect starting point
      if (pst){
        // go to local min/max (not more then 5 steps)
        mapview->panel_misc.srtm.move_to_extr(pt,down,5);
      }
      else {
        pt = T.set_stop_segment(pt0,pt);
      }

      // run the calculation
      double area = T.get_a(pt);

      // sort rivers
      std::list<std::list<iPoint> > rivs = T.sort_areas();

      // make track
      boost::shared_ptr<g_track> track(new g_track());
      std::list<std::list<iPoint> >::iterator ri;
      for (ri = rivs.begin(); ri!=rivs.end(); ri++){
      std::list<iPoint>::iterator pi;

        // move to tracer??
        iPoint pt1 = *ri->begin();
        if (T.dirs.count(pt1))
          ri->push_front(adjacent(pt1, T.dirs[pt1]+4));

        for (pi = ri->begin(); pi!=ri->end(); pi++){
          g_trackpoint tpt;
          tpt.dPoint::operator=(*pi);
          tpt/=srtmw;
          tpt.z = mapview->panel_misc.srtm.geth(*pi);
          if (pi==ri->begin()) tpt.start=true;
          track->push_back(tpt);
        }
      }
      track->comm = "River";
      mapview->panel_trks.add(track);

      // border
      {
      dMultiLine B = pset2line(T.done);
      boost::shared_ptr<g_track> btrack(new g_track());
      dMultiLine::const_iterator li;
      for (li=B.begin(); li!=B.end(); li++){
        dLine::const_iterator pi;
        for (pi = li->begin(); pi!=li->end(); pi++){
          g_trackpoint tpt;
          tpt.dPoint::operator=(*pi);
          tpt/=srtmw;
          tpt.z = mapview->panel_misc.srtm.geth(*pi);
          if (pi == li->begin()) tpt.start=true;
          btrack->push_back(tpt);
        }
      }
      btrack->comm = boost::lexical_cast<std::string>(area);
      btrack->color = 0xFFFFFF00;
      mapview->panel_trks.add(btrack);
      }


      // stop segment
      {
      dMultiLine C = pset2line(T.stop);
      boost::shared_ptr<g_track> ctrack(new g_track());
      dMultiLine::const_iterator li;
      for (li=C.begin(); li!=C.end(); li++){
        dLine::const_iterator pi;
        for (pi = li->begin(); pi!=li->end(); pi++){
          g_trackpoint tpt;
          tpt.dPoint::operator=(*pi);
          tpt/=srtmw;
          tpt.z = mapview->panel_misc.srtm.geth(*pi);
          if (pi == li->begin()) tpt.start=true;
          ctrack->push_back(tpt);
        }
      }
      ctrack->comm = "Stop segment";
      ctrack->color = 0xFF00FFFF;
      mapview->panel_trks.add(ctrack);
      }


      mapview->rubber.clear();
      abort();
    }
};

#endif
