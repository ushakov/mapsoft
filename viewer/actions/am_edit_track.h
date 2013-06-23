#ifndef AM_EDIT_TRACK_H
#define AM_EDIT_TRACK_H

#include "action_mode.h"
#include "../dialogs/trk.h"

class EditTrack : public ActionMode {
public:
    EditTrack (Mapview * mapview) : ActionMode(mapview) {
      dlg.set_transient_for(*mapview);
      dlg.signal_response().connect(
        sigc::mem_fun (this, &EditTrack::on_result));
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Edit Track"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::EDIT; }

    void abort() {dlg.hide_all();}

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      int d = mapview->layer_trks.panel.find_tpt(p, &gobj, true);
      if (d < 0) return;
      g_track * track = gobj->get_data();

      dlg.trk2dlg(track);
      dlg.show_all();
    }

private:
    GObjTRK      * gobj;
    DlgTrk        dlg;

    void on_result(int r) {
      if (r!=Gtk::RESPONSE_OK) return;
      dlg.dlg2trk(gobj->get_data());
      mapview->set_changed();
      mapview->layer_trks.gobj.refresh_gobj(gobj);
      mapview->layer_trks.panel.upd_comm(gobj, false); // move comm to layerlist
    }
};

#endif /* AM_EDIT_TRACK_H */
