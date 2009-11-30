#ifndef AM_DELETE_TPT_H
#define AM_DELETE_TPT_H

#include "action_mode.h"
#include "action_manager.h"
#include "mapview.h"

class DeleteTrackpoint : public ActionMode {
public:
    DeleteTrackpoint (Mapview * state_, Viewer * viewer_) : state(state_), viewer(viewer_) { }

    // Returns name of the mode as string.
    virtual std::string get_name() {
	return "Delete Trackpoint";
    }

    // Activates this mode.
    virtual void activate() { }

    // Abandons any action in progress and deactivates mode.
    virtual void abort() { }

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p) {
	std::cout << "DELETETPT: " << p << std::endl;

        for (int i = 0; i < state->trk_layers.size(); ++i) {
            LayerTRK * current_layer = dynamic_cast<LayerTRK *> (state->trk_layers[i].get());
	    if (!viewer->workplane.get_layer_active(current_layer)) continue;
            assert (current_layer);
            std::pair<int, int> point_addr = current_layer->find_trackpoint(p);
            if (point_addr.first >= 0) {
                std::cout << "DELETETPT: found at " << current_layer << std::endl;
		bool start = current_layer->get_world()->trks[point_addr.first][point_addr.second].start;
                current_layer->get_world()->trks[point_addr.first].erase(
                  current_layer->get_world()->trks[point_addr.first].begin()+point_addr.second);
		if (start && (point_addr.second < current_layer->get_world()->trks[point_addr.first].size()))
                  current_layer->get_world()->trks[point_addr.first][point_addr.second].start = start;
                viewer->workplane.refresh_layer(current_layer);
                break;
            }
        }
    }

private:
    Mapview * state;
    Viewer  * viewer;
};

#endif /* AM_DELETE_TPT_H */
