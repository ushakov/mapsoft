#ifndef AM_DELETE_WPT_H
#define AM_DELETE_WPT_H

#include "action_mode.h"
#include <sstream>

class DeleteWaypoint : public ActionMode {
public:
    DeleteWaypoint (Mapview * mapview) : ActionMode(mapview) {
      dlg.signal_response().connect(
        sigc::mem_fun (this, &DeleteWaypoint::on_result));
    }

    std::string get_name() { return "Delete Waypoint"; }

    void handle_click(iPoint p, const Gdk::ModifierType & state) {
      pt_num=find_wpt(p, &layer);
      if (pt_num < 0) return;
      dlg.set_name(layer->get_pt(pt_num)->name);
      dlg.show_all();
    }

private:
    int pt_num;
    LayerWPT * layer;
    DlgDeleteWpt dlg;

    void on_result(int r) {
      if ((r != Gtk::RESPONSE_OK) || (pt_num<0)) return;
      layer->get_data()->erase(layer->get_data()->begin() + pt_num);
      mapview->workplane.refresh_layer(layer);
    }
};

#endif /* AM_DELETE_WPT_H */
