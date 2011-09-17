#ifndef AM_DELETE_WPT_H
#define AM_DELETE_WPT_H

#include "action_mode.h"
#include "../generic_dialog.h"

class DeleteWaypoint : public ActionMode {
public:
    DeleteWaypoint (Mapview * mapview) : ActionMode(mapview) {}

    std::string get_name() { return "Delete Waypoint"; }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      pt_num=find_wpt(p, &layer);
      if (pt_num < 0) return;
      mapview->gend.activate("Delete Waypoint?", Options(),
        sigc::mem_fun(this, &DeleteWaypoint::on_result));
    }

private:
    int pt_num;
    LayerWPT * layer;

    void on_result(int r, const Options & o) {
      if ((r != 0) || (pt_num<0)) return;
      layer->get_data()->erase(layer->get_data()->begin() + pt_num);
      mapview->workplane.refresh_layer(layer);
    }
};

#endif /* AM_DELETE_WPT_H */
