#ifndef AM_EDIT_WPT_H
#define AM_EDIT_WPT_H

#include <viewer/action_mode.h>
#include <viewer/action_manager.h>
#include <viewer/generic_dialog.h>
#include <programs/mapview.h>

class EditWaypoint : public ActionMode {
public:
    EditWaypoint (MapviewState * state_) : state(state_) {
	gend = GenericDialog::get_instance();
	gend->signal_result().connect(sigc::mem_fun(this, &EditWaypoint::on_result));
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
	gend->deactivate();
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(Point<int> p) {
	std::cout << "EDITWPT: " << p << std::endl;
	for (int i = 0; i < state->data_layers.size(); ++i) {
	    LayerGeoData * layer = dynamic_cast<LayerGeoData *> (state->data_layers[i].get());
	    assert (layer);
	    std::pair<int, int> d = layer->find_waypoint(p);
	    if (d.first >= 0) {
		std::cout << "EDITWPT: found at " << layer << std::endl;
		current_wpt = &(layer->get_world()->wpts[d.first][d.second]);
		Options opt = current_wpt->to_options();
		gend->activate("Edit Waypoint", opt);
	    }
	}
    }

private:
    MapviewState * state;
    g_waypoint * current_wpt;
    GenericDialog * gend;

    void on_result(int r) {
	if (current_wpt) {
	    if (r == 0) { // OK
		current_wpt->parse_from_options(gend->get_options());
	    } else {
		// do nothing
	    }
	    current_wpt = 0;
	}
    }
};

#endif /* AM_EDIT_WPT_H */
