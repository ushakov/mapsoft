#ifndef AM_SRTM_AREA_H
#define AM_SRTM_AREA_H

#include "action_mode.h"
#include <srtm/tracers.h>

class SrtmArea : public ActionMode {
public:
    SrtmArea (Mapview * mapview_) : ActionMode(mapview_) { }

    std::string get_name() { return "Trace area"; }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {

      bool down = true;
      double mina = 0.5;
      int maxp = down?10000:10000;// макс. размер "неправильного" стока
      int dh   = down?200:200;  // макс. разница высот "неправильного" стока

      dPoint gpt(p);
      mapview->cnv.frw(gpt);
      iPoint pt(gpt*1200.0);

      trace_area T(mapview->srtm, dh, maxp, mina, down);

      // go to local min/max
      if (down) mapview->srtm.move_to_min(pt);
      else      mapview->srtm.move_to_max(pt);

      // calculate area
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
          tpt/=1200;
          tpt.z = mapview->srtm.geth(*pi);
          if (pi==ri->begin()) tpt.start=true;
          track->push_back(tpt);
        }
      }

      // todo - border

      mapview->add_trks(track);
      mapview->rubber.clear();
      abort();
    }
};

#endif
