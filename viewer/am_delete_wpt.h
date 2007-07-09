#ifndef AM_DELETE_WPT_H
#define AM_DELETE_WPT_H

#include <viewer/action_mode.h>
#include <viewer/action_manager.h>
#include <viewer/generic_dialog.h>
#include <programs/mapview.h>

class DeleteWaypoint : public ActionMode {
public:
    DeleteWaypoint (MapviewState * state_) : state(state_) {   
	gend = GenericDialog::get_instance();
    }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Delete Waypoint";
    }

    // Activates this mode.
    virtual void activate() { }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() {
	gend->deactivate();
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(Point<int> p) {
	std::cout << "DELETEWPT: " << p << std::endl;

        for (int i = 0; i < state->data_layers.size(); ++i) {
            current_layer = dynamic_cast<LayerGeoData *> (state->data_layers[i].get());
	    if (!state->workplane->get_layer_active(current_layer)) continue;
            assert (current_layer);
            point_addr = current_layer->find_waypoint(p);
            if (point_addr.first >= 0) {
                std::cout << "DELETEWPT: found at " << current_layer << std::endl;

                current_connection = gend->signal_result().connect(sigc::mem_fun(this, &DeleteWaypoint::on_result));
                gend->activate("Delete Waypoint?", Options());
                break;
            }
        }
    }

private:
    std::pair<int, int> point_addr;
    MapviewState * state;
    GenericDialog * gend;
    LayerGeoData * current_layer;
    sigc::connection current_connection;

    void on_result(int r) {
	if (r == 0) { // OK
          current_layer->get_world()->wpts[point_addr.first].erase(
            current_layer->get_world()->wpts[point_addr.first].begin()+point_addr.second);
          state->workplane->refresh_layer(current_layer);
	} else {
	  // do nothing
	}
        current_connection.disconnect();
    }
};

#endif /* AM_DELETE_WPT_H */
