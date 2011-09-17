#ifndef AM_EDIT_TRACK_H
#define AM_EDIT_TRACK_H

#include <sstream>

#include "action_mode.h"
#include "../generic_dialog.h"

class EditTrack : public ActionMode {
public:
    EditTrack (Mapview * mapview) : ActionMode(mapview) { }

    std::string get_name() { return "Edit Track"; }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      track = 0;
      LayerTRK * layer;
      int d = find_tpt(p, &layer, true);
      if (d < 0) return;
      track = layer->get_data();

      std::ostringstream st;
      st << "Editing track... "
         << track->size() << " points, "
         << track->length()/1000 << " km";
      mapview->statusbar.push(st.str(),0);

      mapview->gend.activate(get_name(), track->to_options(),
        sigc::mem_fun(this, &EditTrack::on_result));
    }

private:
    g_track       * track;
    LayerTRK      * layer;

    void on_result(int r, const Options & o) {
      if ((track==0) || (r!=0)) return;
      track->parse_from_options(o);
      mapview->workplane.refresh_layer(layer);
    }
};

#endif /* AM_EDIT_TRACK_H */
