#ifndef AM_FILTER_TRACK_H
#define AM_FILTER_TRACK_H

#include "action_mode.h"
#include "../dialogs/trk_filter.h"

class FilterTrack : public ActionMode {
public:
    FilterTrack (Mapview * mapview) : ActionMode(mapview) {
      dlg.set_transient_for(*mapview);
      dlg.signal_response().connect(
        sigc::mem_fun (this, &FilterTrack::on_result));
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Filter Track"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::CONVERT; }

    void abort() {dlg.hide_all();}

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      int d = mapview->layer_trks.panel.find_tpt(p, &gobj, true);
      if (d < 0) return;

      dlg.set_info(gobj->get_data());
      dlg.show_all();
    }

private:
    GObjTRK * gobj;
    DlgTrkFilter dlg;

    void on_result(int r) {
      if (r!=Gtk::RESPONSE_OK) return;
      io::generalize(gobj->get_data(), dlg.get_acc(), dlg.get_num());
      if (dlg.get_rg()){
        g_track::iterator i = gobj->get_data()->begin();
        for (i++; i!= gobj->get_data()->end(); i++) i->start=false;
      }
      mapview->set_changed();
      mapview->layer_trks.gobj.refresh_gobj(gobj);
    }
};

#endif /* AM_FILTER_TRACK_H */
