#ifndef AM_EDIT_TPT_H
#define AM_EDIT_TPT_H

#include "action_mode.h"
#include "../generic_dialog.h"
#include "../mapview.h"

class EditTrackpoint : public ActionMode {
public:
    EditTrackpoint (Mapview * state_) : state(state_) {
	current_tpt = 0;
    }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Edit Trackpoint";
    }

    // Activates this mode.
    virtual void activate() { }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() { }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p) {
	std::cout << "EDITTPT: " << p << std::endl;
	for (int i = 0; i < state->trk_layers.size(); ++i) {
	    current_layer = dynamic_cast<LayerTRK *> (state->trk_layers[i].get());
            if (!state->viewer.workplane.get_layer_active(current_layer)) continue;
	    assert (current_layer);
	    std::pair<int, int> d = current_layer->find_trackpoint(p);
	    if (d.first >= 0) {
		std::cout << "EDITTPT: found at " << current_layer << std::endl;
		current_tpt = &(current_layer->get_world()->trks[d.first][d.second]);
		Options opt = current_tpt->to_options();

		state->gend.activate(get_name(), opt,
		  sigc::mem_fun(this, &EditTrackpoint::on_result));
                break;
	    }
	}
    }

private:
    Mapview       * state;
    g_trackpoint  * current_tpt;
    LayerTRK      * current_layer;

    void on_result(int r, const Options & o) {
	if (current_tpt) {
	    if (r == 0) { // OK
		current_tpt->parse_from_options(o);
                state->viewer.workplane.refresh_layer(current_layer);
 		std::cout << "EDITWPT: OK\n";
	    } else {
		// do nothing
	    }
	    current_tpt = 0;
	}
    }
};

#endif /* AM_EDIT_TPT_H */
