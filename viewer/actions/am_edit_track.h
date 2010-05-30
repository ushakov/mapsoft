#ifndef AM_EDIT_TRACK_H
#define AM_EDIT_TRACK_H

#include <sstream>

#include "action_mode.h"
#include "../generic_dialog.h"
#include "../mapview.h"

class EditTrack : public ActionMode {
public:
    EditTrack (Mapview * mapview_) : mapview(mapview_) {
	current_track = 0;
    }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Edit Track";
    }

    // Activates this mode.
    virtual void activate() { }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() { }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p, const Gdk::ModifierType & state) {
	std::cout << "EDITTRACK: " << p << std::endl;
	for (int i = 0; i < mapview->trk_layers.size(); ++i) {
	    current_layer = dynamic_cast<LayerTRK *> (mapview->trk_layers[i].get());
            if (!mapview->viewer.workplane.get_layer_active(current_layer)) continue;
	    assert (current_layer);
	    std::pair<int, int> d = current_layer->find_track(p);
	    if (d.first >= 0) {
		std::cout << "EDITTRACK: found at " << current_layer << std::endl;
		current_track = &(current_layer->get_world()->trks[d.first]);
		Options opt = current_track->to_options();

		std::ostringstream st;
		st << "Editing track... "
		   << current_track->size() << " points, "
                   << current_track->length()/1000 << " km";
		mapview->statusbar.push(st.str(),0);

		mapview->gend.activate(get_name(), opt,
		  sigc::mem_fun(this, &EditTrack::on_result));
		break;
	    }
	}
    }

private:
    Mapview       * mapview;
    g_track       * current_track;
    LayerTRK      * current_layer;

    void on_result(int r, const Options & o) {
	if (current_track) {
	    if (r == 0) { // OK
		current_track->parse_from_options(o);
                mapview->viewer.workplane.refresh_layer(current_layer);
 		std::cout << "EDITTRACK: " << current_track->comm << std::endl;
	    } else {
		// do nothing
	    }
	    mapview->statusbar.push("",0);
	    current_track = 0;
	}
    }
};

#endif /* AM_EDIT_TRACK_H */
