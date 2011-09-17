#ifndef AM_EDIT_TPT_H
#define AM_EDIT_TPT_H

#include "action_mode.h"
#include "../generic_dialog.h"

class EditTrackpoint : public ActionMode {
public:
    EditTrackpoint (Mapview * mapview) : ActionMode(mapview) { }

    std::string get_name() { return "Edit Trackpoint"; }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      int pt_num = find_tpt(p, &layer);
      if (pt_num < 0) return;
      tpt = layer->get_pt(pt_num);
      mapview->gend.activate(get_name(), tpt->to_options(),
        sigc::mem_fun(this, &EditTrackpoint::on_result));
    }

private:
    g_trackpoint * tpt;
    LayerTRK * layer;

    void on_result(int r, const Options & o) {
      if ((!tpt) || (r!=0)) return;
      tpt->parse_from_options(o);
      mapview->workplane.refresh_layer(layer);
    }
};

#endif /* AM_EDIT_TPT_H */
