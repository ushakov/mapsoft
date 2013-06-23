#ifndef AM_MARK_TRK_H
#define AM_MARK_TRK_H

#include <boost/lexical_cast.hpp>
#include <sstream>
#include <iomanip>

#include "action_mode.h"
#include "2d/line_dist.h"
#include "../dialogs/trk_mark.h"

class MarkTrack : public ActionMode {
public:
    MarkTrack (Mapview * mapview) : ActionMode(mapview) {
      dlg.set_transient_for(*mapview);
      dlg.signal_response().connect(
        sigc::mem_fun (this, &MarkTrack::on_result));
      dlg.set_title(get_name());
    }

    std::string get_name() {return "Mark Track's length"; }

    void abort() {dlg.hide_all();}

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      GObjTRK * gobj;
      int d = mapview->find_tpt(p, &gobj, true);
      if (d < 0) return;
      track = gobj->get_data();

      dlg.set_rev(2*d > track->size());
      dlg.show_all();
    }

private:
    g_track * track;
    DlgMarkTrk dlg;

    void add_wpt (g_waypoint_list * wpt_list, dPoint where, double dist) {
      std::ostringstream ss;
      g_waypoint wpt;
      wpt.bgcolor=0xFF00FFFF;
      ss << std::fixed << std::setprecision(1) << dist/1000 << "km";
      wpt.name = ss.str();
      wpt.dPoint::operator=(where);
      wpt_list->push_back(wpt);
    }

    void on_result(int r) {
      if (r!=Gtk::RESPONSE_OK) return;

      Options o;
      o.put("lon0", convs::lon2lon0((*track)[0].x));
      convs::pt2wgs cnv(Datum("wgs84"), Proj("tmerc"), o);

      boost::shared_ptr<g_waypoint_list> wpt_list(new g_waypoint_list);
      wpt_list->comm="track marks";

      dLine gk_track=(dLine)(*track);
      if (dlg.get_rev()) gk_track = gk_track.inv();
      cnv.line_bck_p2p(gk_track);

      LineDist ld(gk_track);
      double dist=dlg.get_dist();
      while (1){
        dPoint p=ld.pt(); cnv.frw(p);
        add_wpt (wpt_list.get(), p, ld.dist());
        if (ld.is_end()) break;
        ld.move_frw(dist*1000);
      }
      mapview->add_wpts(wpt_list);
    }

};


#endif /* AM_MARK_TRK_H */
