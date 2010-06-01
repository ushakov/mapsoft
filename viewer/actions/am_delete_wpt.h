#ifndef AM_DELETE_WPT_H
#define AM_DELETE_WPT_H

#include "action_mode.h"
#include "../generic_dialog.h"
#include "../mapview.h"

class DeleteWaypoint : public ActionMode {
public:
    DeleteWaypoint (Mapview * mapview_) : mapview(mapview_) {}

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Delete Waypoint";
    }

    // Activates this mode.
    virtual void activate() { }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() { }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p, const Gdk::ModifierType & state) {
	std::cout << "DELETEWPT: " << p << std::endl;

        for (int i = 0; i < mapview->wpt_layers.size(); ++i) {
            current_layer = dynamic_cast<LayerWPT *> (mapview->wpt_layers[i].get());
	    if (!mapview->workplane.get_layer_active(current_layer)) continue;
            assert (current_layer);
            point_addr = current_layer->find_waypoint(p);
            if (point_addr.first >= 0) {
                std::cout << "DELETEWPT: found at " << current_layer << std::endl;

                mapview->gend.activate("Delete Waypoint?", Options(),
                  sigc::mem_fun(this, &DeleteWaypoint::on_result));
                break;
            }
        }
    }

private:
    std::pair<int, int> point_addr;
    Mapview       * mapview;
    LayerWPT      * current_layer;

    void on_result(int r, const Options & o) {
	if (r == 0) { // OK
          current_layer->get_world()->wpts[point_addr.first].erase(
            current_layer->get_world()->wpts[point_addr.first].begin()+point_addr.second);
          mapview->workplane.refresh_layer(current_layer);
	}
    }
};

#endif /* AM_DELETE_WPT_H */
