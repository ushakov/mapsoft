#ifndef AM_FILTER_TRACK_H
#define AM_FILTER_TRACK_H

#include "action_mode.h"
#include "../widgets.h"

class FilterTrack : public ActionMode {
public:
    FilterTrack (Mapview * mapview) : ActionMode(mapview) {
      dlg.signal_response().connect(
        sigc::mem_fun (this, &FilterTrack::on_result));
      dlg.set_title(get_name());
    }

    std::string get_name() { return "Filter Track"; }

    void abort() {dlg.hide_all();}

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      int d = find_tpt(p, &layer, true);
      if (d < 0) return;

      dlg.set_info(layer->get_data());
      dlg.show_all();
    }

private:
    LayerTRK * layer;
    DlgTrkFilter dlg;

    void on_result(int r) {
      if (r!=Gtk::RESPONSE_OK) return;
      filters::generalize(layer->get_data(), dlg.get_acc(), dlg.get_num());
      if (dlg.get_rg()){
        g_track::iterator i = layer->get_data()->begin();
        for (i++; i!= layer->get_data()->end(); i++) i->start=false;
      }
      mapview->workplane.refresh_layer(layer);
    }
};

#endif /* AM_FILTER_TRACK_H */
