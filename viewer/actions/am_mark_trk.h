#ifndef AM_MARK_TRK_H
#define AM_MARK_TRK_H

#include <boost/lexical_cast.hpp>
#include <sstream>
#include <iomanip>

#include "action_mode.h"
#include "2d/line_dist.h"

class MarkTrack : public ActionMode {
public:
     MarkTrack (Mapview * mapview) : ActionMode(mapview) { }

    // Returns name of the mode as string.
    std::string get_name() {return "Mark Track's length"; }

    void add_wpt (g_waypoint_list * wpt_list, dPoint where, double what) {
	std::ostringstream ss;
	g_waypoint wpt;
	wpt.bgcolor=0xFF00FFFF;
	ss << std::fixed << std::setprecision(1) << what/1000 << "km";
	wpt.name = ss.str();
	wpt.dPoint::operator=(where);
	wpt_list->push_back(wpt);
    }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      LayerTRK * layer;
      int d = find_tpt(p, &layer, true);
      if (d < 0) return;
      g_track * track = layer->get_data();

      bool go_back = 2*d > track->size();

      Options o;
      o.put("lon0", convs::lon2lon0((*track)[0].x));
      convs::pt2pt cnv(Datum("wgs84"), Proj("tmerc"), o,
                       Datum("wgs84"), Proj("lonlat"), Options());

      boost::shared_ptr<g_waypoint_list> wpt_list(new g_waypoint_list);
      wpt_list->comm="track marks";

      dLine gk_track=(dLine)(*track);
      if (go_back) gk_track = gk_track.inv();
      cnv.line_bck_p2p(gk_track);

      LineDist ld(gk_track);
      while (1){
        dPoint p=ld.pt(); cnv.frw(p);
        add_wpt (wpt_list.get(), p, ld.dist());
        if (ld.is_end()) break;
        ld.move_frw(1000);
      }
      mapview->add_wpts(wpt_list);
    }
};


#endif /* AM_MARK_TRK_H */
