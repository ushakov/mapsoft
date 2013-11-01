#ifndef AM_ADD_TRACK_H
#define AM_ADD_TRACK_H

#include "action_mode.h"
#include "../dialogs/trk.h"

class AddTrack : public ActionMode {
public:
    AddTrack (Mapview * mapview) : ActionMode(mapview) {
      dlg.set_transient_for(*mapview);
      dlg.signal_response().connect(
        sigc::mem_fun (this, &AddTrack::on_result));
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Add Track"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::ADD; }

    void activate() { abort(); }

    void abort() {
      trk.clear();
      trk.comm="";
      mapview->rubber.clear();
      dlg.hide_all();
    }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
         if (!mapview->have_reference){
           mapview->statusbar.push("No geo-reference", 0);
           return;
         }

         if (trk.size() == 0){
           dlg.trk2dlg(&trk);
           dlg.set_hint("<u>Use mouse buttons to draw track:</u>\n"
                        "* <b>1.</b> Add point.\n"
                        "* <b>Ctrl-1.</b> Remove last point.\n"
                        "* <b>Shift-1.</b> Start new segment.\n"
                        "* <b>2.</b> Scroll map.\n"
                        "* <b>3.</b> Abort drawing.\n"
                        );
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

          g_trackpoint pt;
          pt.dPoint::operator=(p);
          mapview->get_cnv()->frw(pt);
          pt.start = (state&Gdk::SHIFT_MASK) || (trk.size()==0);
          trk.push_back(pt);

          if (mapview->rubber.size()>0){
            RubberSegment s = mapview->rubber.pop();
            s.flags &= ~RUBBFL_MOUSE;
            s.p2 = pt.start ? s.p1 : p;
            mapview->rubber.add(s);
          }
          mapview->rubber.add_line(p);
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
        mapview->panel_trks.add(track);
      }
      abort();
    }
};

#endif /* AM_ADD_TRACK_H */
