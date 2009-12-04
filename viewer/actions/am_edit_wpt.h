#ifndef AM_EDIT_WPT_H
#define AM_EDIT_WPT_H

#include "action_mode.h"
#include "../generic_dialog.h"
#include "../mapview.h"

class EditWaypoint : public ActionMode {
public:
    EditWaypoint (Mapview * state_) : state(state_) {
	current_wpt = 0;
    }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Edit Waypoint";
    }

    // Activates this mode.
    virtual void activate() { }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() {
	state->gend.deactivate();
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p) {
	std::cout << "EDITWPT: " << p << std::endl;
	for (int i = 0; i < state->wpt_layers.size(); ++i) {
	    current_layer = dynamic_cast<LayerWPT *> (state->wpt_layers[i].get());
            if (!state->viewer->workplane.get_layer_active(current_layer)) continue;
	    assert (current_layer);
	    std::pair<int, int> d = current_layer->find_waypoint(p);
	    if (d.first >= 0) {
		std::cout << "EDITWPT: found at " << current_layer << std::endl;
		current_wpt = &(current_layer->get_world()->wpts[d.first][d.second]);
		Options opt = current_wpt->to_options();

		state->gend.activate(get_name(), opt,
		   sigc::mem_fun(this, &EditWaypoint::on_result));
		break;
	    }
	}
    }

private:
    Mapview       * state;
    g_waypoint    * current_wpt;
    LayerWPT      * current_layer;

    void on_result(int r) {
	if (current_wpt) {
	    if (r == 0) { // OK
		current_wpt->parse_from_options(state->gend.get_options());
                state->viewer->workplane.refresh_layer(current_layer);
 		std::cout << "EDITWPT: " << current_wpt->name << std::endl;
	    } else {
		// do nothing
	    }
	    current_wpt = 0;
	}
    }
};

#endif /* AM_EDIT_WPT_H */
