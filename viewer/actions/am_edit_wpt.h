#ifndef AM_EDIT_WPT_H
#define AM_EDIT_WPT_H

#include "action_mode.h"
#include "../generic_dialog.h"

class EditWaypoint : public ActionMode {
public:
    EditWaypoint (Mapview * mapview) : ActionMode(mapview) { }

    std::string get_name() { return "Edit Waypoint"; }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      pt_num=find_wpt(p, &layer);
      if (pt_num < 0) return;

      g_waypoint * wpt = &(* layer->get_data())[pt_num];
      mapview->gend.activate(get_name(), wpt->to_options(),
        sigc::mem_fun(this, &EditWaypoint::on_result));
    }

private:
    int pt_num;
    LayerWPT * layer;

    void on_result(int r, const Options & o) {
      if ((pt_num<0) || (r!=0)) return;
      g_waypoint * wpt = layer->get_pt(pt_num);
      wpt->parse_from_options(o);
      mapview->workplane.refresh_layer(layer);
    }
};

#endif /* AM_EDIT_WPT_H */
