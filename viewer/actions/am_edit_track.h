#ifndef AM_EDIT_TRACK_H
#define AM_EDIT_TRACK_H

#include "action_mode.h"
#include "../widgets.h"

class EditTrack : public ActionMode {
public:
    EditTrack (Mapview * mapview) : ActionMode(mapview) {
      dlg.signal_response().connect(
        sigc::mem_fun (this, &EditTrack::on_result));
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Edit Track"; }

    void abort() {dlg.hide_all();}

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      int d = find_tpt(p, &layer, true);
      if (d < 0) return;
      g_track * track = layer->get_data();

      dlg.trk2dlg(track);
      dlg.show_all();
    }

private:
    LayerTRK      * layer;
    DlgTrk        dlg;

    void on_result(int r) {
      if (r!=Gtk::RESPONSE_OK) return;
      dlg.dlg2trk(layer->get_data());
      mapview->workplane.refresh_layer(layer);
      mapview->update_ll_comm(layer);
    }
};

#endif /* AM_EDIT_TRACK_H */
