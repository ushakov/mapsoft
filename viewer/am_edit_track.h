#ifndef AM_EDIT_TRACK_H
#define AM_EDIT_TRACK_H

#include <viewer/action_mode.h>
#include <viewer/action_manager.h>
#include <viewer/generic_dialog.h>
#include <programs/mapview.h>

class EditTrack : public ActionMode {
public:
    EditTrack (MapviewState * state_) : state(state_) {
      	gend = GenericDialog::get_instance();
	current_track = 0;
    }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Edit Track";
    }

    // Activates this mode.
    virtual void activate() { }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() {
	gend->deactivate();
    }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(Point<int> p) {
	std::cout << "EDITTRACK: " << p << std::endl;
	for (int i = 0; i < state->data_layers.size(); ++i) {
	    current_layer = dynamic_cast<LayerGeoData *> (state->data_layers[i].get());
            if (!state->workplane->get_layer_active(current_layer)) continue;
	    assert (current_layer);
	    std::pair<int, int> d = current_layer->find_track(p);
	    if (d.first >= 0) {
		std::cout << "EDITTRACK: found at " << current_layer << std::endl;
		current_track = &(current_layer->get_world()->trks[d.first]);
		Options opt = current_track->to_options();

	        current_connection = gend->signal_result().connect(sigc::mem_fun(this, &EditTrack::on_result));
		gend->activate(get_name(), opt);
		break;
	    }
	}
    }

private:
    MapviewState * state;
    GenericDialog * gend;
    g_track * current_track;
    LayerGeoData * current_layer;
    sigc::connection current_connection;

    void on_result(int r) {
	if (current_track) {
	    if (r == 0) { // OK
		current_track->parse_from_options(gend->get_options());
                state->workplane->refresh_layer(current_layer);
 		std::cout << "EDITTRACK: " << current_track->comm << std::endl;
	    } else {
		// do nothing
	    }
	    current_track = 0;
            current_connection.disconnect();
	}
    }
};

#endif /* AM_EDIT_TRACK_H */
