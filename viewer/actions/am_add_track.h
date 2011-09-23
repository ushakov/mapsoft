#ifndef AM_ADD_TRACK_H
#define AM_ADD_TRACK_H

#include "action_mode.h"
#include "../widgets.h"

class AddTrack : public ActionMode {
public:
    AddTrack (Mapview * mapview) : ActionMode(mapview) {
      dlg.signal_response().connect(
        sigc::mem_fun (this, &AddTrack::on_result));
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Add Track"; }

    void activate() { abort(); }

    void abort() {
      trk.clear();
      trk.comm="";
      mapview->rubber.clear();
    }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
         if (!mapview->have_reference){
           mapview->statusbar.push("No geo-reference", 0);
           return;
         }

         if (trk.size() == 0){
           dlg.trk2dlg(&trk);
           dlg.show_all();
         }

        if (state&Gdk::CONTROL_MASK){ // remove point
          if (trk.size()>0) trk.pop_back();
          if (mapview->rubber.size()>0){
            mapview->rubber.pop();
          }
          if (mapview->rubber.size()>0){
            RubberSegment s = mapview->rubber.pop();
            s.flags |= RUBBFL_MOUSE_P2;
            s.p2=iPoint(0,0);
            mapview->rubber.add(s);
          }
        }
        else{ // add point
          if (mapview->rubber.size()>0){
            RubberSegment s = mapview->rubber.pop();
            s.flags &= ~RUBBFL_MOUSE;
            s.p2 = p;
            mapview->rubber.add(s);
          }
          mapview->rubber.add_line(p);
          g_map map = mapview->reference;
          convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"));

          g_trackpoint pt;
          pt.dPoint::operator=(p);
	  cnv.frw(pt);
	  trk.push_back(pt);
        }

        dlg.set_info(&trk);
    }

private:
    g_track trk;
    DlgTrk dlg;

    void on_result(int r){
      if (r == Gtk::RESPONSE_OK){
        dlg.dlg2trk(&trk);
        boost::shared_ptr<g_track> track(new g_track(trk));
        mapview->add_trks(track, false);
      }
      abort();
    }
};

#endif /* AM_ADD_TRACK_H */
